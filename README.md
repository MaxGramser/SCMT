# Now playing

A tile for Tessera that shows what a Home Assistant media player is playing right now: the title in large letters, the artist below it.

## What it shows

- While something plays, the title and the artist.
- While paused, the same, with the title dimmed.
- When nothing plays, "Nothing playing"; before a player is chosen, "Choose a player".
- A title that is too wide for the tile gets a smaller font first, then is cut with dots.

## Setup

1. Add the plugin to your screen in Tessera (Plugins) and build the screen.
2. Place the **Now playing** tile in the layout.
3. In the tile's inspector, choose the media player. Any `media_player` entity works, for example the one that HASS.Agent makes from the Windows media session, or the Spotify integration.

The tile reads the entity's state and its `media_title` and `media_artist` attributes. A player that does not report them shows "Nothing playing".

## Privacy

The plugin makes no network requests and asks for no rights. It only shows what Home Assistant already knows about the chosen entity.
