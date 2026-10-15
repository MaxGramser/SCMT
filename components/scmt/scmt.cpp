#include "scmt.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

namespace esphome {
namespace scmt {

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

// Material Design Icons of the speaker. Tessera's icon set may not hold all of them: the code asks ui::covers first
// and shows nothing for one that is missing.
static const char *volume_icon_(float level, bool muted) {
  if (muted) return "\U000F0581";      // volume-off
  if (level < 0.34f) return "\U000F057F";  // volume-low
  if (level < 0.67f) return "\U000F0580";  // volume-medium
  return "\U000F057E";                 // volume-high
}

enum Btn : uint8_t { PREV, PLAY_PAUSE, NEXT, VOL_DOWN, VOL_UP, MUTE };

static const char *service_for(Btn b) {
  switch (b) {
    case PREV: return "media_player.media_previous_track";
    case PLAY_PAUSE: return "media_player.media_play_pause";
    case NEXT: return "media_player.media_next_track";
    case VOL_DOWN: return "media_player.volume_down";
    case VOL_UP: return "media_player.volume_up";
    case MUTE: return "media_player.volume_mute";
  }
  return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------
// Tile 1: cover, title, artist, album, progress, volume and buttons of a media_player entity. Every extra is an option.
// A tap on the tile plays or pauses; a tap on a button, on the progress bar, on the volume bar or on the volume display
// does what it shows.
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
    const bool want_vbar = c.options["volume_bar"] | false;
    const bool want_overlay = c.options["volume_overlay"] | false;
    want_mute_ = c.options["mute"] | false;
    want_seek_ = c.options["seek"] | false;
    show_album_ = c.options["album"] | false;
    cover_url_ = (const char *) (c.options["cover_url"] | "");  // fixed address of a cover file, e.g. /local/cover.jpg

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
      if (want_mute_) kinds_[n_++] = MUTE;
    }
    ctrl_h_ = n_ > 0 ? ui::mm(8) : 0;
    vbar_h_ = (c.rows >= 2 && want_vbar) ? ui::mm(7) : 0;
    if ((ctrl_h_ + vbar_h_) * 2 > height_) vbar_h_ = 0;  // too little room: the volume bar goes first
    if (ctrl_h_ * 2 > height_) {
      n_ = 0;
      ctrl_h_ = 0;
    }

    // From the bottom up: the buttons, the volume bar, the progress bar, the time. What is left above is for the cover
    // and the text.
    const int gap = ui::px(4);
    int bh = c.options["bar_height"] | 4;  // in pixels of the reference look
    if (bh < 1) bh = 1;
    bar_h_ = ui::px(bh);
    const int time_h = show_time_ ? ui::line_height(Font::BODY) : 0;
    int bottom = height_;
    if (ctrl_h_ > 0) bottom = height_ - ctrl_h_ - gap;
    if (vbar_h_ > 0) {
      vbar_y_ = bottom - vbar_h_;
      bottom = vbar_y_ - gap;
    }
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
    lv_obj_set_width(artist_lbl_, text_w_);
    if (show_album_) {
      album_lbl_ = ui::label(c.parent, Font::BODY, theme::MUTED);
      lv_obj_set_width(album_lbl_, text_w_);
    }

    track_ = ui::block(c.parent, theme::TRACK);
    fill_ = ui::block(c.parent, theme::ACCENT);
    lv_obj_set_pos(track_, 0, bar_y_);
    lv_obj_set_pos(fill_, 0, bar_y_);
    lv_obj_set_size(track_, 0, bar_h_);  // width 0 = not shown; on_tick gives it its width
    lv_obj_set_size(fill_, 0, bar_h_);

    if (show_time_) {
      time_lbl_ = ui::label(c.parent, Font::BODY, theme::INK_SOFT);
      lv_obj_set_width(time_lbl_, width_);
      lv_obj_set_pos(time_lbl_, 0, time_y_);
    }
    if (vol_mode_ != 0) {
      vol_lbl_ = ui::label(c.parent, Font::BODY, theme::MUTED);
      vol_icon_lbl_ = ui::label(c.parent, Font::ICON_SMALL, theme::MUTED);
    }

    const int m = ui::px(2);   // space between two buttons, and at the sides
    const int bw = ui::px(1);  // width of a frame

    // The volume bar: a framed track with a fill that shows the volume. A tap on it sets the volume.
    if (vbar_h_ > 0) {
      const int pad = ui::px(2);
      vbar_x0_ = m + bw + pad;
      vbar_w_ = width_ - 2 * (m + bw + pad);
      if (vbar_w_ < 1) vbar_w_ = 1;
      vbar_frame_ = ui::block(c.parent, theme::LINE);
      lv_obj_set_size(vbar_frame_, width_ - 2 * m, vbar_h_);
      lv_obj_set_pos(vbar_frame_, m, vbar_y_);
      vbar_inner_ = ui::block(c.parent, theme::CARD);
      lv_obj_set_size(vbar_inner_, width_ - 2 * m - 2 * bw, vbar_h_ - 2 * bw);
      lv_obj_set_pos(vbar_inner_, m + bw, vbar_y_ + bw);
      vbar_fill_ = ui::block(c.parent, theme::ACCENT);
      lv_obj_set_size(vbar_fill_, 0, vbar_h_ - 2 * (bw + pad));
      lv_obj_set_pos(vbar_fill_, vbar_x0_, vbar_y_ + bw + pad);
    }

    // The buttons, each with a light frame: a block in the line colour with a smaller one in the card's colour on it.
    if (n_ > 0) {
      const int cell = width_ / n_;
      const int top = height_ - ctrl_h_;
      for (int i = 0; i < n_; ++i) {
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
        lv_obj_set_pos(btn_[i], i * cell, top + (ctrl_h_ - ui::line_height(Font::TITLE)) / 2);
      }
    }

    // The big volume display over the whole tile, last, so that it lies on top: a half see-through block, the number
    // and the speaker.
    if (want_overlay) {
      ov_bg_ = ui::block(c.parent, theme::CARD);
      lv_obj_set_pos(ov_bg_, 0, 0);
      lv_obj_set_size(ov_bg_, 0, 0);
      lv_obj_set_style_bg_opa(ov_bg_, LV_OPA_70, 0);
      ov_num_ = ui::label(c.parent, Font::DIGITS, theme::INK);
      ov_icon_ = ui::label(c.parent, Font::ICON, theme::INK_SOFT);
    }
  }

  void on_state(JsonObjectConst data) override {
    has_entity_ = data["state"].is<const char *>();
    state_ = (const char *) (data["state"] | "");
    JsonObjectConst a = data["attributes"];
    title_ = (const char *) (a["media_title"] | "");
    artist_ = (const char *) (a["media_artist"] | "");
    album_ = (const char *) (a["media_album_name"] | "");
    duration_ = a["media_duration"] | 0.0f;
    position_ = a["media_position"] | 0.0f;
    updated_ = a["media_position_updated_at"] | 0u;  // seconds since 1970 (an attribute ending in _at)
    const float v = a["volume_level"] | -1.0f;  // 0 to 1
    const bool mu = a["is_volume_muted"] | false;
    if (v >= 0.0f) {
      if (last_volume_ >= 0.0f && (std::fabs(v - last_volume_) > 0.0005f || mu != muted_)) vol_flag_ = true;
      last_volume_ = v;
    }
    volume_ = v;
    muted_ = mu;
    if (cover_ != nullptr) {
      if (!cover_url_.empty()) {
        // A fixed address (option "Cover address"): the cover is a file that something else keeps up to date. The
        // address gets a new number for every track, because the same address again does nothing.
        const bool active = has_entity_ && !title_.empty() && (state_ == "playing" || state_ == "paused");
        const std::string track = title_ + "|" + artist_ + "|" + album_;
        if (track != cover_track_) {
          cover_track_ = track;
          uint32_t h = 2166136261u;  // FNV-1a
          for (char ch : track) {
            h ^= static_cast<uint8_t>(ch);
            h *= 16777619u;
          }
          cover_key_ = h;
          cover_gen_ = 0;
          cover_retry_ = true;
          cover_retry_at_ = 0;
        }
        apply_cover_url_(active);
      } else {
        const char *pic = a["entity_picture"] | "";
        cover_on_ = picture_allowed_(pic);
        lv_obj_set_size(cover_, cover_on_ ? cover_size_ : 0, cover_on_ ? cover_size_ : 0);
        ui::set_picture(cover_, cover_on_ ? pic : "", tessera::Fit::COVER);  // the same address again does nothing
      }
    }
  }

  void on_tick(uint32_t epoch) override {
    const bool playing = state_ == "playing";
    paused_ = state_ == "paused";
    const bool active = playing || paused_;

    // The cover file may be written a moment after the title changes: three seconds later the address changes once
    // more, so the screen asks again.
    if (cover_retry_ && cover_ != nullptr && !cover_url_.empty() && cover_on_) {
      if (cover_retry_at_ == 0) {
        cover_retry_at_ = epoch + 3;
      } else if (epoch >= cover_retry_at_) {
        cover_gen_++;
        cover_retry_ = false;
        apply_cover_url_(true);
      }
    }

    // The text starts after the cover when there is one, and takes the whole width when there is none.
    x0_ = cover_on_ ? cover_size_ + ui::px(8) : 0;
    text_w_ = width_ - x0_;
    lv_obj_set_width(artist_lbl_, text_w_);
    if (album_lbl_ != nullptr) lv_obj_set_width(album_lbl_, text_w_);

    std::string title, artist, album;
    if (!has_entity_) {
      title = plugin_->text("choose");
    } else if (active && !title_.empty()) {
      title = title_;
      artist = artist_;
      album = album_;
    } else {
      title = plugin_->text("idle");
    }

    // The volume: a change starts three seconds in which the big display shows, or the small one when there is no
    // big one.
    if (vol_flag_) {
      vol_until_ = epoch + 3;
      vol_flag_ = false;
    }
    const bool vol_known = has_entity_ && volume_ >= 0.0f;
    const bool recent = epoch <= vol_until_;
    ov_on_ = ov_bg_ != nullptr && vol_known && recent;
    const int pct = static_cast<int>(volume_ * 100.0f + 0.5f);
    std::string vtext;
    if (vol_lbl_ != nullptr && vol_known && !ov_on_ && (vol_mode_ == 2 || recent))
      vtext = muted_ ? plugin_->text("muted") : std::to_string(pct) + " %";
    const char *vic = volume_icon_(volume_, muted_);
    int vol_tw = 0, vol_iw = 0, vol_total = 0;
    if (!vtext.empty()) {
      vol_tw = ui::text_width(vtext, Font::BODY) + ui::px(4);
      if (vol_icon_lbl_ != nullptr && ui::covers(vic, Font::ICON_SMALL))
        vol_iw = ui::text_width(vic, Font::ICON_SMALL) + ui::px(2);
      vol_total = vol_tw + (vol_iw > 0 ? ui::px(4) + vol_iw : 0);
    }
    const int title_w = text_w_ - (vol_total > 0 ? vol_total + ui::px(6) : 0);
    lv_obj_set_width(title_lbl_, title_w);

    // The largest font the title fits in; the label cuts the rest with dots.
    Font f = Font::HEADLINE;
    if (ui::text_width(title, f) > title_w) f = Font::TITLE;
    if (ui::text_width(title, f) > title_w) f = Font::BODY_LARGE;
    ui::set_font(title_lbl_, f);
    ui::set_text(title_lbl_, title);
    ui::set_text(artist_lbl_, artist);
    ui::set_color(title_lbl_, paused_ ? theme::MUTED : theme::INK);

    // Title, artist and, when it fits, the album, together in the middle of the space beside the cover.
    const int th = ui::line_height(f);
    const int ah = ui::line_height(Font::BODY);
    const int g2 = ui::px(2);
    int block_h = th + g2 + ah;
    const bool with_album = album_lbl_ != nullptr && !album.empty() && block_h + g2 + ah <= content_h_;
    if (with_album) block_h += g2 + ah;
    int y = (content_h_ - block_h) / 2;
    if (y < 0) y = 0;
    lv_obj_set_pos(title_lbl_, x0_, y);
    lv_obj_set_pos(artist_lbl_, x0_, y + th + g2);
    if (album_lbl_ != nullptr) {
      ui::set_text(album_lbl_, with_album ? album : std::string());
      lv_obj_set_pos(album_lbl_, x0_, y + th + g2 + ah + g2);
    }

    // The small volume display: a text and the speaker, at the title's right. A tap on it mutes (option).
    if (vol_lbl_ != nullptr) {
      ui::set_text(vol_lbl_, vtext);
      lv_obj_set_width(vol_lbl_, vol_tw);
      lv_obj_set_pos(vol_lbl_, width_ - vol_total, y + (th - ui::line_height(Font::BODY)) / 2);
      ui::set_text(vol_icon_lbl_, vol_iw > 0 ? vic : "");
      lv_obj_set_width(vol_icon_lbl_, vol_iw);
      lv_obj_set_pos(vol_icon_lbl_, width_ - vol_iw, y + (th - ui::line_height(Font::ICON_SMALL)) / 2);
    }
    vol_hit_x_ = width_ - vol_total;
    vol_hit_w_ = vol_total;
    vol_hit_y_ = y;
    vol_hit_h_ = th;

    // The position: that of the last update plus the time that has passed since while it plays.
    const bool timeline = active && duration_ > 0.5f;
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

    // The volume bar's fill.
    if (vbar_fill_ != nullptr) {
      float lvl = volume_ < 0.0f ? 0.0f : volume_;
      if (lvl > 1.0f) lvl = 1.0f;
      lv_obj_set_width(vbar_fill_, static_cast<int>(static_cast<float>(vbar_w_) * lvl));
    }

    // The buttons: plain signs, and the speaker for muting when the icon set has it.
    const int cell = n_ > 0 ? width_ / n_ : 0;
    for (int i = 0; i < n_; ++i) {
      const char *s = "";
      Font bf = Font::TITLE;
      switch (kinds_[i]) {
        case PREV: s = "<<"; break;
        case PLAY_PAUSE: s = playing ? "||" : ">"; break;
        case NEXT: s = ">>"; break;
        case VOL_DOWN: s = "-"; break;
        case VOL_UP: s = "+"; break;
        case MUTE: {
          const char *ic = muted_ ? "\U000F0581" : "\U000F057E";
          if (ui::covers(ic, Font::ICON)) {
            s = ic;
            bf = Font::ICON;
          } else {
            s = "M";
          }
          break;
        }
      }
      ui::set_font(btn_[i], bf);
      ui::set_text(btn_[i], s);
      lv_obj_set_pos(btn_[i], i * cell, height_ - ctrl_h_ + (ctrl_h_ - ui::line_height(bf)) / 2);
    }

    // The big volume display: the number in the largest font that fits, and the speaker after it.
    if (ov_bg_ != nullptr) {
      if (ov_on_) {
        const std::string num = muted_ ? plugin_->text("muted") : std::to_string(pct) + "%";
        const char *ic = volume_icon_(volume_, muted_);
        const bool icon_ok = ui::covers(ic, Font::ICON);
        const int iw = icon_ok ? ui::text_width(ic, Font::ICON) : 0;
        const int gap = ui::px(8);
        Font nf = Font::DIGITS;
        if (muted_ || !ui::covers(num, nf)) nf = Font::VALUE;
        if (ui::text_width(num, nf) + (iw > 0 ? gap + iw : 0) > width_) nf = Font::HEADLINE;
        if (ui::text_width(num, nf) + (iw > 0 ? gap + iw : 0) > width_) nf = Font::TITLE;
        const int nw = ui::text_width(num, nf);
        const int total = nw + (iw > 0 ? gap + iw : 0);
        int x = (width_ - total) / 2;
        if (x < 0) x = 0;
        lv_obj_set_size(ov_bg_, width_, height_);
        ui::set_font(ov_num_, nf);
        ui::set_text(ov_num_, num);
        lv_obj_set_width(ov_num_, nw + ui::px(4));
        lv_obj_set_pos(ov_num_, x, (height_ - ui::line_height(nf)) / 2);
        ui::set_text(ov_icon_, icon_ok ? ic : "");
        lv_obj_set_width(ov_icon_, iw + ui::px(2));
        lv_obj_set_pos(ov_icon_, x + nw + gap, (height_ - ui::line_height(Font::ICON)) / 2);
      } else {
        lv_obj_set_size(ov_bg_, 0, 0);
        ui::set_text(ov_num_, "");
        ui::set_text(ov_icon_, "");
      }
    }
  }

  void on_theme() override {
    ui::set_color(title_lbl_, paused_ ? theme::MUTED : theme::INK);
    ui::set_color(artist_lbl_, theme::INK_SOFT);
    if (album_lbl_ != nullptr) ui::set_color(album_lbl_, theme::MUTED);
    if (time_lbl_ != nullptr) ui::set_color(time_lbl_, theme::INK_SOFT);
    if (vol_lbl_ != nullptr) ui::set_color(vol_lbl_, theme::MUTED);
    if (vol_icon_lbl_ != nullptr) ui::set_color(vol_icon_lbl_, theme::MUTED);
    ui::set_fill(track_, tessera::Tone::NORMAL, theme::TRACK);
    ui::set_fill(fill_, tessera::Tone::ACCENT, theme::ACCENT);
    if (vbar_frame_ != nullptr) {
      ui::set_fill(vbar_frame_, tessera::Tone::NORMAL, theme::LINE);
      ui::set_fill(vbar_inner_, tessera::Tone::NORMAL, theme::CARD);
      ui::set_fill(vbar_fill_, tessera::Tone::ACCENT, theme::ACCENT);
    }
    for (int i = 0; i < n_; ++i) {
      ui::set_color(btn_[i], theme::INK);
      ui::set_fill(frame_[i], tessera::Tone::NORMAL, theme::LINE);
      ui::set_fill(inner_[i], tessera::Tone::NORMAL, theme::CARD);
    }
    if (ov_bg_ != nullptr) {
      ui::set_fill(ov_bg_, tessera::Tone::NORMAL, theme::CARD);
      lv_obj_set_style_bg_opa(ov_bg_, LV_OPA_70, 0);  // after the fill, which may set its own
      ui::set_color(ov_num_, theme::INK);
      ui::set_color(ov_icon_, theme::INK_SOFT);
    }
  }

  void on_tap() override {
    if (entity_.empty()) return;
    auto p = tessera::tap_point();
    const int x = tap_x_(p, 0);
    const int y = tap_y_(p, 0);
    const bool known = x >= 0 && y >= 0;  // -1 when the touch point is not known: then it is a plain tap

    // 1. The big volume display: a tap mutes (option).
    if (known && ov_on_ && want_mute_) {
      mute_();
      return;
    }
    // 2. A button.
    if (known && n_ > 0 && y >= height_ - ctrl_h_) {
      int i = x / (width_ / n_);
      if (i >= n_) i = n_ - 1;
      if (kinds_[i] == MUTE) {
        mute_();
      } else {
        const char *service = service_for(kinds_[i]);
        if (service != nullptr) tessera::action(service, entity_.c_str());
      }
      return;
    }
    // 3. The volume bar: a tap sets the volume where it hit.
    if (known && vbar_h_ > 0 && y >= vbar_y_ - ui::px(4) && y < vbar_y_ + vbar_h_ + ui::px(4)) {
      float lvl = static_cast<float>(x - vbar_x0_) / static_cast<float>(vbar_w_);
      if (lvl < 0.0f) lvl = 0.0f;
      if (lvl > 1.0f) lvl = 1.0f;
      char b[16];
      snprintf(b, sizeof b, "%.2f", lvl);
      tessera::action("media_player.volume_set", entity_.c_str(), "volume_level", b);
      return;
    }
    // 4. The small volume display: a tap mutes (option).
    if (known && want_mute_ && vol_hit_w_ > 0 && x >= vol_hit_x_ - ui::mm(2) && y >= vol_hit_y_ - ui::mm(1) &&
        y <= vol_hit_y_ + vol_hit_h_ + ui::mm(2)) {
      mute_();
      return;
    }
    // 5. The progress bar: a tap seeks to where it hit (option).
    if (known && want_seek_ && show_bar_ && duration_ > 0.5f && y >= bar_y_ - ui::mm(3) &&
        y <= bar_y_ + bar_h_ + ui::mm(2)) {
      float frac = static_cast<float>(x) / static_cast<float>(width_);
      if (frac < 0.0f) frac = 0.0f;
      if (frac > 1.0f) frac = 1.0f;
      char b[16];
      snprintf(b, sizeof b, "%d", static_cast<int>(frac * duration_));
      tessera::action("media_player.media_seek", entity_.c_str(), "seek_position", b);
      return;
    }
    // 6. Anywhere else: play or pause.
    tessera::action("media_player.media_play_pause", entity_.c_str());
  }

 private:
  void mute_() {
    tessera::action("media_player.volume_mute", entity_.c_str(), "is_volume_muted", muted_ ? "false" : "true");
  }

  // Shows the cover file of the option "Cover address", with a number behind it that changes with the track.
  void apply_cover_url_(bool active) {
    std::string addr = cover_url_ + "?v=" + std::to_string(cover_key_ + cover_gen_);
    cover_on_ = active && picture_allowed_(addr.c_str());
    lv_obj_set_size(cover_, cover_on_ ? cover_size_ : 0, cover_on_ ? cover_size_ : 0);
    ui::set_picture(cover_, cover_on_ ? addr.c_str() : "", tessera::Fit::COVER);
  }

  const tessera::Plugin *plugin_;
  lv_obj_t *cover_{nullptr};
  lv_obj_t *title_lbl_{nullptr};
  lv_obj_t *artist_lbl_{nullptr};
  lv_obj_t *album_lbl_{nullptr};
  lv_obj_t *track_{nullptr};
  lv_obj_t *fill_{nullptr};
  lv_obj_t *time_lbl_{nullptr};
  lv_obj_t *vol_lbl_{nullptr};
  lv_obj_t *vol_icon_lbl_{nullptr};
  lv_obj_t *vbar_frame_{nullptr};
  lv_obj_t *vbar_inner_{nullptr};
  lv_obj_t *vbar_fill_{nullptr};
  lv_obj_t *btn_[6]{};
  lv_obj_t *frame_[6]{};
  lv_obj_t *inner_[6]{};
  lv_obj_t *ov_bg_{nullptr};
  lv_obj_t *ov_num_{nullptr};
  lv_obj_t *ov_icon_{nullptr};
  Btn kinds_[6]{};
  int n_{0};
  int width_{0}, height_{0}, text_w_{0}, x0_{0};
  int cover_size_{0};
  bool cover_on_{false};
  std::string cover_url_, cover_track_;
  uint32_t cover_key_{0}, cover_gen_{0}, cover_retry_at_{0};
  bool cover_retry_{false};
  int bar_h_{0}, bar_y_{0}, time_y_{0}, ctrl_h_{0}, content_h_{0};
  int vbar_h_{0}, vbar_y_{0}, vbar_x0_{0}, vbar_w_{1};
  int vol_hit_x_{0}, vol_hit_w_{0}, vol_hit_y_{0}, vol_hit_h_{0};
  bool show_bar_{true}, show_time_{false}, show_album_{false};
  bool want_mute_{false}, want_seek_{false};
  int vol_mode_{1};  // 0 never, 1 for three seconds after a change, 2 always
  float volume_{-1.0f}, last_volume_{-1.0f};
  bool muted_{false}, vol_flag_{false}, ov_on_{false};
  uint32_t vol_until_{0};
  bool has_entity_{false};
  bool paused_{false};
  std::string entity_, state_, title_, artist_, album_;
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

void Scmt::setup() {
  add_tile("now_playing", [this]() { return new NowPlayingTile(this); });
  add_tile("media_button", [this]() { return new MediaButtonTile(this); });
}

}  // namespace scmt
}  // namespace esphome
