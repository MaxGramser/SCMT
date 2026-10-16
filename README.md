# SCMT - Simple Customizable Media Tile

Two tiles for Tessera that show and control what a Home Assistant media player is playing.

## Tile "SCMT"

The title in large letters, the artist below it, and an optional progress bar at the bottom.

- While something plays, the title and the artist. While paused, the same, with the title dimmed.
- When nothing plays, "Nothing playing"; before a player is chosen, "Choose a player".
- A title that is too wide for the tile gets a smaller font first, then is cut with dots.
- The option "Progress" shows a bar (its height is an option too, 2 to 16 pixels), the time (played / length), both, or nothing. It needs the player to report
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
- **Cover address:** a fixed address of a cover file in Home Assistant, shown instead of the player's own picture
  (see "The cover" below).

## Tile "SCMT button"

One button for the same media player: previous, play/pause, next, louder or quieter. Choose the button in the tile's
options and place as many of these tiles as you like, next to the "SCMT" tile.

## Setup

1. Add the plugin to your screen in Tessera (Plugins) and build the screen.
2. Place the tiles in the layout.
3. In each tile's inspector, choose the media player. Any `media_player` entity works, for example the one that
   HASS.Agent makes from the Windows media session, or the Spotify integration.

The buttons use the Home Assistant actions `media_player.media_play_pause`, `media_next_track`, `media_previous_track`, `volume_up`, `volume_down`, `media_seek`, `volume_mute` and `volume_set`. A player that does not support one of them ignores it.

## The cover

By default the cover is the picture Home Assistant itself has for the media player (its `entity_picture`); the plugin
fetches nothing from the internet.

A tile may show only pictures from `/api/image/serve/`, `/api/image_proxy/` or `/local/`, and text attributes are cut
at 48 bytes. The cover of some players (for example the one of HASS.Agent, `/api/media_player_proxy/...`) is refused
for that reason. For these, use the option "Cover address":

1. Let something save the current cover as a file in Home Assistant's `www` folder, for example `/config/www/cover.jpg`
   (an automation that downloads the player's `entity_picture` with a `shell_command`).
2. Enter `/local/cover.jpg?v={v}` as the "Cover address" in the tile's options.

The tile then shows that file instead of the player's picture, while something plays or is paused. The address is used
exactly as typed, except for `{v}`: the plugin replaces it with a number that changes with every track, so the screen
asks again (Home Assistant ignores the `?v=...` part and sends the file). Three seconds after a new track it asks once
more, in case the file was written a moment late. An address without `{v}` never changes, so the screen shows the first
picture it loaded until the address changes.

## Privacy

The plugin makes no network requests. It only reads what Home Assistant already knows about the chosen entity, and
calls the media player actions above on it when you tap a tile.
