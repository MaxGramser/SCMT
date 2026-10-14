#include "now_playing.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

namespace esphome {
namespace now_playing {

using tessera::Font;
namespace ui = tessera::ui;

// tessera::tap_point() says where the finger was. Read its x and y whether it is a struct with x/y or a pair.
template <typename P>
static auto tap_x_(const P &p, int) -> decltype(static_cast<int>(p.x)) {
  return static_cast<int>(p.x);
}
template <typename P>
static auto tap_x_(const P &p, long) -> decltype(static_cast<int>(p.first)) {
  return static_cast<int>(p.first);
}
template <typename P>
static auto tap_y_(const P &p, int) -> decltype(static_cast<int>(p.y)) {
  return static_cast<int>(p.y);
}
template <typename P>
static auto tap_y_(const P &p, long) -> decltype(static_cast<int>(p.second)) {
  return static_cast<int>(p.second);
}

// The pictures of Home Assistant that a plugin tile may show (FIRMWARE_API.md, "Pictures on a tile").
static bool picture_allowed_(const char *url) {
  return std::strncmp(url, "/api/image/serve/", 17) == 0 || std::strncmp(url, "/api/image_proxy/", 17) == 0 ||
         std::strncmp(url, "/local/", 7) == 0;
}

// Seconds as 1:36, or 1:02:03 from an hour on.
static std::string fmt_time_(int sec) {
  if (sec < 0) sec = 0;
  char b[16];
  const int h = sec / 3600, m = (sec / 60) % 60, s = sec % 60;
  if (h > 0)
    snprintf(b, sizeof b, "%d:%02d:%02d", h, m, s);
  else
    snprintf(b, sizeof b, "%d:%02d", m, s);
  return b;
}

enum Btn : uint8_t { PREV, PLAY_PAUSE, NEXT, VOL_DOWN, VOL_UP };

static const char *service_for(Btn b) {
  switch (b) {
    case PREV: return "media_player.media_previous_track";
    case PLAY_PAUSE: return "media_player.media_play_pause";
    case NEXT: return "media_player.media_next_track";
    case VOL_DOWN: return "media_player.volume_down";
    case VOL_UP: return "media_player.volume_up";
  }
  return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------
// Tile 1: cover, title, artist, progress bar and optional buttons of a media_player entity.
// A tap on the tile plays or pauses; a tap on a button does what the button says.
// ---------------------------------------------------------------------------------------------------------------------
class NowPlayingTile : public tessera::Tile {
 public:
  explicit NowPlayingTile(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override {
    width_ = c.width;
    height_ = c.height;
    entity_ = c.entity;
    const std::string timeline = (const char *) (c.options["timeline"] | "bar");
    show_bar_ = timeline == "bar" || timeline == "both";
    show_time_ = timeline == "time" || timeline == "both";
    const std::string vd = (const char *) (c.options["volume_display"] | "change");
    vol_mode_ = vd == "always" ? 2 : (vd == "off" ? 0 : 1);
    const bool want_cover = c.options["cover"] | true;
    const bool want_controls = c.options["controls"] | false;
    const bool want_volume = c.options["volume"] | false;

    // Which buttons, in which order. They need a tile of two rows or more.
    n_ = 0;
    if (c.rows >= 2) {
      if (want_controls) {
        kinds_[n_++] = PREV;
        kinds_[n_++] = PLAY_PAUSE;
        kinds_[n_++] = NEXT;
      }
      if (want_volume) {
        kinds_[n_++] = VOL_DOWN;
        kinds_[n_++] = VOL_UP;
      }
    }
    ctrl_h_ = n_ > 0 ? ui::mm(8) : 0;
    if (ctrl_h_ * 2 > height_) {
      n_ = 0;
      ctrl_h_ = 0;
    }

    // From the bottom up: the buttons, the bar, the time. What is left above is for the cover and the text.
    const int gap = ui::px(4);
    bar_h_ = ui::px(4);
    const int time_h = show_time_ ? ui::line_height(Font::BODY) : 0;
    const int bottom = height_ - ctrl_h_ - (ctrl_h_ > 0 ? gap : 0);  // top of the buttons, or the tile's end
    bar_y_ = show_bar_ ? bottom - bar_h_ : bottom;
    time_y_ = (show_bar_ ? bar_y_ - gap : bottom) - time_h;
    int above = bottom;
    if (show_time_) above = time_y_;
    else if (show_bar_) above = bar_y_;
    content_h_ = (show_bar_ || show_time_) ? above - gap : bottom;
    if (content_h_ < 1) content_h_ = 1;

    // The cover, a square at the left, when the tile is two cells wide or more and the screen shows pictures.
    x0_ = 0;
    if (want_cover && ui::pictures() && c.columns >= 2 && content_h_ > ui::px(24)) {
      cover_size_ = content_h_;
      if (cover_size_ > width_ / 2) cover_size_ = width_ / 2;
      cover_ = ui::picture(c.parent);
      lv_obj_set_size(cover_, 0, 0);  // shown only when the player has a picture the screen may fetch
      lv_obj_set_pos(cover_, 0, 0);
    }
    text_w_ = width_;

    title_lbl_ = ui::label(c.parent, Font::HEADLINE, theme::INK);
    artist_lbl_ = ui::label(c.parent, Font::BODY, theme::INK_SOFT);
    lv_obj_set_width(title_lbl_, text_w_);
    lv_obj_set_width(artist_lbl_, text_w_);

    track_ = ui::block(c.parent, theme::TRACK);
    fill_ = ui::block(c.parent, theme::ACCENT);
    lv_obj_set_pos(track_, 0, bar_y_);
    lv_obj_set_pos(fill_, 0, bar_y_);

    if (show_time_) {
      time_lbl_ = ui::label(c.parent, Font::BODY, theme::INK_SOFT);
      lv_obj_set_width(time_lbl_, width_);
      lv_obj_set_pos(time_lbl_, 0, time_y_);
    }
    if (vol_mode_ != 0) {
      vol_lbl_ = ui::label(c.parent, Font::BODY, theme::MUTED);
      lv_obj_set_style_text_align(vol_lbl_, LV_TEXT_ALIGN_RIGHT, 0);
    }
    lv_obj_set_size(track_, 0, bar_h_);  // width 0 = not shown; on_tick gives it its width
    lv_obj_set_size(fill_, 0, bar_h_);

    if (n_ > 0) {
      const int cell = width_ / n_;
      const int th = ui::line_height(Font::TITLE);
      const int m = ui::px(2);   // space between two buttons
      const int bw = ui::px(1);  // width of the frame
      const int top = height_ - ctrl_h_;
      for (int i = 0; i < n_; ++i) {
        // A light frame: a block in the line colour with a smaller one in the card's colour on it.
        frame_[i] = ui::block(c.parent, theme::LINE);
        lv_obj_set_size(frame_[i], cell - 2 * m, ctrl_h_ - 2 * m);
        lv_obj_set_pos(frame_[i], i * cell + m, top + m);
        inner_[i] = ui::block(c.parent, theme::CARD);
        lv_obj_set_size(inner_[i], cell - 2 * m - 2 * bw, ctrl_h_ - 2 * m - 2 * bw);
        lv_obj_set_pos(inner_[i], i * cell + m + bw, top + m + bw);
      }
      for (int i = 0; i < n_; ++i) {
        btn_[i] = ui::label(c.parent, Font::TITLE, theme::INK);
        lv_obj_set_width(btn_[i], cell);
        lv_obj_set_style_text_align(btn_[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_pos(btn_[i], i * cell, height_ - ctrl_h_ + (ctrl_h_ - th) / 2);
      }
    }
  }

  void on_state(JsonObjectConst data) override {
    has_entity_ = data["state"].is<const char *>();
    state_ = (const char *) (data["state"] | "");
    JsonObjectConst a = data["attributes"];
    title_ = (const char *) (a["media_title"] | "");
    artist_ = (const char *) (a["media_artist"] | "");
    duration_ = a["media_duration"] | 0.0f;
    position_ = a["media_position"] | 0.0f;
    updated_ = a["media_position_updated_at"] | 0u;  // seconds since 1970 (an attribute ending in _at)
    const float v = a["volume_level"] | -1.0f;  // 0 to 1
    const bool m = a["is_volume_muted"] | false;
    if (v >= 0.0f) {
      if (last_volume_ >= 0.0f && (std::fabs(v - last_volume_) > 0.0005f || m != muted_)) vol_flag_ = true;
      last_volume_ = v;
    }
    volume_ = v;
    muted_ = m;
    if (cover_ != nullptr) {
      const char *pic = a["entity_picture"] | "";
      cover_on_ = picture_allowed_(pic);
      lv_obj_set_size(cover_, cover_on_ ? cover_size_ : 0, cover_on_ ? cover_size_ : 0);
      ui::set_picture(cover_, cover_on_ ? pic : "", tessera::Fit::COVER);  // the same address again does nothing
    }
  }

  void on_tick(uint32_t epoch) override {
    const bool playing = state_ == "playing";
    paused_ = state_ == "paused";

    // The text starts after the cover when there is one, and takes the whole width when there is none.
    x0_ = cover_on_ ? cover_size_ + ui::px(8) : 0;
    text_w_ = width_ - x0_;
    lv_obj_set_width(artist_lbl_, text_w_);

    std::string title, artist;
    if (!has_entity_) {
      title = plugin_->text("choose");
    } else if ((playing || paused_) && !title_.empty()) {
      title = title_;
      artist = artist_;
    } else {
      title = plugin_->text("idle");
    }

    // The volume, as a small text at the title's right: always, or for three seconds after it changed.
    std::string vtext;
    if (vol_lbl_ != nullptr && has_entity_ && volume_ >= 0.0f) {
      if (vol_flag_) {
        vol_until_ = epoch + 3;
        vol_flag_ = false;
      }
      if (vol_mode_ == 2 || epoch <= vol_until_)
        vtext = muted_ ? plugin_->text("muted") : std::to_string(static_cast<int>(volume_ * 100.0f + 0.5f)) + " %";
    }
    const int vol_w = vtext.empty() ? 0 : ui::text_width(vtext, Font::BODY) + ui::px(4);
    const int title_w = text_w_ - (vol_w > 0 ? vol_w + ui::px(6) : 0);
    lv_obj_set_width(title_lbl_, title_w);

    // The largest font the title fits in; the label cuts the rest with dots.
    Font f = Font::HEADLINE;
    if (ui::text_width(title, f) > title_w) f = Font::TITLE;
    if (ui::text_width(title, f) > title_w) f = Font::BODY_LARGE;
    ui::set_font(title_lbl_, f);
    ui::set_text(title_lbl_, title);
    ui::set_text(artist_lbl_, artist);
    ui::set_color(title_lbl_, paused_ ? theme::MUTED : theme::INK);

    // Title and artist together, in the middle of the space beside the cover.
    const int th = ui::line_height(f);
    const int ah = ui::line_height(Font::BODY);
    int y = (content_h_ - th - ui::px(2) - ah) / 2;
    if (y < 0) y = 0;
    lv_obj_set_pos(title_lbl_, x0_, y);
    lv_obj_set_pos(artist_lbl_, x0_, y + th + ui::px(2));

    if (vol_lbl_ != nullptr) {
      ui::set_text(vol_lbl_, vtext);
      lv_obj_set_width(vol_lbl_, vol_w);
      lv_obj_set_pos(vol_lbl_, width_ - vol_w, y + (th - ui::line_height(Font::BODY)) / 2);
    }

    // The position: that of the last update plus the time that has passed since while it plays.
    const bool timeline = (playing || paused_) && duration_ > 0.5f;
    float pos = position_;
    if (timeline) {
      if (playing && updated_ > 0 && epoch > updated_) pos += static_cast<float>(epoch - updated_);
      if (pos < 0.0f) pos = 0.0f;
      if (pos > duration_) pos = duration_;
    }
    int track_w = 0, fill_w = 0;
    if (show_bar_ && timeline) {
      track_w = width_;
      fill_w = static_cast<int>(static_cast<float>(width_) * (pos / duration_));
    }
    lv_obj_set_width(track_, track_w);
    lv_obj_set_width(fill_, fill_w);
    if (time_lbl_ != nullptr)
      ui::set_text(time_lbl_, timeline ? fmt_time_(static_cast<int>(pos)) + " / " + fmt_time_(static_cast<int>(duration_))
                                       : std::string());

    // The buttons: plain signs, so no icon outside Tessera's set is needed.
    for (int i = 0; i < n_; ++i) {
      const char *s = "";
      switch (kinds_[i]) {
        case PREV: s = "<<"; break;
        case PLAY_PAUSE: s = playing ? "||" : ">"; break;
        case NEXT: s = ">>"; break;
        case VOL_DOWN: s = "-"; break;
        case VOL_UP: s = "+"; break;
      }
      ui::set_text(btn_[i], s);
    }
  }

  void on_theme() override {
    ui::set_color(title_lbl_, paused_ ? theme::MUTED : theme::INK);
    ui::set_color(artist_lbl_, theme::INK_SOFT);
    if (time_lbl_ != nullptr) ui::set_color(time_lbl_, theme::INK_SOFT);
    if (vol_lbl_ != nullptr) ui::set_color(vol_lbl_, theme::MUTED);
    ui::set_fill(track_, tessera::Tone::NORMAL, theme::TRACK);
    ui::set_fill(fill_, tessera::Tone::ACCENT, theme::ACCENT);
    for (int i = 0; i < n_; ++i) {
      ui::set_color(btn_[i], theme::INK);
      ui::set_fill(frame_[i], tessera::Tone::NORMAL, theme::LINE);
      ui::set_fill(inner_[i], tessera::Tone::NORMAL, theme::CARD);
    }
  }

  void on_tap() override {
    if (entity_.empty()) return;
    auto p = tessera::tap_point();
    const int x = tap_x_(p, 0);
    const int y = tap_y_(p, 0);
    Btn which = PLAY_PAUSE;  // a tap anywhere else, or where the finger is unknown: play or pause
    if (n_ > 0 && x >= 0 && y >= height_ - ctrl_h_) {
      int i = x / (width_ / n_);
      if (i >= n_) i = n_ - 1;
      which = kinds_[i];
    }
    const char *service = service_for(which);
    if (service != nullptr) tessera::action(service, entity_.c_str());
  }

 private:
  const tessera::Plugin *plugin_;
  lv_obj_t *cover_{nullptr};
  lv_obj_t *title_lbl_{nullptr};
  lv_obj_t *artist_lbl_{nullptr};
  lv_obj_t *track_{nullptr};
  lv_obj_t *fill_{nullptr};
  lv_obj_t *time_lbl_{nullptr};
  lv_obj_t *vol_lbl_{nullptr};
  lv_obj_t *btn_[5]{};
  lv_obj_t *frame_[5]{};
  lv_obj_t *inner_[5]{};
  Btn kinds_[5]{};
  int n_{0};
  int width_{0}, height_{0}, text_w_{0}, x0_{0};
  int cover_size_{0};
  bool cover_on_{false};
  int bar_h_{0}, bar_y_{0}, time_y_{0}, ctrl_h_{0}, content_h_{0};
  bool show_bar_{true}, show_time_{false};
  int vol_mode_{1};  // 0 never, 1 for three seconds after a change, 2 always
  float volume_{-1.0f}, last_volume_{-1.0f};
  bool muted_{false}, vol_flag_{false};
  uint32_t vol_until_{0};
  bool has_entity_{false};
  bool paused_{false};
  std::string entity_, state_, title_, artist_;
  float duration_{0.0f}, position_{0.0f};
  uint32_t updated_{0};
};

// ---------------------------------------------------------------------------------------------------------------------
// Tile 2: one button for a media player (previous, play/pause, next, louder, quieter), a tile of its own for a person
// who wants the buttons apart from the info tile.
// ---------------------------------------------------------------------------------------------------------------------
class MediaButtonTile : public tessera::Tile {
 public:
  explicit MediaButtonTile(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override {
    width_ = c.width;
    height_ = c.height;
    entity_ = c.entity;
    action_ = (const char *) (c.options["action"] | "play_pause");

    label_ = ui::label(c.parent, Font::TITLE, theme::INK);
    lv_obj_set_width(label_, width_);
    lv_obj_set_style_text_align(label_, LV_TEXT_ALIGN_CENTER, 0);
  }

  void on_state(JsonObjectConst data) override { has_entity_ = data["state"].is<const char *>(); }

  void on_tick(uint32_t) override {
    const std::string key = "btn_" + action_;
    const std::string text = has_entity_ ? plugin_->text(key.c_str()) : plugin_->text("choose");
    Font f = Font::TITLE;
    if (ui::text_width(text, f) > width_) f = Font::BODY_LARGE;
    if (ui::text_width(text, f) > width_) f = Font::BODY;
    ui::set_font(label_, f);
    ui::set_text(label_, text);
    lv_obj_set_pos(label_, 0, (height_ - ui::line_height(f)) / 2);
  }

  void on_theme() override { ui::set_color(label_, theme::INK); }

  void on_tap() override {
    Btn b = PLAY_PAUSE;
    if (action_ == "previous") b = PREV;
    else if (action_ == "next") b = NEXT;
    else if (action_ == "volume_up") b = VOL_UP;
    else if (action_ == "volume_down") b = VOL_DOWN;
    const char *service = service_for(b);
    if (service != nullptr && !entity_.empty()) tessera::action(service, entity_.c_str());
  }

 private:
  const tessera::Plugin *plugin_;
  lv_obj_t *label_{nullptr};
  int width_{0}, height_{0};
  bool has_entity_{false};
  std::string entity_, action_;
};

void NowPlaying::setup() {
  add_tile("now_playing", [this]() { return new NowPlayingTile(this); });
  add_tile("media_button", [this]() { return new MediaButtonTile(this); });
}

}  // namespace now_playing
}  // namespace esphome
