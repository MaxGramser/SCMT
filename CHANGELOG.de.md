# Änderungsprotokoll

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
