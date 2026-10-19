# Änderungsprotokoll

## 0.14.0 - 2026-10-19

- Die Tasten zeigen Tesseras Icons (zurück, Play, Pause, weiter, leiser, lauter, Lautsprecher) statt Zeichen wie
  „<<“ und „||“.
- Der Lautsprecher der Lautstärkeanzeige zeigt volume-high, oder volume-off bei stumm. Die Icons für leise und mittlere
  Lautstärke sind nicht in Tesseras Set, darum zeigte der Lautsprecher unter 67 % nichts.
- Ein zu breiter Titel, Interpret oder Albumname endet jetzt mit Punkten statt als Laufschrift zu laufen, wie jede
  Beschriftung des Bildschirms. Die Kachel lässt den Bildschirm nicht mehr dauernd zeichnen, solange sie zu sehen ist.
- Die Tempozeile („138 BPM“) kommt aus den Übersetzungen.
- Die Kachel hält ihre Texte im Speicher neben dem Chip (PSRAM), wo das Board ihn hat, nicht im Chip.
- Die README sagt, auf welchem Board SCMT ausprobiert ist.

## 0.13.0 - 2026-10-18

- Neue Option „Farbe der Balken“ für den Fortschrittsbalken und die Lautstärkeleiste: die Akzentfarbe des Bildschirms
  (Standard, wie bisher), Grün, Gelb, Rot, Blau, Lila oder wie der Text. Es sind die Farben des Kerns, hell und dunkel
  gut lesbar; eigene Farben erlaubt Tessera nicht.
- „Albumname“ und „Tempo“ sind jetzt eine Option, „Zusatzzeilen“ (keine, Album, Tempo oder beides), damit die Farbe
  Platz hat (eine Kachel hat höchstens 12 Optionen). Kacheln mit den beiden Schaltern zeigen weiter dasselbe.

## 0.12.1 - 2026-10-18

- Fortschrittsleiste und Zeit springen nicht mehr hin und her, wenn ein Player eine einige Sekunden alte Position als
  neu meldet (HASS.Agent mit Spotify). Solange derselbe Titel weiterläuft, führt die Kachel eine eigene Uhr und folgt
  einer Meldung nur, wenn sie um mehr als 3 Sekunden abweicht (Spulen); ein neuer Titel, Pause oder Start stellt die Uhr
  neu.

## 0.12.0 - 2026-10-17

- Neue Option „Tempo“: das Tempo der Musik (zum Beispiel „138 BPM“) als Zeile unter dem Interpreten. Es kommt von einem
  Sensor, der in Tessera pro Bildschirm gewählt wird (neue Eingabe „Tempo-Sensor“), zum Beispiel dem, den HASS.Agent
  aus BPM2SCMT macht. Nach einem neuen Titel wartet die Zeile, bis der Sensor das neue Tempo hat.
- Das Plugin liest diese eine Entität jetzt selbst (Berechtigung `read_entities`); Tessera fragt beim Update danach.
- Die README beschreibt einen neuen Weg zum Cover für Player, deren eigenes Cover abgelehnt wird: ein Template-Bild in
  Home Assistant, angezeigt mit der „Cover-Adresse“ `/api/image_proxy/image.scmt_cover?v={v}`. Er ersetzt die
  Cover-Automation mit einer Datei unter `/local/`, die mit Tessera 0.4.88 nicht funktioniert (403). Am Plugin ändert
  sich dafür nichts.

## 0.11.0 - 2026-10-17

- Neue Option „Schriftgröße“: klein (wie vor 0.10.3), mittel (Standard, wie jetzt) oder groß (Interpret und Album eine
  Stufe größer, ein langer Titel läuft in der großen Schrift).
- Ein Tipp auf den Fortschrittsbalken spult immer; die Option „Spulen per Tipp auf den Balken“ entfällt.
- Stumm gehört zu den Lautstärke-Tasten, zwischen Leiser und Lauter; die Option „Stumm“ entfällt. Mit den
  Lautstärke-Tasten schaltet auch ein Tipp auf eine Lautstärkeanzeige stumm.

## 0.10.3 - 2026-10-17

- Größerer Text: der Titel in der großen Schrift eines Kartenwerts, Interpret und Album in der Schrift einer
  Tastenbeschriftung. Ein langer Titel wird nur bis zur bisherigen Größe kleiner und läuft dann als Laufschrift; auf
  einer niedrigen Kachel wird wie bisher alles kleiner.
- Fehlerbehebung: Laufschrift läuft nicht mehr über das Cover oder den Rand der Karte.

## 0.10.2 - 2026-10-17

- Die Lautstärkeleiste steht immer am rechten Rand, auch wenn das Cover rechts ist, und ist ein Drittel schmaler.
- Gleichmäßige Abstände: Cover, Text, Fortschrittsbalken und Tasten stehen jetzt so weit voneinander entfernt wie vom
  Rand der Karte.

## 0.10.1 - 2026-10-17

- Fehlerbehebung: Auf einer niedrigen Kachel (dem kleinen 2,8-Zoll-Bildschirm) liegt der Interpret nicht mehr über der
  Zeit. Der Titel bekommt auch dann eine kleinere Schrift, wenn er zu hoch ist, und steht allein, wenn für den Interpreten
  kein Platz ist.
- Die README sagt jetzt, dass die Option „Cover-Adresse“ und die Cover-Automation mit der aktuellen Tessera-App nicht
  funktionieren (0.4.88): Sie holt `/local/` über den Supervisor, der das ablehnt (403), deshalb zeigt die Kachel ein
  graues Viereck.
- Das Bild für den Plugin-Store zeigt das neue Aussehen.

## 0.10.0 - 2026-10-16

- Ein Titel, Interpret oder Album, der zu breit für die Kachel ist, läuft als Laufschrift durch, statt mit Punkten
  gekürzt zu werden.
- Die Lautstärkeleiste steht aufrecht am Rand der Kachel, gegenüber dem Cover (rechts, oder links, wenn das Cover rechts
  steht). Sie braucht jetzt zwei Spalten statt zwei Zeilen.
- „Spulen per Tipp auf den Balken“ ist standardmäßig an.
- Gleichmäßige Abstände: derselbe Abstand zwischen Cover, Text, Fortschrittsbalken und Tasten, und die äußeren Tasten
  schließen bündig mit den Rändern ab.
- Steht der Text oben oder unten, schließt er bündig mit der Ober- oder Unterkante des Covers ab.

## 0.9.1 - 2026-10-16

- Fehlerbehebung: Das Manifest wurde abgelehnt („tiles[0].options: at most 12 entries“), weil die Kachel „SCMT“ 15
  Optionen hatte. Drei Paare sind jetzt je eine Auswahl, ohne dass etwas verloren geht:
  - „Albumcover“: links, rechts oder aus (war „Albumcover“ und „Position des Covers“).
  - „Tasten“: keine, Wiedergabe, Lautstärke oder beides (war „Wiedergabe-Tasten“ und „Lautstärke-Tasten“).
  - „Große Anzeige“: keine, Lautstärke, Pause oder beides (war „Große Lautstärkeanzeige“ und „Pause-Anzeige“).
- Kacheln, die mit einer älteren Version platziert wurden, behalten ihre Einstellungen; die Kachel liest die früheren
  Optionen weiterhin.

## 0.9.0 - 2026-10-16

- Neue Option „Pause-Anzeige“: Solange der Player pausiert ist, liegt eine halb durchsichtige Anzeige mit dem
  Pause-Zeichen über der ganzen Kachel.
- Neue Option „Position des Covers“: das Albumcover links oder rechts vom Text.
- Neue Option „Position des Textes“: Titel, Interpret und Album oben, in der Mitte oder unten in dem Platz neben dem
  Cover.
- Mehr Sprachen: Französisch, Spanisch, Italienisch und Niederländisch, neben Englisch und Deutsch. Auch die README und
  dieses Änderungsprotokoll gibt es in diesen Sprachen (`README.de.md`, `CHANGELOG.de.md` und so weiter).
- Die README beschreibt die Automation, die ein Albumcover auf das Display bringt (experimentell), mit einem Beispiel in
  `examples/cover_automation.yaml`.
- Die Kachel reserviert etwas mehr Speicher für die neue Anzeige.

## 0.8.1 - 2026-10-16

- Die „Cover-Adresse“ wird jetzt genau so verwendet, wie sie eingegeben wird. Ein `{v}` darin wird durch eine Zahl
  ersetzt, die sich mit jedem Titel ändert (zum Beispiel `/local/cover.jpg?v={v}`); vorher wurde diese Zahl immer als
  `?v=` hinter die Adresse gehängt.
- Das Store-Bild `store.png` für den Plugin-Store wurde hinzugefügt.

## 0.8.0 - 2026-10-15

- Neue Option „Cover-Adresse“: eine feste Adresse einer Cover-Datei in Home Assistant (zum Beispiel `/local/cover.jpg`),
  die statt des eigenen Bildes des Players angezeigt wird. Für Player, deren Cover die Kachel nicht laden darf.

## 0.7.1 - 2026-10-15

- Fehlerbehebung: Der Build scheiterte mit „Component not found: now_playing“, weil `plugin.yaml` noch die alte
  Komponente nannte. Dort steht jetzt `scmt`, wie im Manifest und im Ordnernamen.

## 0.7.0 - 2026-10-15

- Die ID des Plugins ist jetzt `scmt` (vorher `now_playing`), wie ihr Ordner und ihre Komponente. Die App sieht ein
  neues Plugin: Das alte vom Bildschirm entfernen, dieses hinzufügen und seine Kacheln neu platzieren.

## 0.6.0 - 2026-10-15

- Das Plugin heißt SCMT (Simple Customizable Media Tile). Seine ID `now_playing` bleibt, Kacheln, die schon auf einem
  Bildschirm liegen, funktionieren also weiter.
- Neue Option „Höhe des Fortschrittsbalkens“ (2 bis 16 Pixel, standardmäßig 4).

## 0.5.0 - 2026-10-15

- Neue Optionen: „Spulen per Tipp auf den Balken“, „Stumm“, „Lautstärkeleiste“ (ein Tipp stellt die Lautstärke ein),
  „Albumname“ (eine dritte Zeile).
- Neue Option „Große Lautstärkeanzeige“: drei Sekunden nach einer Änderung liegt eine halb durchsichtige Anzeige über
  der ganzen Kachel, mit der Zahl in großen Ziffern und einem Lautsprecher dahinter. Auch die kleine Lautstärkeanzeige
  hat einen Lautsprecher.
- Die Kachel kann bis zu 4x4 Zellen groß sein (der Editor bietet an, was in das Raster des Bildschirms passt).
- Braucht drei weitere Aktionen: `media_seek`, `volume_mute`, `volume_set`.

## 0.4.1 - 2026-10-14

- Fehlerbehebung: Das Manifest wurde abgelehnt, weil der Auswahlwert `off` als Wahrheitswert gelesen wurde; jeder
  Auswahlwert ist jetzt als Text geschrieben.

## 0.4.0 - 2026-10-14

- Neu: Die Option „Fortschritt“ wählt jetzt einen Balken, die Zeit (1:36 / 7:36), beides oder nichts.
- Neu: Die Option „Lautstärkeanzeige“ zeigt die Lautstärke rechts neben dem Titel: bei Änderung (drei Sekunden), immer
  oder nie. Ein stummer Player zeigt „Stumm“.

## 0.3.2 - 2026-10-14

- Die Tasten in der Kachel haben einen hellen Rahmen.

## 0.3.1 - 2026-10-14

- Der Platz für das Cover wird nur angezeigt, wenn der Player ein Bild hat, das der Bildschirm laden darf; sonst nimmt
  der Text die ganze Breite, und es bleibt kein leeres graues Quadrat.

## 0.3.0 - 2026-10-14

- Neu: das Albumcover neben dem Titel (Option „Albumcover“), ab zwei Zellen Breite, auf Bildschirmen, die Bilder zeigen.
- Neu: Wiedergabe-Tasten (zurück, Play/Pause, weiter) und Lautstärke-Tasten (leiser, lauter) in der Kachel, als
  Optionen. Sie brauchen eine Kachel mit zwei Zeilen oder mehr.
- Braucht die Plugin-API 0.10.

## 0.2.0 - 2026-10-14

- Neu: ein Fortschrittsbalken in der Kachel (Option „Fortschrittsbalken“, standardmäßig an), wenn der Player Dauer und
  Position meldet.
- Neu: Ein Tipp auf die Kachel startet oder pausiert die Wiedergabe.
- Neu: die Kachel „Media-Taste“ (zurück, Play/Pause, weiter, lauter, leiser), eine Kachel pro Taste.
- Die Kachel kann jetzt 1x1 groß sein; vorher mindestens 2x1.

## 0.1.1 - 2026-10-14

- Fehlerbehebung: Der Build scheiterte, weil die Farben als `tessera::theme::` geschrieben waren; es heißt `theme::`.

## 0.1.0 - 2026-10-14

- Erste Version: zeigt Titel und Interpret eines Media Players an.
