# Wijzigingenlogboek

## 0.14.1 - 2026-10-28

- De pauzeweergave bedekt nu de hele tegel, ook de uiteinden van de voortgangsbalk in de hoeken.
- Het pauzeteken is groter: twee derde van de tegelhoogte in plaats van de grootte van een icoon.

## 0.14.0 - 2026-10-19

- De knoppen tonen de iconen van Tessera (vorige, afspelen, pauze, volgende, zachter, harder, luidspreker) in plaats van
  tekens als „<<” en „||”.
- De luidspreker van de volumeweergave toont volume-high, of volume-off als het gedempt is. De iconen voor een laag en
  een middelmatig volume zitten niet in de set van Tessera, dus onder 67 % toonde de luidspreker niets.
- Een te brede titel, artiest of album eindigt nu op puntjes in plaats van als lichtkrant te lopen, zoals elke tekst
  van het scherm. De tegel laat het scherm niet meer steeds tekenen zolang hij te zien is.
- De temporegel („138 BPM”) komt uit de vertalingen.
- De tegel bewaart zijn teksten in het geheugen naast de chip (PSRAM) als het bord dat heeft, niet in de chip.
- De README zegt op welk bord SCMT is geprobeerd.

## 0.13.0 - 2026-10-18

- Nieuwe optie „Kleur van de balken” voor de voortgangsbalk en de volumebalk: de accentkleur van het scherm
  (standaard, zoals eerder), groen, amber, rood, blauw, paars of zoals de tekst. Het zijn de kleuren van de kern,
  leesbaar in licht en donker; eigen kleuren staat Tessera niet toe.
- „Albumnaam” en „Tempo” zijn nu één optie, „Extra regels” (geen, album, tempo of beide), zodat de kleur erbij past
  (een tegel heeft hooguit 12 opties). Tegels met de twee schakelaars tonen hetzelfde als voorheen.

## 0.12.1 - 2026-10-18

- De voortgangsbalk en de tijd springen niet meer heen en weer wanneer een speler een positie van enkele seconden oud
  als nieuw meldt (HASS.Agent met Spotify). Zolang hetzelfde nummer doorspeelt, houdt de tegel een eigen klok bij en
  volgt een melding alleen als die meer dan 3 seconden afwijkt (spoelen); een nieuw nummer, een pauze of een start zet
  de klok opnieuw.

## 0.12.0 - 2026-10-17

- Nieuwe optie „Tempo”: het tempo van de muziek (bijvoorbeeld „138 BPM”) als regel onder de artiest. Het komt van een
  sensor die per scherm in Tessera wordt gekozen (nieuwe invoer „Temposensor”), bijvoorbeeld die welke HASS.Agent maakt
  van BPM2SCMT. Na een nieuw nummer wacht de regel tot de sensor het nieuwe tempo heeft.
- De plug-in leest die ene entiteit nu zelf (recht `read_entities`); Tessera vraagt erom bij de update.
- De README beschrijft een nieuwe weg naar de hoes voor spelers waarvan de eigen hoes wordt geweigerd: een
  sjabloonafbeelding in Home Assistant, getoond met het „Adres van de hoes” `/api/image_proxy/image.scmt_cover?v={v}`.
  Die vervangt de hoesautomatisering met een bestand onder `/local/`, dat niet werkt met Tessera 0.4.88 (403). De
  plug-in verandert daarvoor niet.

## 0.11.0 - 2026-10-17

- Nieuwe optie „Tekstgrootte”: klein (zoals vóór 0.10.3), middel (standaard, zoals nu) of groot (artiest en album een
  stap groter, een lange titel loopt in het grote lettertype).
- Een tik op de voortgangsbalk spoelt altijd; de optie „Spoelen door op de balk te tikken” vervalt.
- Dempen hoort bij de volumeknoppen, tussen zachter en harder; de optie „Dempen” vervalt. Met de volumeknoppen dempt
  ook een tik op een volumeweergave.

## 0.10.3 - 2026-10-17

- Grotere tekst: de titel in het grote lettertype van de waarde van een kaart, artiest en album in het lettertype van de
  woorden op een knop. Een lange titel wordt alleen tot de vroegere grootte kleiner en loopt daarna als lichtkrant; op
  een lage tegel wordt alles kleiner zoals voorheen.
- Reparatie: lichtkranttekst loopt niet meer over de hoes of de rand van de kaart.

## 0.10.2 - 2026-10-17

- De volumebalk staat altijd aan de rechterrand, ook als de hoes rechts staat, en is een derde smaller.
- Gelijke afstanden: hoes, tekst, voortgangsbalk en knoppen staan nu even ver van elkaar als van de rand van de kaart.

## 0.10.1 - 2026-10-17

- Reparatie: op een lage tegel (het kleine 2,8-inch-scherm) ligt de artiest niet meer over de tijd. De titel krijgt ook
  een kleiner lettertype als hij te hoog is, en staat alleen als er geen plaats is voor de artiest.
- De README zegt nu dat de optie „Adres van de hoes” en de hoesautomatisering niet werken met de huidige Tessera-app
  (0.4.88): die haalt `/local/` op via de Supervisor, en die weigert het (403), dus de tegel toont een grijs blok.
- De afbeelding voor de pluginwinkel toont het nieuwe uiterlijk.

## 0.10.0 - 2026-10-16

- Een titel, artiest of album die te breed is voor de tegel loopt als lichtkrant door in plaats van met puntjes te
  worden afgekapt.
- De volumebalk staat rechtop langs de rand van de tegel, tegenover de hoes (rechts, of links als de hoes rechts staat).
  Hij vraagt nu twee kolommen in plaats van twee rijen.
- „Spoelen door op de balk te tikken” staat standaard aan.
- Gelijke afstanden: dezelfde ruimte tussen hoes, tekst, voortgangsbalk en knoppen, en de buitenste knoppen sluiten
  gelijk met de randen.
- Staat de tekst boven of onderaan, dan sluit hij gelijk met de boven- of onderrand van de hoes.

## 0.9.1 - 2026-10-16

- Reparatie: het manifest werd geweigerd („tiles[0].options: at most 12 entries”), omdat de tegel „SCMT” 15 opties had.
  Drie paren zijn nu elk één keuze, zonder dat er iets verloren gaat:
  - „Albumhoes”: links, rechts of uit (was „Albumhoes” en „Positie van de hoes”).
  - „Knoppen”: geen, afspelen, volume of allebei (was „Afspeelknoppen” en „Volumeknoppen”).
  - „Grote weergave”: geen, volume, pauze of allebei (was „Grote volumeweergave” en „Pauzeweergave”).
- Tegels die met een oudere versie zijn geplaatst, houden hun instellingen; de tegel leest de vroegere opties nog steeds.

## 0.9.0 - 2026-10-16

- Nieuwe optie „Pauzeweergave”: zolang de speler gepauzeerd is, ligt een half doorzichtige weergave met het pauzeteken
  over de hele tegel.
- Nieuwe optie „Positie van de hoes”: de albumhoes links of rechts van de tekst.
- Nieuwe optie „Positie van de tekst”: titel, artiest en album boven, in het midden of onderaan de ruimte naast de hoes.
- Meer talen: Frans, Spaans, Italiaans en Nederlands, naast Engels en Duits. Ook de README en dit logboek bestaan in
  deze talen (`README.de.md`, `CHANGELOG.de.md`, enzovoort).
- De README beschrijft de automatisering die een albumhoes naar het scherm brengt (experimenteel), met een voorbeeld in
  `examples/cover_automation.yaml`.
- De tegel reserveert iets meer geheugen voor de nieuwe weergave.

## 0.8.1 - 2026-10-16

- Het „Adres van de hoes” wordt nu precies zo gebruikt als het is ingevoerd. Een `{v}` erin wordt vervangen door een
  getal dat bij elk nummer verandert (bijvoorbeeld `/local/cover.jpg?v={v}`); eerder werd dat getal altijd als `?v=`
  achter het adres gezet.
- De winkelafbeelding `store.png` voor de pluginwinkel is toegevoegd.

## 0.8.0 - 2026-10-15

- Nieuwe optie „Adres van de hoes”: een vast adres van een hoesbestand in Home Assistant (bijvoorbeeld
  `/local/cover.jpg`), getoond in plaats van de eigen afbeelding van de speler. Voor spelers waarvan de tegel de hoes
  niet mag laden.

## 0.7.1 - 2026-10-15

- Reparatie: de build mislukte met „Component not found: now_playing”, omdat `plugin.yaml` nog het oude component
  noemde. Daar staat nu `scmt`, net als in het manifest en de map.

## 0.7.0 - 2026-10-15

- De id van de plugin is nu `scmt` (eerder `now_playing`), net als de map en het component. De app ziet een nieuwe
  plugin: verwijder de oude van het scherm, voeg deze toe en plaats de tegels opnieuw.

## 0.6.0 - 2026-10-15

- De plugin heet SCMT (Simple Customizable Media Tile). De id, `now_playing`, blijft, dus tegels die al op een scherm
  staan, blijven werken.
- Nieuwe optie „Hoogte van de voortgangsbalk” (2 tot 16 pixels, standaard 4).

## 0.5.0 - 2026-10-15

- Nieuwe opties: „Spoelen door op de balk te tikken”, „Dempen”, „Volumebalk” (een tik stelt het volume in), „Albumnaam”
  (een derde regel).
- Nieuwe optie „Grote volumeweergave”: drie seconden na een wijziging ligt een half doorzichtige weergave over de hele
  tegel, met het getal in grote cijfers en een luidspreker erachter. Ook de kleine volumeweergave heeft een luidspreker.
- De tegel kan tot 4x4 cellen groot zijn (de editor biedt aan wat in het raster van het scherm past).
- Vraagt drie extra acties: `media_seek`, `volume_mute`, `volume_set`.

## 0.4.1 - 2026-10-14

- Reparatie: het manifest werd geweigerd omdat de keuzewaarde `off` als waarheidswaarde werd gelezen; elke keuzewaarde
  wordt nu als tekst geschreven.

## 0.4.0 - 2026-10-14

- Nieuw: de optie „Voortgang” kiest nu een balk, de tijd (1:36 / 7:36), beide of niets.
- Nieuw: de optie „Volumeweergave” toont het volume rechts van de titel: bij een wijziging (drie seconden), altijd of
  nooit. Een gedempte speler toont „Gedempt”.

## 0.3.2 - 2026-10-14

- De knoppen in de tegel hebben een lichte rand.

## 0.3.1 - 2026-10-14

- De plek voor de hoes wordt alleen getoond als de speler een afbeelding heeft die het scherm mag laden; anders neemt de
  tekst de hele breedte en blijft er geen leeg grijs vierkant over.

## 0.3.0 - 2026-10-14

- Nieuw: de albumhoes naast de titel (optie „Albumhoes”), vanaf twee cellen breed, op schermen die afbeeldingen tonen.
- Nieuw: afspeelknoppen (vorige, afspelen/pauze, volgende) en volumeknoppen (zachter, harder) in de tegel, als opties.
  Ze vragen een tegel van twee rijen of meer.
- Vraagt plugin-API 0.10.

## 0.2.0 - 2026-10-14

- Nieuw: een voortgangsbalk op de tegel (optie „Voortgangsbalk”, standaard aan), als de speler duur en positie
  doorgeeft.
- Nieuw: een tik op de tegel start of pauzeert het afspelen.
- Nieuw: de tegel „Mediaknop” (vorige, afspelen/pauze, volgende, harder, zachter), een tegel per knop.
- De tegel kan nu 1x1 zijn; eerder minstens 2x1.

## 0.1.1 - 2026-10-14

- Reparatie: de build mislukte omdat de kleuren als `tessera::theme::` waren geschreven; het is `theme::`.

## 0.1.0 - 2026-10-14

- Eerste versie: toont titel en artiest van een mediaspeler.
