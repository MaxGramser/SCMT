# Changelog

## 0.8.1 - 2026-10-16

- The "Cover address" is now used exactly as typed. A `{v}` in it is replaced by a number that changes with every
  track (for example `/local/cover.jpg?v={v}`); before, that number was always added behind the address as `?v=`.

## 0.8.0 - 2026-10-15

- New option "Cover address": a fixed address of a cover file in Home Assistant (for example `/local/cover.jpg`),
  shown instead of the player's own picture. For players whose cover the tile may not fetch.

## 0.7.1 - 2026-10-15

- Fix: the build failed with "Component not found: now_playing", because `plugin.yaml` still named the old
  component. It now says `scmt`, the same as the manifest and the folder.

## 0.7.0 - 2026-10-15

- The plugin's id is now `scmt` (it was `now_playing`), the same as its folder and its component. The app sees a new
  plugin: remove the old one from the screen, add this one, and place its tiles again.

## 0.6.0 - 2026-10-15

- The plugin is called SCMT (Simple Customizable Media Tile). Its id, `now_playing`, stays, so tiles already on a
  screen keep working.
- New option "Height of the progress bar" (2 to 16 pixels, 4 by default).

## 0.5.0 - 2026-10-15

- New options: "Seek by tapping the bar", "Mute", "Volume bar" (a tap sets the volume), "Album name" (a third line).
- New option "Big volume display": a half see-through display over the whole tile for three seconds after a change,
  with the number in large digits and a speaker after it. The small volume display has a speaker too.
- The tile can be up to 4x4 cells (the editor offers what fits the screen's grid).
- Needs three more actions: `media_seek`, `volume_mute`, `volume_set`.

## 0.4.1 - 2026-10-14

- Fix: the manifest was refused because the choice value `off` was read as a truth value; every choice value is
  now written as text.

## 0.4.0 - 2026-10-14

- New: the option "Progress" now chooses a bar, the time (1:36 / 7:36), both, or nothing.
- New: the option "Volume display" shows the volume at the title's right: when it changes (three seconds), always,
  or never. A muted player shows "Muted".

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
