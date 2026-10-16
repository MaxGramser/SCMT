# Wijzigingenlogboek

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

## 0.8.1.1 - 2026-10-16

- De winkelafbeelding `store.png` voor de pluginwinkel is toegevoegd.

## 0.8.1 - 2026-10-16

- Het „Adres van de hoes” wordt nu precies zo gebruikt als het is ingevoerd. Een `{v}` erin wordt vervangen door een
  getal dat bij elk nummer verandert (bijvoorbeeld `/local/cover.jpg?v={v}`); eerder werd dat getal altijd als `?v=`
  achter het adres gezet.

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
