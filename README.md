# Now playing

Two tiles for Tessera that show and control what a Home Assistant media player is playing.

## Tile "Now playing"

The title in large letters, the artist below it, and an optional progress bar at the bottom.

- While something plays, the title and the artist. While paused, the same, with the title dimmed.
- When nothing plays, "Nothing playing"; before a player is chosen, "Choose a player".
- A title that is too wide for the tile gets a smaller font first, then is cut with dots.
- The option "Progress" shows a bar, the time (played / length), both, or nothing. It needs the player to report
  `media_duration` and `media_position`; a player that does not shows neither.
- The option "Volume display" shows the volume at the title's right: for three seconds after it changed, always, or
  never. It needs `volume_level`; a muted player shows "Muted".
- A tap on the tile plays or pauses.
- The album cover stands beside the title, when the tile is at least two cells wide and the screen can show pictures
  (the small boards cannot). It can be switched off in the tile's options.
- Two options add buttons under the title: "Playback buttons" (previous, play/pause, next) and "Volume buttons"
  (quieter, louder). They need a tile of two rows or more.

### More options of this tile

All off until you switch them on in the tile's options:

- **Big volume display:** for three seconds after the volume changes, a half see-through display with the number and a
  speaker lies over the whole tile. With the option "Mute", a tap on it mutes.
- **Seek by tapping the bar:** a tap on the progress bar jumps to that place in the song.
- **Mute:** a mute button among the buttons; a tap on the volume display mutes or unmutes.
- **Volume bar:** a bar under the text; a tap on it sets the volume to that place. Needs two rows.
- **Album name:** the album as a third line, when the tile is high enough.

## Tile "Media button"

One button for the same media player: previous, play/pause, next, louder or quieter. Choose the button in the tile's
options and place as many of these tiles as you like, next to the "Now playing" tile.

## Setup

1. Add the plugin to your screen in Tessera (Plugins) and build the screen.
2. Place the tiles in the layout.
3. In each tile's inspector, choose the media player. Any `media_player` entity works, for example the one that
   HASS.Agent makes from the Windows media session, or the Spotify integration.

The cover is the picture Home Assistant itself has for the media player (its `entity_picture`); the plugin fetches
nothing from the internet.

The buttons use the Home Assistant actions `media_player.media_play_pause`, `media_next_track`,
`media_previous_track`, `volume_up`, `volume_down`, `media_seek`, `volume_mute` and `volume_set`. A player that does not support one of them ignores it.

## Privacy

The plugin makes no network requests. It only reads what Home Assistant already knows about the chosen entity, and
calls the media player actions above on it when you tap a tile.
