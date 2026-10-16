# SCMT - Simple Customizable Media Tile

[English](README.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Nederlands](README.nl.md)

Two tiles for Tessera that show and control what a Home Assistant media player is playing.

## Tile "SCMT"

The title in large letters, the artist below it, and an optional progress bar at the bottom.

- While something plays, the title and the artist. While paused, the same, with the title dimmed.
- When nothing plays, "Nothing playing"; before a player is chosen, "Choose a player".
- A title that is too wide for the tile gets a smaller font first, then runs as a marquee. So do an artist and an
  album that are too wide.
- The option "Progress" shows a bar (its height is an option too, 2 to 16 pixels), the time (played / length), both, or
  nothing. It needs the player to report `media_duration` and `media_position`; a player that does not shows neither.
- The option "Volume display" shows the volume at the title's right: for three seconds after it changed, always, or
  never. It needs `volume_level`; a muted player shows "Muted".
- A tap on the tile plays or pauses.
- The album cover stands beside the title, when the tile is at least two cells wide and the screen can show pictures
  (the small boards cannot). The option "Album cover" puts it at the left (default) or at the right of the text, or
  switches it off.
- The option "Buttons" adds buttons under the title: playback (previous, play/pause, next), volume (quieter, louder),
  or both. They need a tile of two rows or more.

### More options of this tile

- **Position of the text:** title, artist and album in the middle (default), at the top or at the bottom beside the
  cover. At the top or the bottom, the text lines up with the cover's upper or lower edge.
- **Big display:** a half see-through display over the whole tile, for the volume, for the pause, or both. Off by
  default.
  - Volume: for three seconds after the volume changes, the number and a speaker. With the option "Mute", a tap on it
    mutes.
  - Pause: while the player is paused, the pause sign.
- **Seek by tapping the bar:** a tap on the progress bar jumps to that place in the song. On by default.
- **Mute:** a mute button among the buttons; a tap on the volume display mutes or unmutes.
- **Volume bar:** an upright bar along the edge of the tile, opposite the cover (at the right, or at the left when the
  cover is at the right); a tap on it sets the volume to that height. Needs two columns.
- **Album name:** the album as a third line, when the tile is high enough.
- **Cover address:** a fixed address of a cover file in Home Assistant, shown instead of the player's own picture (see
  "The cover" below).

## Tile "SCMT button"

One button for the same media player: previous, play/pause, next, louder or quieter. Choose the button in the tile's
options and place as many of these tiles as you like, next to the "SCMT" tile.

## Setup

1. Add the plugin to your screen in Tessera (Plugins) and build the screen.
2. Place the tiles in the layout.
3. In each tile's inspector, choose the media player. Any `media_player` entity works, for example the one that
   HASS.Agent makes from the Windows media session, or the Spotify integration.

The buttons use the Home Assistant actions `media_player.media_play_pause`, `media_next_track`, `media_previous_track`,
`volume_up`, `volume_down`, `media_seek`, `volume_mute` and `volume_set`. A player that does not support one of them
ignores it.

## The cover

By default the cover is the picture Home Assistant itself has for the media player (its `entity_picture`); the plugin
fetches nothing from the internet.

A tile may show only pictures from `/api/image/serve/`, `/api/image_proxy/` or `/local/`, and text attributes are cut at
48 bytes. The cover of some players (for example the one of HASS.Agent, `/api/media_player_proxy/...`) is refused for
that reason.

**Note:** the option "Cover address" together with the automation below was meant for these players, but it does not
work with the current Tessera app (0.4.88). The app fetches an address under `/local/` through the Supervisor
(`http://supervisor/core/local/...`), which refuses it (403, logged by the app as "no picture from /local/cover.jpg"),
so the tile shows a grey block instead of the cover, whatever the format of the file. Both stay in the plugin until
Tessera fetches `/local/` in another way, or allows the player's own cover
([MaxGramser/tessera-plugins#6](https://github.com/MaxGramser/tessera-plugins/issues/6)).

### Cover automation (experimental)

The automation saves the player's current cover as a file in Home Assistant's `www` folder. The tile then shows that
file. The Home Assistant side works; whether your screen shows the file depends on your Tessera version, so treat it as
experimental. The same code is in `examples/cover_automation.yaml`.

1. Create the folder `/config/www/` if it does not exist, and restart Home Assistant once.
2. Add the command to your `configuration.yaml` (then reload the shell commands or restart). There is no `&&` in it on
   purpose: Home Assistant runs commands that contain `{{ }}` without a shell.

```yaml
shell_command:
  cover_fetch: >-
    curl -sSfkL --connect-timeout 5 --max-time 20 -o /config/www/cover.jpg "{{ url }}"
```

3. Create the automation. Replace `media_player.your_player` with your player and `YOUR_HOME_ASSISTANT` with the
   address under which you open Home Assistant, as the computer of Home Assistant sees it (for example
   `http://192.168.0.10:8123`, or `https://ha.example.com` with the option `-k` already in the command). It reacts to a
   new title, waits two seconds and fetches the cover three times (after 2, 7 and 15 seconds), because the new cover
   often arrives some seconds after the title.

```yaml
alias: Cover for Tessera
triggers:
  - trigger: state
    entity_id: media_player.your_player
    attribute: media_title
actions:
  - delay: "00:00:02"
  - condition: template
    value_template: "{{ state_attr('media_player.your_player', 'entity_picture') not in [none, ''] }}"
  - action: shell_command.cover_fetch
    data:
      url: "http://YOUR_HOME_ASSISTANT{{ state_attr('media_player.your_player', 'entity_picture') }}"
  - delay: "00:00:05"
  - action: shell_command.cover_fetch
    data:
      url: "http://YOUR_HOME_ASSISTANT{{ state_attr('media_player.your_player', 'entity_picture') }}"
  - delay: "00:00:08"
  - action: shell_command.cover_fetch
    data:
      url: "http://YOUR_HOME_ASSISTANT{{ state_attr('media_player.your_player', 'entity_picture') }}"
mode: restart
```

4. In the tile's options, enter `/local/cover.jpg?v={v}` as the "Cover address".

The address is used exactly as typed, except for `{v}`: the plugin replaces it with a number that changes with every
track, so the screen asks again (Home Assistant ignores the `?v=...` part and sends the file). Three seconds after a
new track it asks once more, in case the file was written a moment late. An address without `{v}` never changes, so
the screen shows the first picture it loaded until the address changes.

To check the file, open `http://YOUR_HOME_ASSISTANT/local/cover.jpg?x=1` in a private browser window (change the number
each time, because the browser keeps the picture).

## Privacy

The plugin makes no network requests. It only reads what Home Assistant already knows about the chosen entity, and
calls the media player actions above on it when you tap a tile. The cover automation runs in Home Assistant, not in the
plugin.
