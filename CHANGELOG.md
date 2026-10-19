# Changelog

## 0.14.0 - 2026-10-19

- The buttons show Tessera's icons (previous, play, pause, next, quieter, louder, speaker) instead of signs such as
  "<<" and "||".
- The speaker of the volume display shows volume-high, or volume-off while muted. The icons for a low and a medium
  volume are not in Tessera's set, so below 67 % the speaker showed nothing.
- A title, an artist or an album that is too wide now ends in dots instead of running as a marquee, as every label of
  the screen. The tile no longer keeps the screen drawing while it is on the glass.
- The tempo line ("138 BPM") comes from the translations.
- The tile keeps its texts in the memory beside the chip (PSRAM) where the board has it, not inside the chip.
- The README says which board SCMT was tried on.

## 0.13.0 - 2026-10-18

- New option "Colour of the bars" for the progress bar and the volume bar: the screen's accent (default, as before),
  green, amber, red, blue, purple, or like the text. These are the core's colours, readable in light and dark; Tessera
  allows no colour of your own.
- "Album name" and "Tempo" are now one option, "Extra lines" (none, album, tempo, or both), to make room for the colour
  (a tile has at most 12 options). Tiles that had the two switches keep what they showed.

## 0.12.1 - 2026-10-18

- The progress bar and the time no longer jump back and forth when a player reports a position a few seconds old as if
  it were new (HASS.Agent with Spotify). While the same track plays on, the tile keeps its own clock and follows a
  report only when it differs by more than 3 seconds (a seek); a new track, a pause or a start sets the clock anew.

## 0.12.0 - 2026-10-17

- New option "Tempo": the tempo of the music (for example "138 BPM") as a line under the artist. It comes from a sensor
  chosen per screen in Tessera (new input "Tempo sensor"), for example the one HASS.Agent makes from BPM2SCMT. After a
  new track the line waits until the sensor has the new tempo.
- The plugin now reads that one entity itself (permission `read_entities`); Tessera asks for it on the update.
- The README describes a new way to the cover for players whose own cover is refused: a template image in Home
  Assistant, shown with the "Cover address" `/api/image_proxy/image.scmt_cover?v={v}`. It replaces the cover
  automation with a file under `/local/`, which does not work with Tessera 0.4.88 (403). The plugin is unchanged there.

## 0.11.0 - 2026-10-17

- New option "Text size": small (as before 0.10.3), medium (default, as now) or large (artist and album one step
  larger, a long title runs in the large font).
- A tap on the progress bar always seeks; the option "Seek by tapping the bar" is gone.
- Mute comes with the volume buttons, between quieter and louder; the option "Mute" is gone. With the volume buttons,
  a tap on a volume display mutes too.

## 0.10.3 - 2026-10-17

- Larger text: the title in the large font of a card's value, artist and album in the font of a key's words. A long
  title gets smaller only down to the former size and then runs as a marquee; on a low tile everything steps down as
  before.
- Fix: text that runs as a marquee no longer runs over the cover or the card's edge.

## 0.10.2 - 2026-10-17

- The volume bar always stands at the right edge, also when the cover is at the right, and is a third narrower.
- Even spacing: the cover, the text, the progress bar and the buttons now stand as far apart as from the card's edge.

## 0.10.1 - 2026-10-17

- Fix: on a low tile (the small 2.8-inch screen) the artist no longer lies over the time. The title takes a smaller font
  when it is too high as well, and stands alone when there is no room for the artist.
- The README now says that the option "Cover address" and the cover automation do not work with the current Tessera
  app (0.4.88): it fetches `/local/` through the Supervisor, which refuses it (403), so the tile shows a grey block.
- The picture for the plugin store shows the new look.

## 0.10.0 - 2026-10-16

- A title, artist or album too wide for the tile runs as a marquee instead of being cut with dots.
- The volume bar stands upright along the edge of the tile, opposite the cover (at the right, or at the left when the
  cover is at the right). It now needs two columns instead of two rows.
- "Seek by tapping the bar" is on by default.
- Even spacing: the same gap between the cover, the text, the progress bar and the buttons, and the outer buttons line
  up with the edges.
- With the text at the top or the bottom, it lines up with the cover's upper or lower edge.

## 0.9.1 - 2026-10-16

- Fix: the manifest was refused ("tiles[0].options: at most 12 entries"), because the tile "SCMT" had 15 options.
  Three pairs are now one choice each, and nothing is lost:
  - "Album cover": left, right or off (was "Album cover" and "Cover position").
  - "Buttons": none, playback, volume, or both (was "Playback buttons" and "Volume buttons").
  - "Big display": none, volume, pause, or both (was "Big volume display" and "Pause display").
- Tiles placed with an older version keep their settings; the tile still reads the former options.

## 0.9.0 - 2026-10-16

- New option "Pause display": while the player is paused, a half see-through display with the pause sign lies over the
  whole tile.
- New option "Cover position": the album cover at the left or at the right of the text.
- New option "Position of the text": title, artist and album at the top, in the middle or at the bottom of the space
  beside the cover.
- More languages: French, Spanish, Italian and Dutch, beside English and German. The README and this changelog come in
  these languages too (`README.de.md`, `CHANGELOG.de.md`, and so on).
- The README describes the automation that brings an album cover to the display (experimental), with an example in
  `examples/cover_automation.yaml`.
- The tile reserves a little more memory for the new display.

## 0.8.1 - 2026-10-16

- The "Cover address" is now used exactly as typed. A `{v}` in it is replaced by a number that changes with every track
  (for example `/local/cover.jpg?v={v}`); before, that number was always added behind the address as `?v=`.
- The store picture `store.png` for the plugin store was added.

## 0.8.0 - 2026-10-15

- New option "Cover address": a fixed address of a cover file in Home Assistant (for example `/local/cover.jpg`), shown
  instead of the player's own picture. For players whose cover the tile may not fetch.

## 0.7.1 - 2026-10-15

- Fix: the build failed with "Component not found: now_playing", because `plugin.yaml` still named the old component. It
  now says `scmt`, the same as the manifest and the folder.

## 0.7.0 - 2026-10-15

- The plugin's id is now `scmt` (it was `now_playing`), the same as its folder and its component. The app sees a new
  plugin: remove the old one from the screen, add this one, and place its tiles again.

## 0.6.0 - 2026-10-15

- The plugin is called SCMT (Simple Customizable Media Tile). Its id, `now_playing`, stays, so tiles already on a screen
  keep working.
- New option "Height of the progress bar" (2 to 16 pixels, 4 by default).

## 0.5.0 - 2026-10-15

- New options: "Seek by tapping the bar", "Mute", "Volume bar" (a tap sets the volume), "Album name" (a third line).
- New option "Big volume display": a half see-through display over the whole tile for three seconds after a change, with
  the number in large digits and a speaker after it. The small volume display has a speaker too.
- The tile can be up to 4x4 cells (the editor offers what fits the screen's grid).
- Needs three more actions: `media_seek`, `volume_mute`, `volume_set`.

## 0.4.1 - 2026-10-14

- Fix: the manifest was refused because the choice value `off` was read as a truth value; every choice value is now
  written as text.

## 0.4.0 - 2026-10-14

- New: the option "Progress" now chooses a bar, the time (1:36 / 7:36), both, or nothing.
- New: the option "Volume display" shows the volume at the title's right: when it changes (three seconds), always, or
  never. A muted player shows "Muted".

## 0.3.2 - 2026-10-14

- The buttons in the tile have a light frame.

## 0.3.1 - 2026-10-14

- The cover place is shown only when the player has a picture the screen may fetch; otherwise the text takes the whole
  width, and no empty grey square stays.

## 0.3.0 - 2026-10-14

- New: the album cover beside the title (option "Album cover"), from two cells wide, on screens that show pictures.
- New: playback buttons (previous, play/pause, next) and volume buttons (quieter, louder) inside the tile, as options.
  They need a tile of two rows or more.
- Needs plugin API 0.10.

## 0.2.0 - 2026-10-14

- New: a progress bar on the tile (option "Progress bar", on by default), if the player reports duration and position.
- New: a tap on the tile plays or pauses.
- New: the tile "Media button" (previous, play/pause, next, louder, quieter), one tile per button.
- The tile can now be 1x1; it was 2x1 at least.

## 0.1.1 - 2026-10-14

- Fix: the build failed because the colours were written as `tessera::theme::`; they are `theme::`.

## 0.1.0 - 2026-10-14

- First version: shows the title and artist of a media player.
