#include "now_playing.h"

#include <string>

namespace esphome {
namespace now_playing {

using tessera::Font;
namespace ui = tessera::ui;

// ---------------------------------------------------------------------------------------------------------------------
// Tile 1: title, artist and an optional progress bar of a media_player entity. A tap plays or pauses.
// ---------------------------------------------------------------------------------------------------------------------
class NowPlayingTile : public tessera::Tile {
 public:
  explicit NowPlayingTile(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override {
    width_ = c.width;
    height_ = c.height;
    entity_ = c.entity;
    show_progress_ = c.options["progress"] | true;

    title_lbl_ = ui::label(c.parent, Font::HEADLINE, theme::INK);
    artist_lbl_ = ui::label(c.parent, Font::BODY, theme::INK_SOFT);
    track_ = ui::block(c.parent, theme::TRACK);
    fill_ = ui::block(c.parent, theme::ACCENT);

    lv_obj_set_width(title_lbl_, width_);
    lv_obj_set_width(artist_lbl_, width_);
    bar_h_ = ui::px(4);
    lv_obj_set_pos(track_, 0, height_ - bar_h_);
    lv_obj_set_pos(fill_, 0, height_ - bar_h_);
    lv_obj_set_size(track_, 0, bar_h_);  // width 0 = not shown; on_tick gives it its width
    lv_obj_set_size(fill_, 0, bar_h_);
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
  }

  void on_tick(uint32_t epoch) override {
    const bool playing = state_ == "playing";
    paused_ = state_ == "paused";

    std::string title, artist;
    if (!has_entity_) {
      title = plugin_->text("choose");
    } else if ((playing || paused_) && !title_.empty()) {
      title = title_;
      artist = artist_;
    } else {
      title = plugin_->text("idle");
    }

    // The largest font the title fits in; the label cuts the rest with dots.
    Font f = Font::HEADLINE;
    if (ui::text_width(title, f) > width_) f = Font::TITLE;
    if (ui::text_width(title, f) > width_) f = Font::BODY_LARGE;
    ui::set_font(title_lbl_, f);
    ui::set_text(title_lbl_, title);
    ui::set_text(artist_lbl_, artist);
    ui::set_color(title_lbl_, paused_ ? theme::MUTED : theme::INK);
    lv_obj_set_pos(title_lbl_, 0, 0);
    lv_obj_set_pos(artist_lbl_, 0, ui::line_height(f) + ui::px(2));

    // Progress bar: the position of the last update plus the time that has passed since while it plays.
    int track_w = 0, fill_w = 0;
    if (show_progress_ && (playing || paused_) && duration_ > 0.5f) {
      float pos = position_;
      if (playing && updated_ > 0 && epoch > updated_) pos += static_cast<float>(epoch - updated_);
      if (pos < 0.0f) pos = 0.0f;
      if (pos > duration_) pos = duration_;
      track_w = width_;
      fill_w = static_cast<int>(static_cast<float>(width_) * (pos / duration_));
    }
    lv_obj_set_width(track_, track_w);
    lv_obj_set_width(fill_, fill_w);
  }

  void on_theme() override {
    ui::set_color(title_lbl_, paused_ ? theme::MUTED : theme::INK);
    ui::set_color(artist_lbl_, theme::INK_SOFT);
    ui::set_fill(track_, tessera::Tone::NORMAL, theme::TRACK);
    ui::set_fill(fill_, tessera::Tone::ACCENT, theme::ACCENT);
  }

  void on_tap() override {
    if (!entity_.empty()) tessera::action("media_player.media_play_pause", entity_.c_str());
  }

 private:
  const tessera::Plugin *plugin_;
  lv_obj_t *title_lbl_{nullptr};
  lv_obj_t *artist_lbl_{nullptr};
  lv_obj_t *track_{nullptr};
  lv_obj_t *fill_{nullptr};
  int width_{0}, height_{0}, bar_h_{0};
  bool show_progress_{true};
  bool has_entity_{false};
  bool paused_{false};
  std::string entity_, state_, title_, artist_;
  float duration_{0.0f}, position_{0.0f};
  uint32_t updated_{0};
};

// ---------------------------------------------------------------------------------------------------------------------
// Tile 2: one button for a media player (previous, play/pause, next, louder, quieter). The person chooses which in the
// tile's options and puts as many of these tiles on the screen as they like.
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
    const char *service = nullptr;
    if (action_ == "previous") service = "media_player.media_previous_track";
    else if (action_ == "next") service = "media_player.media_next_track";
    else if (action_ == "play_pause") service = "media_player.media_play_pause";
    else if (action_ == "volume_up") service = "media_player.volume_up";
    else if (action_ == "volume_down") service = "media_player.volume_down";
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
