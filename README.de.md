# SCMT - Simple Customizable Media Tile

[English](README.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Nederlands](README.nl.md)

Zwei Kacheln für Tessera, die anzeigen und steuern, was ein Media Player in Home Assistant gerade spielt.

## Kachel „SCMT“

Der Titel in großen Buchstaben, darunter der Interpret und unten optional ein Fortschrittsbalken.

- Solange etwas läuft: Titel und Interpret. Bei Pause dasselbe, mit gedämpftem Titel.
- Wenn nichts läuft: „Nichts läuft“; solange noch kein Player gewählt ist: „Player wählen“.
- Ein Titel, der zu breit für die Kachel ist, bekommt zuerst eine kleinere Schrift und endet dann mit Punkten, wie
  jede Beschriftung des Bildschirms. Ebenso ein Interpret und ein Album, die zu breit sind.
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
  stumm, lauter) oder beides. Sie brauchen eine Kachel mit zwei Zeilen oder mehr. Mit den Lautstärke-Tasten schaltet
  auch ein Tipp auf eine Lautstärkeanzeige stumm.
- Ein Tipp auf den Fortschrittsbalken springt an diese Stelle im Titel.

### Weitere Optionen dieser Kachel

- **Position des Textes:** Titel, Interpret und Album in der Mitte (Standard), oben oder unten neben dem Cover. Oben
  oder unten schließt der Text bündig mit der Ober- oder Unterkante des Covers ab.
- **Große Anzeige:** eine halb durchsichtige Anzeige über der ganzen Kachel, für die Lautstärke, für die Pause oder für
  beides. Standardmäßig aus.
  - Lautstärke: drei Sekunden nach einer Änderung der Lautstärke, mit der Zahl und einem Lautsprecher. Mit den
    Lautstärke-Tasten schaltet ein Tipp darauf stumm.
  - Pause: solange der Player pausiert ist, das Pause-Zeichen.
- **Schriftgröße:** klein, mittel (Standard) oder groß, für Titel, Interpret und Album.
- **Lautstärkeleiste:** eine aufrechte Leiste am rechten Rand der Kachel; ein Tipp darauf stellt die Lautstärke auf
  diese Höhe. Braucht zwei Spalten.
- **Farbe der Balken:** die Füllung des Fortschrittsbalkens und der Lautstärkeleiste: die Akzentfarbe des Bildschirms
  (Standard), Grün, Gelb, Rot, Blau, Lila oder wie der Text. Es sind die Farben des Kerns, hell und dunkel gut lesbar;
  eigene Farben erlaubt Tessera nicht.
- **Zusatzzeilen:** unter dem Interpreten das Album, das Tempo der Musik (zum Beispiel „138 BPM“) oder beides, wenn
  die Kachel hoch genug ist. Das Tempo kommt vom Tempo-Sensor, der für den Bildschirm gewählt ist (siehe „Tempo“
  unten).
- **Cover-Adresse:** die Adresse eines Bildes in Home Assistant, das statt des eigenen Bildes des Players angezeigt
  wird, zum Beispiel ein Template-Bild mit dem Cover des Players (siehe „Das Cover“ unten).

## Kachel „SCMT-Taste“

Eine Taste für denselben Media Player: zurück, Play/Pause, weiter, lauter oder leiser. Die Taste wählst du in den
Optionen der Kachel, und du kannst beliebig viele dieser Kacheln neben der Kachel „SCMT“ platzieren.

## Einrichtung

1. Das Plugin in Tessera (Plugins) zum Bildschirm hinzufügen und den Bildschirm bauen.
2. Die Kacheln im Layout platzieren.
3. Im Inspektor jeder Kachel den Media Player wählen. Jede `media_player`-Entity funktioniert, zum Beispiel die, die
   HASS.Agent aus der Windows-Mediensitzung macht, oder die Spotify-Integration.

Ausprobiert auf einem Guition ESP32-S3 mit 4-Zoll-Bildschirm (480 × 480). Andere Boards sind bisher nicht getestet;
schreib in einem Issue, wie es auf deinem läuft.

Die Tasten nutzen die Home-Assistant-Aktionen `media_player.media_play_pause`, `media_next_track`,
`media_previous_track`, `volume_up`, `volume_down`, `media_seek`, `volume_mute` und `volume_set`. Ein Player, der eine
davon nicht unterstützt, ignoriert sie.

## Tempo

Die Auswahl „Tempo“ der Option „Zusatzzeilen“ der Kachel „SCMT“ zeigt das Tempo der Musik als Zeile unter dem
Interpreten. Home Assistant kennt kein eigenes Tempo, deshalb kommt es von einem Sensor, zum Beispiel dem, den
HASS.Agent aus [BPM2SCMT](https://github.com/Schumey/BPM2SCMT) macht; das misst, was ein Windows-PC abspielt.

1. In Tessera (Plugins) den Sensor als „Tempo-Sensor“ des Plugins für den Bildschirm wählen und den Bildschirm bauen.
   Sein Zustand muss das Tempo als Zahl sein (`138`); `unknown` oder leer blendet die Zeile aus.
2. In den Optionen der Kachel unter „Zusatzzeilen“ „Tempo“ oder „Album und Tempo“ wählen.

Die Kachel zeigt das Tempo, solange der Player spielt oder pausiert ist. Nach einem neuen Titel wartet sie, bis der
Sensor das neue Tempo hat (er war einmal leer und hat wieder einen Wert), höchstens 12 Sekunden, damit sie nicht das
Tempo des vorigen Titels zeigt. Den Sensor gibt es einmal pro Bildschirm, nicht pro Kachel: Er gehört zu dem PC, der
spielt, egal welchen Player die Kachel zeigt.

## Das Cover

Standardmäßig ist das Cover das Bild, das Home Assistant selbst für den Media Player hat (sein `entity_picture`); das
Plugin holt nichts aus dem Internet.

Eine Kachel darf nur Bilder von `/api/image/serve/`, `/api/image_proxy/` oder `/local/` zeigen, und Textattribute
werden bei 48 Bytes abgeschnitten. Das Cover mancher Player (zum Beispiel das von HASS.Agent,
`/api/media_player_proxy/...`) wird deshalb abgelehnt.

Für diese Player gibt es die Option „Cover-Adresse“ zusammen mit einem Template-Bild in Home Assistant.

### Cover über ein Template-Bild

Ein Template-Bild ist eine Entität in Home Assistant, die das aktuelle Cover des Players enthält. Die Kachel zeigt es
über `/api/image_proxy/`, eine Adresse, die die Tessera-App holen darf. Dafür braucht es keine Automation, keine Datei
und keine Änderung in der `configuration.yaml`.

1. In Home Assistant **Einstellungen → Geräte & Dienste → Helfer → Helfer erstellen → Template → Bild** öffnen. Einen
   Namen vergeben, zum Beispiel „SCMT Cover“ (die Entität heißt dann `image.scmt_cover`), und als URL eintragen:

```
http://127.0.0.1{{ state_attr('media_player.your_player', 'entity_picture') }}
```

   `media_player.your_player` durch deinen Player ersetzen. `http://127.0.0.1` ist Home Assistant, so wie es sich
   selbst erreicht; die Adresse des Player-Bildes enthält ihr Token schon, Home Assistant darf sie also ohne Anmeldung
   holen.
2. In den Optionen der Kachel als „Cover-Adresse“ eintragen:

```
/api/image_proxy/image.scmt_cover?v={v}
```

   Die Entity-ID muss genau zu der des Helfers passen (sie steht in den Einstellungen des Helfers).

Die Adresse wird genau so verwendet, wie sie eingegeben ist, nur `{v}` ersetzt das Plugin durch eine Zahl, die sich
mit jedem Titel ändert, damit der Bildschirm neu fragt (Home Assistant ignoriert den Teil `?v=...`). Drei Sekunden nach
einem neuen Titel fragt er noch einmal, falls das Bild einen Moment später kam. Eine Adresse ohne `{v}` ändert sich
nie, dann zeigt der Bildschirm das erste geladene Bild, bis sich die Adresse ändert.

Zum Prüfen den Helfer in Home Assistant öffnen: Er zeigt das aktuelle Cover. Wer
`http://YOUR_HOME_ASSISTANT/api/image_proxy/image.scmt_cover` im Browser öffnet, bekommt „401: Unauthorized“. Das ist
normal, denn die Adresse braucht ein Token (`?token=...`), und das schickt die Tessera-App selbst mit.

Eine Datei im Ordner `www` (`/local/...`) funktioniert mit der aktuellen Tessera-App (0.4.88) nicht: Sie holt `/local/`
über den Supervisor, und der lehnt das ab (403), deshalb zeigt die Kachel ein graues Viereck
([MaxGramser/tessera-plugins#6](https://github.com/MaxGramser/tessera-plugins/issues/6)).

## Datenschutz

Das Plugin macht keine Netzwerkanfragen. Es liest nur, was Home Assistant über die gewählte Entity schon weiß, und ruft
bei einem Tipp auf eine Kachel die oben genannten Media-Player-Aktionen auf. Das Template-Bild für das Cover läuft in
Home Assistant, nicht im Plugin.
