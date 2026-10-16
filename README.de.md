# SCMT - Simple Customizable Media Tile

[English](README.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Nederlands](README.nl.md)

Zwei Kacheln für Tessera, die anzeigen und steuern, was ein Media Player in Home Assistant gerade spielt.

## Kachel „SCMT“

Der Titel in großen Buchstaben, darunter der Interpret und unten optional ein Fortschrittsbalken.

- Solange etwas läuft: Titel und Interpret. Bei Pause dasselbe, mit gedämpftem Titel.
- Wenn nichts läuft: „Nichts läuft“; solange noch kein Player gewählt ist: „Player wählen“.
- Ein Titel, der zu breit für die Kachel ist, bekommt zuerst eine kleinere Schrift und wird dann mit Punkten gekürzt.
- Die Option „Fortschritt“ zeigt einen Balken (seine Höhe ist auch eine Option, 2 bis 16 Pixel), die Zeit (gespielt /
  Länge), beides oder nichts. Dafür muss der Player `media_duration` und `media_position` melden; sonst erscheint
  weder das eine noch das andere.
- Die Option „Lautstärkeanzeige“ zeigt die Lautstärke rechts neben dem Titel: drei Sekunden nach einer Änderung,
  immer oder nie. Sie braucht `volume_level`; ein stummer Player zeigt „Stumm“.
- Ein Tipp auf die Kachel startet oder pausiert die Wiedergabe.
- Das Albumcover steht neben dem Titel, wenn die Kachel mindestens zwei Zellen breit ist und der Bildschirm Bilder
  zeigen kann (die kleinen Boards können das nicht). Die Option „Albumcover“ setzt es links (Standard) oder rechts vom
  Text, oder schaltet es ab.
- Die Option „Tasten“ fügt Tasten unter dem Titel hinzu: Wiedergabe (zurück, Play/Pause, weiter), Lautstärke (leiser,
  lauter) oder beides. Sie brauchen eine Kachel mit zwei Zeilen oder mehr.

### Weitere Optionen dieser Kachel

- **Position des Textes:** Titel, Interpret und Album in der Mitte (Standard), oben oder unten in dem Platz neben dem
  Cover.
- **Große Anzeige:** eine halb durchsichtige Anzeige über der ganzen Kachel, für die Lautstärke, für die Pause oder für
  beides. Standardmäßig aus.
  - Lautstärke: drei Sekunden nach einer Änderung der Lautstärke, mit der Zahl und einem Lautsprecher. Mit der Option
    „Stumm“ schaltet ein Tipp darauf stumm.
  - Pause: solange der Player pausiert ist, das Pause-Zeichen.
- **Spulen per Tipp auf den Balken:** Ein Tipp auf den Fortschrittsbalken springt an diese Stelle im Titel.
- **Stumm:** eine Stumm-Taste bei den Tasten; ein Tipp auf die Lautstärkeanzeige schaltet stumm oder wieder laut.
- **Lautstärkeleiste:** eine Leiste unter dem Text; ein Tipp darauf stellt die Lautstärke an dieser Stelle ein. Braucht
  zwei Zeilen.
- **Albumname:** das Album als dritte Zeile, wenn die Kachel hoch genug ist.
- **Cover-Adresse:** eine feste Adresse einer Cover-Datei in Home Assistant, die statt des eigenen Bildes des Players
  angezeigt wird (siehe „Das Cover“ unten).

## Kachel „SCMT-Taste“

Eine Taste für denselben Media Player: zurück, Play/Pause, weiter, lauter oder leiser. Die Taste wählst du in den
Optionen der Kachel, und du kannst beliebig viele dieser Kacheln neben der Kachel „SCMT“ platzieren.

## Einrichtung

1. Das Plugin in Tessera (Plugins) zum Bildschirm hinzufügen und den Bildschirm bauen.
2. Die Kacheln im Layout platzieren.
3. Im Inspektor jeder Kachel den Media Player wählen. Jede `media_player`-Entity funktioniert, zum Beispiel die, die
   HASS.Agent aus der Windows-Mediensitzung macht, oder die Spotify-Integration.

Die Tasten nutzen die Home-Assistant-Aktionen `media_player.media_play_pause`, `media_next_track`,
`media_previous_track`, `volume_up`, `volume_down`, `media_seek`, `volume_mute` und `volume_set`. Ein Player, der eine
davon nicht unterstützt, ignoriert sie.

## Das Cover

Standardmäßig ist das Cover das Bild, das Home Assistant selbst für den Media Player hat (sein `entity_picture`); das
Plugin holt nichts aus dem Internet.

Eine Kachel darf nur Bilder von `/api/image/serve/`, `/api/image_proxy/` oder `/local/` zeigen, und Textattribute
werden bei 48 Bytes abgeschnitten. Das Cover mancher Player (zum Beispiel das von HASS.Agent,
`/api/media_player_proxy/...`) wird deshalb abgelehnt. Für diese gibt es die Option „Cover-Adresse“ zusammen mit der
Automation unten.

### Cover-Automation (experimentell)

Die Automation speichert das aktuelle Cover des Players als Datei im Ordner `www` von Home Assistant. Die Kachel zeigt
dann diese Datei. Die Seite in Home Assistant funktioniert; ob dein Bildschirm die Datei anzeigt, hängt von deiner
Tessera-Version ab, deshalb gilt sie als experimentell. Derselbe Code steht in `examples/cover_automation.yaml`.

1. Den Ordner `/config/www/` anlegen, falls er fehlt, und Home Assistant einmal neu starten.
2. Den Befehl in die `configuration.yaml` eintragen (danach die Shell-Befehle neu laden oder neu starten). Es steht
   absichtlich kein `&&` darin: Home Assistant führt Befehle mit `{{ }}` ohne Shell aus.

```yaml
shell_command:
  cover_fetch: >-
    curl -sSfkL --connect-timeout 5 --max-time 20 -o /config/www/cover.jpg "{{ url }}"
```

3. Die Automation anlegen. `media_player.your_player` durch deinen Player und `YOUR_HOME_ASSISTANT` durch die Adresse
   ersetzen, unter der du Home Assistant öffnest, so wie der Rechner von Home Assistant sie sieht (zum Beispiel
   `http://192.168.0.10:8123`, oder `https://ha.example.com`, dafür steht die Option `-k` schon im Befehl). Sie reagiert
   auf einen neuen Titel, wartet zwei Sekunden und holt das Cover dreimal (nach 2, 7 und 15 Sekunden), weil das neue
   Cover oft erst einige Sekunden nach dem Titel ankommt.

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

4. In den Optionen der Kachel `/local/cover.jpg?v={v}` als „Cover-Adresse“ eintragen.

Die Adresse wird genau so verwendet, wie sie eingegeben ist, bis auf `{v}`: Das ersetzt das Plugin durch eine Zahl, die
sich mit jedem Titel ändert, damit der Bildschirm neu nachfragt (Home Assistant ignoriert den Teil `?v=...` und liefert
die Datei). Drei Sekunden nach einem neuen Titel fragt es noch einmal nach, falls die Datei etwas später geschrieben
wurde. Eine Adresse ohne `{v}` ändert sich nie, der Bildschirm zeigt dann das erste geladene Bild, bis sich die Adresse
ändert.

Zum Prüfen der Datei `http://YOUR_HOME_ASSISTANT/local/cover.jpg?x=1` in einem privaten Browserfenster öffnen (die
Zahl jedes Mal ändern, weil der Browser das Bild behält).

## Datenschutz

Das Plugin macht keine Netzwerkanfragen. Es liest nur, was Home Assistant über die gewählte Entity schon weiß, und ruft
bei einem Tipp auf eine Kachel die oben genannten Media-Player-Aktionen auf. Die Cover-Automation läuft in Home
Assistant, nicht im Plugin.
