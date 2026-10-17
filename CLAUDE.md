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
- „Cover address“ und die Cover-Automation bleiben im Plugin, obwohl Tessera 0.4.88 `/local/` über den Supervisor
  holt und 403 bekommt (siehe README und MaxGramser/tessera-plugins#6).

## Offene Punkte

- Issue bei MaxGramser/homeassistant_espscreen ist entworfen, aber **noch nicht eingereicht** (Text liegt im Chat
  vom 2026-10-17; Vorschlag: in `entry_for()` für `link` auf `self.links` zurückfallen oder `keep_snapshot` vor
  `sidecar` aufrufen).
- Nicht verifiziert: ob im 08:22-Log `Building CXX object …/scmt/scmt.cpp.obj` fehlt (wäre ein Beleg fürs Issue).
- Analyse beruht auf `main`, nicht exakt auf App 0.4.88, und wurde nicht nachgestellt.
- Cover über `/local/` funktioniert nicht (403), wartet auf Tessera (tessera-plugins#6).

## Nächste Schritte

1. Im vollständigen Build-Log nach `scmt` suchen und das Ergebnis im Issue ergänzen.
2. Issue bei MaxGramser/homeassistant_espscreen einreichen.
3. SCMT in den Plugin-Index aufnehmen lassen (PR mit einer dreizeiligen Datei in `community/` bei
   MaxGramser/tessera-plugins, siehe dort `docs/PUBLISHING.md`). Damit findet Tessera neue Versionen über den Index
   und das Problem tritt nicht mehr auf.
4. Bis dahin: nach jedem SCMT-Update prüfen, ob SCMT in `dsp-desktop.plugins.yaml` steht, sonst einmal neu bauen.
