# Now playing

Two tiles for Tessera that show and control what a Home Assistant media player is playing.

## Tile "Now playing"

The title in large letters, the artist below it, and an optional progress bar at the bottom.

- While something plays, the title and the artist. While paused, the same, with the title dimmed.
- When nothing plays, "Nothing playing"; before a player is chosen, "Choose a player".
- A title that is too wide for the tile gets a smaller font first, then is cut with dots.
- The progress bar can be switched off in the tile's options. It needs the player to report `media_duration` and
  `media_position`; a player that does not shows no bar.
- A tap on the tile plays or pauses.

## Tile "Media button"

One button for the same media player: previous, play/pause, next, louder or quieter. Choose the button in the tile's
options and place as many of these tiles as you like, next to the "Now playing" tile.

## Setup

1. Add the plugin to your screen in Tessera (Plugins) and build the screen.
2. Place the tiles in the layout.
3. In each tile's inspector, choose the media player. Any `media_player` entity works, for example the one that
   HASS.Agent makes from the Windows media session, or the Spotify integration.

The buttons use the Home Assistant actions `media_player.media_play_pause`, `media_next_track`,
`media_previous_track`, `volume_up` and `volume_down`. A player that does not support one of them ignores it.

## Privacy

The plugin makes no network requests. It only reads what Home Assistant already knows about the chosen entity, and
calls the five media player actions above on it when you tap a tile.
