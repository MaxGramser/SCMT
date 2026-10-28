#include "scmt.h"
#include "esphome/core/hal.h"

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

// Material Design Icons from Tessera's set (another codepoint shows as nothing).
constexpr uint32_t ICON_PREV = 0xF04AE, ICON_NEXT = 0xF04AD, ICON_PLAY = 0xF040A, ICON_PAUSE = 0xF03E4,
                   ICON_VOLUME_DOWN = 0xF075E, ICON_VOLUME_UP = 0xF075D, ICON_VOLUME = 0xF057E, ICON_MUTED = 0xF0581;

// The speaker: volume-high, or volume-off while muted (the set has no volume-low or volume-medium).
static std::string volume_icon_(bool muted) { return ui::icon(muted ? ICON_MUTED : ICON_VOLUME); }

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
  explicit NowPlayingTile(const Scmt *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override {
    width_ = c.width;
    height_ = c.height;
    entity_ = c.entity;
    const std::string timeline = (const char *) (c.options["timeline"] | "bar");
    show_bar_ = timeline == "bar" || timeline == "both";
    show_time_ = timeline == "time" || timeline == "both";
    const std::string vd = (const char *) (c.options["volume_display"] | "change");
    vol_mode_ = vd == "always" ? 2 : (vd == "off" ? 0 : 1);
    // "cover", "buttons" and "overlay" are choices that each hold two former options (at most 12 options per tile).
    // Tiles placed before 0.9.0 still carry the former options; they are read when the choice is missing.
    bool want_cover;
    if (c.options["cover"].is<bool>()) {
      want_cover = c.options["cover"].as<bool>();
      cover_right_ = std::string((const char *) (c.options["cover_side"] | "left")) == "right";
    } else {
      const std::string cover = (const char *) (c.options["cover"] | "left");
      want_cover = cover != "off";
      cover_right_ = cover == "right";
    }
    const bool old_controls = c.options["controls"] | false;
    const bool old_volume = c.options["volume"] | false;
    const char *old_buttons = old_controls ? (old_volume ? "both" : "playback") : (old_volume ? "volume" : "none");
    const std::string buttons = (const char *) (c.options["buttons"] | old_buttons);
    const bool want_controls = buttons == "playback" || buttons == "both";
    const bool want_volume = buttons == "volume" || buttons == "both";
    const bool old_vol_overlay = c.options["volume_overlay"] | false;
    const bool old_pause_overlay = c.options["pause_overlay"] | false;
    const char *old_overlay =
        old_vol_overlay ? (old_pause_overlay ? "both" : "volume") : (old_pause_overlay ? "pause" : "none");
    const std::string overlay = (const char *) (c.options["overlay"] | old_overlay);
    const bool want_overlay = overlay == "volume" || overlay == "both";
    const bool want_pause_overlay = overlay == "pause" || overlay == "both";
    const bool want_vbar = c.options["volume_bar"] | false;
    // Muting comes with the volume buttons (a key between quieter and louder, and a tap on a volume display); a tap on
    // the progress bar always seeks. Both were options before 0.11.0.
    want_mute_ = want_volume;
    // The text size: small (the look before 0.10.3), medium, large.
    const std::string ts = (const char *) (c.options["text_size"] | "medium");
    text_size_ = ts == "small" ? 0 : (ts == "large" ? 2 : 1);
    // "album" is a choice of the extra lines under the artist since 0.13.0: the album, the tempo of the screen's BPM
    // sensor, or both. Before, "album" and "bpm" were two toggles; tiles that still carry them are read as they are.
    if (c.options["album"].is<const char *>()) {
      const std::string extra = (const char *) c.options["album"];
      show_album_ = extra == "album" || extra == "both";
      show_bpm_ = extra == "bpm" || extra == "both";
    } else {
      show_album_ = c.options["album"] | false;
      show_bpm_ = c.options["bpm"] | false;
    }
    // The colour of the progress bar and the volume bar: the look's accent or a colour of the core, both readable in
    // light and dark (a plugin may not use a colour as a number).
    const std::string bc = (const char *) (c.options["bar_color"] | "accent");
    bar_tone_ = tessera::Tone::NORMAL;
    if (bc == "green") bar_tone_ = tessera::Tone::GOOD;
    else if (bc == "amber") bar_tone_ = tessera::Tone::BUSY;
    else if (bc == "red") bar_tone_ = tessera::Tone::ALERT;
    else if (bc == "blue") bar_role_ = theme::MARK_BLUE;
    else if (bc == "purple") bar_role_ = theme::MARK_PURPLE;
    else if (bc == "ink") bar_role_ = theme::INK;
    else bar_tone_ = tessera::Tone::ACCENT;
    cover_url_ = (const char *) (c.options["cover_url"] | "");  // fixed address of a cover, e.g. /api/image_proxy/image.scmt_cover?v={v}
    const std::string tpos = (const char *) (c.options["text_pos"] | "middle");
    text_pos_ = tpos == "top" ? 1 : (tpos == "bottom" ? 2 : 0);

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
        kinds_[n_++] = MUTE;
        kinds_[n_++] = VOL_UP;
      }
    }
    ctrl_h_ = n_ > 0 ? ui::mm(8) : 0;
    if (ctrl_h_ * 2 > height_) {
      n_ = 0;
      ctrl_h_ = 0;
    }

    // One space between all parts (cover, text, bars, buttons): the card's own padding, so that the parts stand as far
    // from each other as from the card's edge.
    gap_ = ui::px(12);
    const int gap = gap_;
    const int bw = ui::px(1);  // width of a frame

    // The volume bar: upright along the whole height, at the right edge. Everything else lies in the area beside it. It
    // is two thirds of a finger wide; its tap reaches into the space beside it. It needs two columns.
    area_x_ = 0;
    area_w_ = width_;
    if (want_vbar && c.columns >= 2) {
      vbar_w_ = ui::mm(7) * 2 / 3;
      vbar_x_ = width_ - vbar_w_;
      area_w_ = width_ - vbar_w_ - gap;
    }

    // From the bottom up: the buttons, the progress bar, the time. What is left above is for the cover and the text.
    int bh = c.options["bar_height"] | 4;  // in pixels of the reference look
    if (bh < 1) bh = 1;
    bar_h_ = ui::px(bh);
    const int time_h = show_time_ ? ui::line_height(Font::BODY) : 0;
    int bottom = height_;
    if (ctrl_h_ > 0) bottom = height_ - ctrl_h_ - gap;
    bar_y_ = show_bar_ ? bottom - bar_h_ : bottom;
    time_y_ = (show_bar_ ? bar_y_ - ui::px(4) : bottom) - time_h;  // the time sits close over its bar
    int above = bottom;
    if (show_time_) above = time_y_;
    else if (show_bar_) above = bar_y_;
    content_h_ = (show_bar_ || show_time_) ? above - gap : bottom;
    if (content_h_ < 1) content_h_ = 1;

    // The cover, a square at the left or the right (option), when the tile is two cells wide or more and the screen
    // shows pictures.
    x0_ = area_x_;
    if (want_cover && ui::pictures() && c.columns >= 2 && content_h_ > ui::px(24)) {
      cover_size_ = content_h_;
      if (cover_size_ > area_w_ / 2) cover_size_ = area_w_ / 2;
      // Its size is set here, once, as the app cuts the picture to it; it is hidden, not shrunk, while the player has
      // no picture the screen may fetch.
      cover_ = ui::picture(c.parent);
      lv_obj_set_size(cover_, cover_size_, cover_size_);
      lv_obj_set_pos(cover_, cover_right_ ? area_x_ + area_w_ - cover_size_ : area_x_, 0);
      lv_obj_add_flag(cover_, LV_OBJ_FLAG_HIDDEN);
    }
    text_w_ = area_w_;
    text_right_ = area_x_ + area_w_;

    // Title, artist and album end in dots when they are wider than their room, as every label of the screen.
    title_lbl_ = ui::label(c.parent, Font::HEADLINE, theme::INK);
    artist_lbl_ = ui::label(c.parent, Font::BODY_LARGE, theme::INK_SOFT);
    lv_obj_set_width(artist_lbl_, text_w_);
    if (show_album_) {
      album_lbl_ = ui::label(c.parent, Font::BODY_LARGE, theme::MUTED);
      lv_obj_set_width(album_lbl_, text_w_);
    }
    if (show_bpm_) {
      bpm_lbl_ = ui::label(c.parent, Font::BODY_LARGE, theme::MUTED);
      lv_obj_set_width(bpm_lbl_, text_w_);
    }

    track_ = ui::block(c.parent, theme::TRACK);
    fill_ = ui::block(c.parent, theme::ACCENT);
    lv_obj_set_pos(track_, area_x_, bar_y_);
    lv_obj_set_pos(fill_, area_x_, bar_y_);
    lv_obj_set_size(track_, 0, bar_h_);  // width 0 = not shown; on_tick gives it its width
    lv_obj_set_size(fill_, 0, bar_h_);
    ui::set_fill(fill_, bar_tone_, bar_role_);

    if (show_time_) {
      time_lbl_ = ui::label(c.parent, Font::BODY, theme::INK_SOFT);
      lv_obj_set_width(time_lbl_, area_w_);
      lv_obj_set_pos(time_lbl_, area_x_, time_y_);
    }
    if (vol_mode_ != 0) {
      vol_lbl_ = ui::label(c.parent, Font::BODY, theme::MUTED);
      vol_icon_lbl_ = ui::label(c.parent, Font::ICON_SMALL, theme::MUTED);
    }

    // The volume bar: a framed track with a fill that grows from the bottom. A tap on it sets the volume.
    if (vbar_w_ > 0) {
      const int pad = ui::px(2);
      vbar_fy_ = bw + pad;
      vbar_fh_ = height_ - 2 * (bw + pad);
      if (vbar_fh_ < 1) vbar_fh_ = 1;
      vbar_frame_ = ui::block(c.parent, theme::LINE);
      lv_obj_set_size(vbar_frame_, vbar_w_, height_);
      lv_obj_set_pos(vbar_frame_, vbar_x_, 0);
      vbar_inner_ = ui::block(c.parent, theme::CARD);
      lv_obj_set_size(vbar_inner_, vbar_w_ - 2 * bw, height_ - 2 * bw);
      lv_obj_set_pos(vbar_inner_, vbar_x_ + bw, bw);
      vbar_fill_ = ui::block(c.parent, theme::ACCENT);
      lv_obj_set_size(vbar_fill_, vbar_w_ - 2 * (bw + pad), 0);
      lv_obj_set_pos(vbar_fill_, vbar_x_ + bw + pad, vbar_fy_ + vbar_fh_);
      ui::set_fill(vbar_fill_, bar_tone_, bar_role_);
    }

    // The buttons, each with a light frame: a block in the line colour with a smaller one in the card's colour on it.
    // The outer ones stand flush with the edges of the area, like the progress bar.
    if (n_ > 0) {
      btn_gap_ = ui::px(4);
      btn_w_ = (area_w_ - (n_ - 1) * btn_gap_) / n_;
      const int top = height_ - ctrl_h_;
      for (int i = 0; i < n_; ++i) {
        const int x = area_x_ + i * (btn_w_ + btn_gap_);
        frame_[i] = ui::block(c.parent, theme::LINE);
        lv_obj_set_size(frame_[i], btn_w_, ctrl_h_);
        lv_obj_set_pos(frame_[i], x, top);
        inner_[i] = ui::block(frame_[i], theme::CARD);
        lv_obj_set_size(inner_[i], btn_w_ - 2 * bw, ctrl_h_ - 2 * bw);
        lv_obj_set_pos(inner_[i], bw, bw);
      }
      // The icon lies inside the inner block, centred.
      const int inner_w = btn_w_ - 2 * bw, inner_h = ctrl_h_ - 2 * bw;
      for (int i = 0; i < n_; ++i) {
        btn_[i] = ui::label(inner_[i], Font::ICON, theme::INK);
        lv_obj_set_width(btn_[i], inner_w);
        lv_obj_set_style_text_align(btn_[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_pos(btn_[i], 0, (inner_h - ui::line_height(Font::ICON)) / 2);
      }
    }

    // The pause display over the whole tile: a half see-through block and the pause sign, while the player is paused.
    // Before the volume display, so that the volume display lies on top of it. The block has square corners, so that
    // it covers the ends of the progress bar in the corners too. The sign is two bars, as the icon font is too small.
    if (want_pause_overlay) {
      pz_bg_ = ui::block(c.parent, theme::CARD);
      lv_obj_set_pos(pz_bg_, 0, 0);
      lv_obj_set_size(pz_bg_, 0, 0);
      lv_obj_set_style_radius(pz_bg_, 0, 0);
      lv_obj_set_style_bg_opa(pz_bg_, LV_OPA_70, 0);
      for (auto &bar : pz_bar_) {
        bar = ui::block(c.parent, theme::INK);
        lv_obj_set_size(bar, 0, 0);
      }
    }

    // The big volume display over the whole tile, last, so that it lies on top: a half see-through block, the number
    // and the speaker.
    if (want_overlay) {
      ov_bg_ = ui::block(c.parent, theme::CARD);
      lv_obj_set_pos(ov_bg_, 0, 0);
      lv_obj_set_size(ov_bg_, 0, 0);
      lv_obj_set_style_radius(ov_bg_, 0, 0);
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
    // The tempo sensor follows a new track only after some seconds (BPM2SCMT measures anew, HASS.Agent reads its file
    // every 10 seconds): until then it still holds the tempo of the track before, so a new track hides it for a while.
    if (bpm_lbl_ != nullptr && has_entity_ && !title_.empty()) {
      const tessera::Text track = title_ + "|" + artist_;
      if (bpm_seen_ && track != bpm_track_) bpm_hold_ = true;
      bpm_track_ = track;
      bpm_seen_ = true;
    }
    if (cover_ != nullptr) {
      if (!cover_url_.empty()) {
        // A fixed address (option "Cover address"): the cover is a file that something else keeps up to date. The
        // address gets a new number for every track, because the same address again does nothing.
        const bool active = has_entity_ && !title_.empty() && (state_ == "playing" || state_ == "paused");
        const tessera::Text track = title_ + "|" + artist_ + "|" + album_;
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
        show_cover_(cover_on_ ? pic : "");
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

    // The text lies beside the cover (at its right when the cover is at the left, at its left when the cover is at
    // the right) and takes the whole width when there is no cover.
    const int cover_w = cover_on_ ? cover_size_ + gap_ : 0;
    x0_ = area_x_ + (cover_right_ ? 0 : cover_w);
    text_w_ = area_w_ - cover_w;
    text_right_ = x0_ + text_w_;  // where the text area ends: the volume display sits at its right edge
    lv_obj_set_width(artist_lbl_, text_w_);
    if (album_lbl_ != nullptr) lv_obj_set_width(album_lbl_, text_w_);

    // The tempo, as a line under the artist (option). After a new track it waits until the sensor was empty once and
    // has a value again, or at most 12 seconds.
    std::string bpm_text;
    if (bpm_lbl_ != nullptr) {
      const int bpm = plugin_->bpm();
      if (bpm_hold_) {
        bpm_hold_until_ = epoch + 12;
        bpm_cleared_ = false;
        bpm_hold_ = false;
      }
      if (bpm == 0) bpm_cleared_ = true;
      if (bpm_cleared_) bpm_hold_until_ = 0;
      if (has_entity_ && active && !title_.empty() && bpm > 0 && epoch >= bpm_hold_until_)
        bpm_text = tessera::format(plugin_->text("bpm"), bpm);
    }

    std::string title, artist, album;
    if (!has_entity_) {
      title = plugin_->text("choose");
    } else if (active && !title_.empty()) {
      title = roomy::plain(title_);
      artist = roomy::plain(artist_);
      album = roomy::plain(album_);
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
    const std::string vic = volume_icon_(muted_);
    int vol_tw = 0, vol_iw = 0, vol_total = 0;
    if (!vtext.empty()) {
      vol_tw = ui::text_width(vtext, Font::BODY) + ui::px(4);
      if (vol_icon_lbl_ != nullptr) vol_iw = ui::text_width(vic, Font::ICON_SMALL) + ui::px(2);
      vol_total = vol_tw + (vol_iw > 0 ? ui::px(4) + vol_iw : 0);
    }
    const int title_w = text_w_ - (vol_total > 0 ? vol_total + ui::px(6) : 0);
    lv_obj_set_width(title_lbl_, title_w);

    // The fonts of the text size (option). VALUE is the largest font with letters (DIGITS has digits only), so "large"
    // keeps the title of "medium" and makes artist and album larger, and cuts a long title in VALUE.
    struct Faces {
      Font title, wide, low;    // the title: when it fits; when it is wider (cut with dots); on a tile too low for it
      Font sub, sub_low;        // artist and album: when there is room; on a low tile
    };
    static const Faces FACES[3] = {
        {Font::HEADLINE, Font::HEADLINE, Font::BODY_LARGE, Font::BODY, Font::BODY},              // small
        {Font::VALUE, Font::HEADLINE, Font::BODY_LARGE, Font::BODY_LARGE, Font::BODY},           // medium
        {Font::VALUE, Font::VALUE, Font::HEADLINE, Font::HEADLINE, Font::BODY_LARGE},            // large
    };
    const Faces &fc = FACES[text_size_];

    // Artist and album in their font, or one step smaller when the tile is too low for it next to the smallest title.
    const int space_h = cover_on_ ? cover_size_ : content_h_;
    const int g2 = ui::px(2);
    const Font sf = ui::line_height(fc.sub) * 2 + g2 <= space_h ? fc.sub : fc.sub_low;
    const int ah = ui::line_height(sf);
    ui::set_font(artist_lbl_, sf);
    if (album_lbl_ != nullptr) ui::set_font(album_lbl_, sf);
    if (bpm_lbl_ != nullptr) ui::set_font(bpm_lbl_, sf);

    // The title in its font when it fits in width and, with the artist below it, in height, and the font has all its
    // characters. Else in the font for a wide title, cut with dots; in a smaller one only when the tile is too low.
    auto high_ok = [&](Font ff) { return ui::line_height(ff) + g2 + ah <= space_h; };
    Font f = fc.title;
    if (!(ui::covers(title, f) && ui::text_width(title, f) <= title_w && high_ok(f))) f = fc.wide;
    if (!ui::covers(title, f)) f = Font::HEADLINE;  // a character VALUE lacks (a Japanese title)
    if (!high_ok(f)) f = fc.low;
    if (!high_ok(f)) f = Font::BODY_LARGE;
    ui::set_font(title_lbl_, f);
    ui::set_text(title_lbl_, title);
    ui::set_color(title_lbl_, paused_ ? theme::MUTED : theme::INK);

    // Title, artist and, when they fit, the tempo and the album, together at the top, in the middle or at the bottom of
    // the space beside the cover (option). With a cover that space is the cover's height, so "top" and "bottom" line up
    // with its upper and lower edge. When even the smallest title leaves no room for the artist, the title stands alone.
    const int th = ui::line_height(f);
    const bool with_artist = th + g2 + ah <= space_h;
    ui::set_text(artist_lbl_, with_artist ? artist : std::string());
    int block_h = with_artist ? th + g2 + ah : th;
    const bool with_bpm = with_artist && !bpm_text.empty() && block_h + g2 + ah <= space_h;
    if (with_bpm) block_h += g2 + ah;
    const bool with_album = with_artist && album_lbl_ != nullptr && !album.empty() && block_h + g2 + ah <= space_h;
    if (with_album) block_h += g2 + ah;
    int y = text_pos_ == 1 ? 0 : (text_pos_ == 2 ? space_h - block_h : (space_h - block_h) / 2);
    if (y < 0) y = 0;
    place_(title_lbl_, x0_, y, title_w);
    int line_y = y + th + g2;
    place_(artist_lbl_, x0_, line_y, text_w_);
    line_y += ah + g2;
    if (bpm_lbl_ != nullptr) {
      ui::set_text(bpm_lbl_, with_bpm ? bpm_text : std::string());
      place_(bpm_lbl_, x0_, line_y, text_w_);
      if (with_bpm) line_y += ah + g2;
    }
    if (album_lbl_ != nullptr) {
      ui::set_text(album_lbl_, with_album ? album : std::string());
      place_(album_lbl_, x0_, line_y, text_w_);
    }

    // The small volume display: a text and the speaker, at the title's right. A tap on it mutes (option).
    if (vol_lbl_ != nullptr) {
      ui::set_text(vol_lbl_, vtext);
      lv_obj_set_width(vol_lbl_, vol_tw);
      lv_obj_set_pos(vol_lbl_, text_right_ - vol_total, y + (th - ui::line_height(Font::BODY)) / 2);
      ui::set_text(vol_icon_lbl_, vol_iw > 0 ? vic : std::string());
      lv_obj_set_width(vol_icon_lbl_, vol_iw);
      lv_obj_set_pos(vol_icon_lbl_, text_right_ - vol_iw, y + (th - ui::line_height(Font::ICON_SMALL)) / 2);
    }
    vol_hit_x_ = text_right_ - vol_total;
    vol_hit_w_ = vol_total;
    vol_hit_y_ = y;
    vol_hit_h_ = th;

    // The position: that of the last update plus the time that has passed since while it plays. Some players (HASS.Agent
    // with Spotify) report a position a few seconds old as if it were new, so the bar would jump back and forth. While
    // the same track plays on, the tile therefore keeps its own clock and follows a report only when it differs by
    // more than CLOCK_SLACK seconds (a seek); a new track, a pause or a start sets the clock anew.
    const bool timeline = active && duration_ > 0.5f;
    float pos = position_;
    if (timeline) {
      if (playing && updated_ > 0 && epoch > updated_) pos += static_cast<float>(epoch - updated_);
      const uint32_t now = millis();
      const tessera::Text track = title_ + "|" + artist_;
      const float own = clock_pos_ + static_cast<float>(now - clock_ms_) / 1000.0f;
      if (playing && clock_run_ && track == clock_track_ && std::fabs(pos - own) <= CLOCK_SLACK) {
        pos = own;
      } else {
        clock_pos_ = pos;
        clock_ms_ = now;
        clock_track_ = track;
      }
      clock_run_ = playing;
      if (pos < 0.0f) pos = 0.0f;
      if (pos > duration_) pos = duration_;
    } else {
      clock_run_ = false;
    }
    int track_w = 0, fill_w = 0;
    if (show_bar_ && timeline) {
      track_w = area_w_;
      fill_w = static_cast<int>(static_cast<float>(area_w_) * (pos / duration_));
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
      const int fh = static_cast<int>(static_cast<float>(vbar_fh_) * lvl);
      lv_obj_set_height(vbar_fill_, fh);
      lv_obj_set_y(vbar_fill_, vbar_fy_ + vbar_fh_ - fh);
    }

    // The buttons: an icon each; the speaker for muting shows whether it is muted.
    for (int i = 0; i < n_; ++i) {
      uint32_t ic = ICON_PLAY;
      switch (kinds_[i]) {
        case PREV: ic = ICON_PREV; break;
        case PLAY_PAUSE: ic = playing ? ICON_PAUSE : ICON_PLAY; break;
        case NEXT: ic = ICON_NEXT; break;
        case VOL_DOWN: ic = ICON_VOLUME_DOWN; break;
        case VOL_UP: ic = ICON_VOLUME_UP; break;
        case MUTE: ic = muted_ ? ICON_MUTED : ICON_VOLUME; break;
      }
      ui::set_text(btn_[i], ui::icon(ic));
    }

    // The big volume display: the number in the largest font that fits, and the speaker after it.
    if (ov_bg_ != nullptr) {
      if (ov_on_) {
        const std::string num = muted_ ? plugin_->text("muted") : std::to_string(pct) + "%";
        const std::string ic = volume_icon_(muted_);
        const int iw = ui::text_width(ic, Font::ICON);
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
        ui::set_text(ov_icon_, ic);
        lv_obj_set_width(ov_icon_, iw + ui::px(2));
        lv_obj_set_pos(ov_icon_, x + nw + gap, (height_ - ui::line_height(Font::ICON)) / 2);
      } else {
        lv_obj_set_size(ov_bg_, 0, 0);
        ui::set_text(ov_num_, "");
        ui::set_text(ov_icon_, "");
      }
    }

    // The pause display: the pause sign over the whole tile while the player is paused (not while the big volume
    // display is there).
    if (pz_bg_ != nullptr) {
      if (has_entity_ && paused_ && !title_.empty() && !ov_on_) {
        // Two thirds of the tile's height (of its width, when that is smaller), the bars a third of that wide.
        int sh = height_ * 2 / 3;
        if (sh > width_ * 2 / 3) sh = width_ * 2 / 3;
        const int bw = sh / 3, sgap = sh / 4;
        const int sx = (width_ - 2 * bw - sgap) / 2, sy = (height_ - sh) / 2;
        lv_obj_set_size(pz_bg_, width_, height_);
        for (int i = 0; i < 2; ++i) {
          lv_obj_set_size(pz_bar_[i], bw, sh);
          lv_obj_set_pos(pz_bar_[i], sx + i * (bw + sgap), sy);
        }
      } else {
        lv_obj_set_size(pz_bg_, 0, 0);
        for (auto *bar : pz_bar_) lv_obj_set_size(bar, 0, 0);
      }
    }
  }

  void on_theme() override {
    ui::set_color(title_lbl_, paused_ ? theme::MUTED : theme::INK);
    ui::set_color(artist_lbl_, theme::INK_SOFT);
    if (album_lbl_ != nullptr) ui::set_color(album_lbl_, theme::MUTED);
    if (bpm_lbl_ != nullptr) ui::set_color(bpm_lbl_, theme::MUTED);
    if (time_lbl_ != nullptr) ui::set_color(time_lbl_, theme::INK_SOFT);
    if (vol_lbl_ != nullptr) ui::set_color(vol_lbl_, theme::MUTED);
    if (vol_icon_lbl_ != nullptr) ui::set_color(vol_icon_lbl_, theme::MUTED);
    ui::set_fill(track_, tessera::Tone::NORMAL, theme::TRACK);
    ui::set_fill(fill_, bar_tone_, bar_role_);
    if (vbar_frame_ != nullptr) {
      ui::set_fill(vbar_frame_, tessera::Tone::NORMAL, theme::LINE);
      ui::set_fill(vbar_inner_, tessera::Tone::NORMAL, theme::CARD);
      ui::set_fill(vbar_fill_, bar_tone_, bar_role_);
    }
    for (int i = 0; i < n_; ++i) {
      ui::set_color(btn_[i], theme::INK);
      ui::set_fill(frame_[i], tessera::Tone::NORMAL, theme::LINE);
      ui::set_fill(inner_[i], tessera::Tone::NORMAL, theme::CARD);
    }
    if (pz_bg_ != nullptr) {
      ui::set_fill(pz_bg_, tessera::Tone::NORMAL, theme::CARD);
      lv_obj_set_style_bg_opa(pz_bg_, LV_OPA_70, 0);  // after the fill, which may set its own
      for (auto *bar : pz_bar_) ui::set_fill(bar, tessera::Tone::NORMAL, theme::INK);
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
    // 2. The volume bar: a tap sets the volume where it hit, from 0 at the bottom to 100 at the top.
    if (known && vbar_w_ > 0 && x >= vbar_x_ - gap_ / 2) {
      float lvl = static_cast<float>(vbar_fy_ + vbar_fh_ - y) / static_cast<float>(vbar_fh_);
      if (lvl < 0.0f) lvl = 0.0f;
      if (lvl > 1.0f) lvl = 1.0f;
      char b[16];
      snprintf(b, sizeof b, "%.2f", lvl);
      tessera::action("media_player.volume_set", entity_.c_str(), "volume_level", b);
      return;
    }
    // 3. A button.
    if (known && n_ > 0 && y >= height_ - ctrl_h_) {
      int i = (x - area_x_) / (btn_w_ + btn_gap_);
      if (i < 0) i = 0;
      if (i >= n_) i = n_ - 1;
      if (kinds_[i] == MUTE) {
        mute_();
      } else {
        const char *service = service_for(kinds_[i]);
        if (service != nullptr) tessera::action(service, entity_.c_str());
      }
      return;
    }
    // 4. The small volume display: a tap mutes (option).
    if (known && want_mute_ && vol_hit_w_ > 0 && x >= vol_hit_x_ - ui::mm(2) && y >= vol_hit_y_ - ui::mm(1) &&
        y <= vol_hit_y_ + vol_hit_h_ + ui::mm(2)) {
      mute_();
      return;
    }
    // 5. The progress bar: a tap seeks to where it hit (option).
    if (known && show_bar_ && duration_ > 0.5f && y >= bar_y_ - ui::mm(3) &&
        y <= bar_y_ + bar_h_ + ui::mm(2)) {
      float frac = static_cast<float>(x - area_x_) / static_cast<float>(area_w_);
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
  // Puts a label at x, y with the width w; it keeps the height of one line of its font.
  static void place_(lv_obj_t *label, int x, int y, int w) {
    lv_obj_set_pos(label, x, y);
    lv_obj_set_width(label, w);
  }

  void mute_() {
    tessera::action("media_player.volume_mute", entity_.c_str(), "is_volume_muted", muted_ ? "false" : "true");
  }

  // Shows the cover file of the option "Cover address"; a "{v}" in it becomes a number that changes with the track.
  void apply_cover_url_(bool active) {
    // "{v}" in the address is replaced by the number of the track; an address without it is used as it is.
    std::string addr = roomy::plain(cover_url_);
    const size_t pos = addr.find("{v}");
    if (pos != std::string::npos)
      addr.replace(pos, 3, std::to_string(cover_key_ + cover_gen_));
    cover_on_ = active && picture_allowed_(addr.c_str());
    show_cover_(cover_on_ ? addr.c_str() : "");
  }

  // Shows the cover place with that picture, or hides it for "". The same address again does nothing.
  void show_cover_(const char *url) {
    if (*url != '\0')
      lv_obj_clear_flag(cover_, LV_OBJ_FLAG_HIDDEN);
    else
      lv_obj_add_flag(cover_, LV_OBJ_FLAG_HIDDEN);
    ui::set_picture(cover_, url, tessera::Fit::COVER);
  }

  const Scmt *plugin_;
  lv_obj_t *cover_{nullptr};
  lv_obj_t *title_lbl_{nullptr};
  lv_obj_t *artist_lbl_{nullptr};
  lv_obj_t *album_lbl_{nullptr};
  lv_obj_t *bpm_lbl_{nullptr};
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
  lv_obj_t *pz_bg_{nullptr};
  lv_obj_t *pz_bar_[2]{nullptr, nullptr};
  Btn kinds_[6]{};
  int n_{0};
  int width_{0}, height_{0}, text_w_{0}, x0_{0}, text_right_{0};
  bool cover_right_{false};
  int text_pos_{0};  // 0 middle, 1 top, 2 bottom
  int cover_size_{0};
  bool cover_on_{false};
  tessera::Text cover_url_, cover_track_;
  uint32_t cover_key_{0}, cover_gen_{0}, cover_retry_at_{0};
  bool cover_retry_{false};
  int bar_h_{0}, bar_y_{0}, time_y_{0}, ctrl_h_{0}, content_h_{0};
  int gap_{0};                                   // the space between the parts, the card's own padding
  int area_x_{0}, area_w_{0};                    // the area beside the volume bar, for everything else
  int vbar_x_{0}, vbar_w_{0}, vbar_fy_{0}, vbar_fh_{1};  // the volume bar, and where its fill may reach
  int btn_w_{0}, btn_gap_{0};
  int vol_hit_x_{0}, vol_hit_w_{0}, vol_hit_y_{0}, vol_hit_h_{0};
  bool show_bar_{true}, show_time_{false}, show_album_{false}, show_bpm_{false};
  tessera::Tone bar_tone_{tessera::Tone::ACCENT};  // colour of both bars (option "bar_color")
  theme::Role bar_role_{theme::ACCENT};
  tessera::Text bpm_track_;                      // the track the tempo belongs to
  bool bpm_seen_{false}, bpm_hold_{false}, bpm_cleared_{false};
  uint32_t bpm_hold_until_{0};
  bool want_mute_{false};
  int text_size_{1};  // 0 small, 1 medium, 2 large
  int vol_mode_{1};  // 0 never, 1 for three seconds after a change, 2 always
  float volume_{-1.0f}, last_volume_{-1.0f};
  bool muted_{false}, vol_flag_{false}, ov_on_{false};
  uint32_t vol_until_{0};
  bool has_entity_{false};
  bool paused_{false};
  tessera::Text entity_, state_, title_, artist_, album_;
  float duration_{0.0f}, position_{0.0f};
  uint32_t updated_{0};
  // The tile's own clock for the position (see on_tick): the position at clock_ms_ (millis) and the track it belongs to.
  static constexpr float CLOCK_SLACK = 3.0f;
  float clock_pos_{0.0f};
  uint32_t clock_ms_{0};
  bool clock_run_{false};
  tessera::Text clock_track_;
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
    const tessera::Text key = "btn_" + action_;
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
  tessera::Text entity_, action_;
};

int Scmt::bpm() const {
#ifdef USE_SENSOR
  // "unknown" (BPM2SCMT has no tempo yet, or nothing plays) comes as NaN.
  if (bpm_ != nullptr && bpm_->has_state() && !std::isnan(bpm_->state) && bpm_->state >= 20.0f && bpm_->state <= 400.0f)
    return static_cast<int>(std::lround(bpm_->state));
#endif
  return 0;
}

void Scmt::setup() {
  add_tile("now_playing", [this]() { return new NowPlayingTile(this); });
  add_tile("media_button", [this]() { return new MediaButtonTile(this); });
}

}  // namespace scmt
}  // namespace esphome
