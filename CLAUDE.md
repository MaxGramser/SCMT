# SCMT – Projektnotizen für Claude

SCMT (Simple Customizable Media Tile) ist ein Plugin für Tessera (Home-Assistant-App „Tessera Screen Manager“, Slug
`ec8ae0ed_esp_screen_manager`): eine Info-Kachel (`now_playing`) und eine Button-Kachel (`media_button`) für einen
`media_player`. Repo: https://github.com/Schumey/SCMT. Manifest: `tessera-plugin.yaml`, Komponente: `components/scmt/`.

Doku gibt es in sechs Sprachen (README/CHANGELOG `.md`, `.de`, `.fr`, `.es`, `.it`, `.nl`): Änderungen in allen
nachziehen, ebenso die Texte in `translations/*.json`.

## Aktueller Stand (2026-10-17)

- Version 0.10.1 ist veröffentlicht (Commit `697fa83…`) und läuft auf dem Bildschirm „Moin!“ (`dsp-desktop`, Guition
  ESP32-S3). Build und OTA haben geklappt, SCMT steht in `/config/esphome/dsp-desktop.plugins.yaml`.
- SCMT ist in Tessera per **Add with a link** eingebunden (Quelle `link`) und steht **nicht** im Plugin-Index von
  MaxGramser/tessera-plugins. Das ist der Grund für das Problem unten.

## Bekanntes Problem: SCMT fehlt nach einem Update in der plugins.yaml

Ursache liegt in Tessera, nicht in SCMT (gefunden durch Lesen von `screen_manager/app/plugins.py` in
MaxGramser/homeassistant_espscreen, Stand `main`):

- `Plugins.apply()` schreibt die plugins.yaml (`self.sidecar(...)`, ca. Z. 1209), **bevor** der Snapshot der neuen
  Version gespeichert wird (`keep_snapshot`, ca. Z. 1215).
- `entry_for()` (ca. Z. 394–403) sucht für Quelle `link` nur im Snapshot und im Index, nicht in `self.links`. Für eine
  neue Version eines Plugins, das nicht im Index steht, gibt es `None` zurück, und `sidecar()` lässt das Plugin
  stillschweigend weg.
- Folge: Der Build läuft ohne SCMT, meldet Erfolg, der Datensatz wird `active`, die Kachel zeigt „Plugin missing“. Ein
  zweiter Build derselben Version klappt, weil der Snapshot dann existiert.

Weitere Fakten aus dem Tessera-Code, falls das Thema wiederkommt:
- Schlägt der **erste** Build eines Plugins fehl, wird es auf `failed` gesetzt und aus der plugins.yaml genommen;
  schlägt ein **Update** fehl, geht Tessera auf die vorige Version zurück.
- Das komplette Build-Log steht im Tab „Firmware & USB“, bis der nächste Build startet. Das Log der App (Supervisor)
  enthält keine Compiler-Ausgabe.
- Das Build-Log vom 2026-10-17 08:22 (Update-Build) war erfolgreich (Compile + OTA); nur Warnungen aus dem
  Tessera-Kern, keine aus SCMT.

## Entscheidungen

- Am SCMT-Code wird für dieses Problem nichts geändert; der Fehler wird bei Tessera gemeldet.
- Cover für Player, deren eigenes Cover abgelehnt wird (HASS.Agent, `/api/media_player_proxy/`): über einen
  Template-Bild-Helfer in Home Assistant (`image.scmt_cover`, URL
  `http://127.0.0.1{{ state_attr('media_player.desktop_ljbgp13', 'entity_picture') }}`) und die „Cover address“
  `/api/image_proxy/image.scmt_cover?v={v}`. Am 2026-10-17 am Display getestet, keine Code-Änderung nötig. Die
  Cover-Automation über `/local/` ist aus README und Repo entfernt: Tessera 0.4.88 holt `/local/` über den Supervisor
  und bekommt 403 (MaxGramser/tessera-plugins#6).

- Tempo (0.12.0): Eine Kachel bekommt nur ihre eine Entität. Deshalb liest `plugin.yaml` den BPM-Sensor als
  `homeassistant`-Sensor (Eingabe `bpm` pro Bildschirm, `read_entities: ["{bpm}"]`), und die Kachel-Option schaltet
  nur die Zeile unter dem Interpreten ein. Sensor beim Nutzer: `sensor.wohnzimmer_heimrechner_bpm` (HASS.Agent liest
  ihn aus BPM2SCMT, Repo Schumey/BPM2SCMT).
- Balkenfarbe (0.13.0): Option `bar_color` für Fortschritts- und Lautstärkebalken, feste Palette (Akzent, Tones
  GOOD/BUSY/ALERT, `theme::MARK_BLUE`/`MARK_PURPLE`, `theme::INK`). Freie Farben gehen nicht: `tools/check.py` von
  tessera-plugins verbietet Farben als Zahl. Platz dafür: die Toggles `album` und `bpm` sind zur Auswahl `album`
  (`none`/`album`/`bpm`/`both`) zusammengelegt; alte Kacheln mit Bool-Werten werden weiter gelesen. `now_playing` hat
  weiter 12 Optionen, das Maximum: Für eine neue Option muss wieder etwas zusammengelegt werden.

- Plugin-API-Regeln (0.14.0, nach Max' Review von MaxGramser/tessera-plugins#12): nur Icons aus Tesseras Set
  (Liste in `packages/core.yaml` von homeassistant_espscreen; volume-low/-medium fehlen, volume-plus/-minus sind da),
  keine Wörter oder ASCII-Zeichen als Tasten im Code, Texte in Membern als `tessera::Text`, Labels mit Punkten statt
  Laufschrift (`ui::label`).

## Offene Punkte

- Der plugins.yaml-Fehler ist laut Max auf Tesseras dev behoben (homeassistant_espscreen #260) und kommt mit dem
  nächsten Release. Das Issue muss nicht mehr eingereicht werden. Bis zum Release: nach jedem SCMT-Update prüfen, ob
  SCMT in `dsp-desktop.plugins.yaml` steht, sonst einmal neu bauen.
- PR MaxGramser/tessera-plugins#12 (SCMT in `community/`): Max merged, sobald ein Release seine fünf Punkte erfüllt
  (in 0.14.0 umgesetzt). Max gefragt, ob volume-low/-medium ins Icon-Set kommen.
- `cover:<entity>` (Plugin-API 0.17, Tessera dev, geprüft 2026-10-23) könnte `image.scmt_cover` ersetzen: Die App holt
  das Cover über `/api/media_player_proxy/<entity>` mit ihrem Token, wie die eigene Player-Kachel. Aber: Die Firmware
  bildet den Bild-Key einer Plugin-Kachel nur aus Platz und Adresse (`tile_picture::set_parts`), und `cover:` nimmt
  keine Parameter (`picture_question.source`): Bei neuem Titel bleibt wohl das alte Cover stehen. Vor dem Umbau Max
  fragen. API 0.17 geht erst ab dem nächsten Tessera-Release (0.4.90 bietet 0.14).
