# Changelog

## 0.5.0 - 2026-10-14
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
