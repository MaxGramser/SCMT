#include "now_playing.h"

#include <string>

namespace esphome {
namespace now_playing {

// One tile: title (large) and artist (small) of a media_player entity.
class NowPlayingTile : public tessera::Tile {
 public:
  explicit NowPlayingTile(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override {
    width_ = c.width;
    title_lbl_ = tessera::ui::label(c.parent, tessera::Font::HEADLINE, theme::INK);
    artist_lbl_ = tessera::ui::label(c.parent, tessera::Font::BODY, theme::INK_SOFT);
    layout_();
  }

  void on_state(JsonObjectConst data) override {
    state_ = (const char *) (data["state"] | "");
    JsonObjectConst a = data["attributes"];
    title_ = (const char *) (a["media_title"] | "");
    artist_ = (const char *) (a["media_artist"] | "");
    has_entity_ = data["state"].is<const char *>();
  }

  void on_tick(uint32_t) override {
    std::string title, artist;
    bool playing = state_ == "playing";
    bool paused = state_ == "paused";
    if (!has_entity_) {
      title = plugin_->text("choose");
    } else if ((playing || paused) && !title_.empty()) {
      title = title_;
      artist = artist_;
    } else {
      title = plugin_->text("idle");
    }
    // Largest font whose width fits; the label cuts the rest with dots.
    tessera::Font f = tessera::Font::HEADLINE;
    if (tessera::ui::text_width(title, f) > width_) f = tessera::Font::TITLE;
    if (tessera::ui::text_width(title, f) > width_) f = tessera::Font::BODY_LARGE;
    tessera::ui::set_font(title_lbl_, f);
    tessera::ui::set_text(title_lbl_, title);
    tessera::ui::set_text(artist_lbl_, artist);
    tessera::ui::set_color(title_lbl_, paused ? theme::MUTED : theme::INK);
    layout_();
  }

  void on_theme() override {
    tessera::ui::set_color(artist_lbl_, theme::INK_SOFT);
  }

 private:
  void layout_() {
    // Stack both labels, one line high each (needed before API 0.9).
    int h1 = tessera::ui::line_height(tessera::Font::HEADLINE);
    int h2 = tessera::ui::line_height(tessera::Font::BODY);
    lv_obj_set_size(title_lbl_, width_, h1);
    lv_obj_set_size(artist_lbl_, width_, h2);
    lv_obj_set_pos(title_lbl_, 0, 0);
    lv_obj_set_pos(artist_lbl_, 0, h1 + tessera::ui::px(4));
  }

  const tessera::Plugin *plugin_;
  lv_obj_t *title_lbl_{nullptr};
  lv_obj_t *artist_lbl_{nullptr};
  int width_{0};
  bool has_entity_{false};
  std::string state_, title_, artist_;
};

void NowPlaying::setup() {
  add_tile("now_playing", [this]() { return new NowPlayingTile(this); });
}

}  // namespace now_playing
}  // namespace esphome