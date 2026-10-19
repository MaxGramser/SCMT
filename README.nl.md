# SCMT - Simple Customizable Media Tile

[English](README.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Nederlands](README.nl.md)

Twee tegels voor Tessera die tonen en bedienen wat een mediaspeler in Home Assistant afspeelt.

## Tegel „SCMT”

De titel in grote letters, daaronder de artiest en onderaan een optionele voortgangsbalk.

- Zolang er iets speelt: titel en artiest. Bij pauze hetzelfde, met een gedempte titel.
- Als er niets speelt: „Er wordt niets afgespeeld”; zolang er nog geen speler is gekozen: „Kies een speler”.
- Een titel die te breed is voor de tegel krijgt eerst een kleiner lettertype en eindigt daarna op puntjes, zoals
  elke tekst van het scherm. Net zo een artiest en een album die te breed zijn.
- De optie „Voortgang” toont een balk (de hoogte is ook een optie, 2 tot 16 pixels), de tijd (gespeeld / duur), beide of
  niets. De speler moet `media_duration` en `media_position` doorgeven; anders verschijnt geen van beide.
- De optie „Volumeweergave” toont het volume rechts van de titel: drie seconden na een wijziging, altijd of nooit. Ze
  heeft `volume_level` nodig; een gedempte speler toont „Gedempt”.
- Een tik op de tegel start of pauzeert het afspelen.
- De albumhoes staat naast de titel als de tegel minstens twee cellen breed is en het scherm afbeeldingen kan tonen (de
  kleine borden kunnen dat niet). De optie „Albumhoes” zet hem links (standaard) of rechts van de tekst, of zet hem uit.
- De optie „Knoppen” voegt knoppen onder de titel toe: afspelen (vorige, afspelen/pauze, volgende), volume (zachter,
  dempen, harder) of allebei. Ze vragen een tegel van twee rijen of meer. Met de volumeknoppen dempt ook een tik op een
  volumeweergave.
- Een tik op de voortgangsbalk springt naar die plek in het nummer.

### Meer opties van deze tegel

- **Positie van de tekst:** titel, artiest en album in het midden (standaard), boven of onderaan naast de hoes. Boven
  of onderaan sluit de tekst gelijk met de boven- of onderrand van de hoes.
- **Grote weergave:** een half doorzichtige weergave over de hele tegel, voor het volume, voor de pauze of voor allebei.
  Standaard uit.
  - Volume: drie seconden nadat het volume is veranderd, het getal en een luidspreker. Met de volumeknoppen dempt een
    tik erop.
  - Pauze: zolang de speler gepauzeerd is, het pauzeteken.
- **Tekstgrootte:** klein, middel (standaard) of groot, voor titel, artiest en album.
- **Volumebalk:** een staande balk langs de rechterrand van de tegel; een tik erop stelt het volume op die hoogte in.
  Vraagt twee kolommen.
- **Kleur van de balken:** de vulling van de voortgangsbalk en de volumebalk: de accentkleur van het scherm
  (standaard), groen, amber, rood, blauw, paars of zoals de tekst. Het zijn de kleuren van de kern, leesbaar in licht en
  donker; eigen kleuren staat Tessera niet toe.
- **Extra regels:** onder de artiest het album, het tempo van de muziek (bijvoorbeeld „138 BPM”) of beide, als de
  tegel hoog genoeg is. Het tempo komt van de temposensor die voor het scherm is gekozen (zie „Tempo” hieronder).
- **Adres van de hoes:** het adres van een afbeelding in Home Assistant, getoond in plaats van de afbeelding van de
  speler, bijvoorbeeld een sjabloonafbeelding met de hoes van de speler (zie „De hoes” hieronder).

## Tegel „SCMT-knop”

Een knop voor dezelfde speler: vorige, afspelen/pauze, volgende, harder of zachter. Kies de knop in de opties van de
tegel en plaats zoveel van deze tegels als je wilt, naast de tegel „SCMT”.

## Instellen

1. Voeg de plugin toe aan je scherm in Tessera (Plugins) en bouw het scherm.
2. Plaats de tegels in de indeling.
3. Kies in de inspecteur van elke tegel de speler. Elke `media_player`-entiteit werkt, bijvoorbeeld die welke
   HASS.Agent maakt van de Windows-mediasessie, of de Spotify-integratie.

Geprobeerd op een Guition ESP32-S3 met een scherm van 4 inch (480 × 480). Andere borden zijn nog niet getest; laat
in een issue weten hoe het op het jouwe werkt.

De knoppen gebruiken de Home Assistant-acties `media_player.media_play_pause`, `media_next_track`,
`media_previous_track`, `volume_up`, `volume_down`, `media_seek`, `volume_mute` en `volume_set`. Een speler die er een
niet ondersteunt, negeert die.

## Tempo

De keuze „Tempo” van de optie „Extra regels” van de tegel „SCMT” toont het tempo van de muziek als regel onder de
artiest. Home Assistant kent zelf geen tempo, dus het komt van een sensor, bijvoorbeeld die welke HASS.Agent maakt van
[BPM2SCMT](https://github.com/Schumey/BPM2SCMT), dat meet wat een Windows-pc afspeelt.

1. Kies in Tessera (Plugins) de sensor als „Temposensor” van de plug-in voor het scherm en bouw het scherm. Zijn status
   moet het tempo als getal zijn (`138`); `unknown` of leeg verbergt de regel.
2. Kies in de opties van de tegel „Tempo” of „Album en tempo” onder „Extra regels”.

De tegel toont het tempo zolang de speler speelt of gepauzeerd is. Na een nieuw nummer wacht hij tot de sensor het
nieuwe tempo heeft (hij was een keer leeg en heeft weer een waarde), hooguit 12 seconden, zodat hij niet het tempo van
het vorige nummer toont. De sensor is er één per scherm, niet per tegel: hij hoort bij de pc die speelt, welke speler de
tegel ook toont.

## De hoes

Standaard is de hoes de afbeelding die Home Assistant zelf voor de speler heeft (zijn `entity_picture`); de plugin haalt
niets van internet.

Een tegel mag alleen afbeeldingen van `/api/image/serve/`, `/api/image_proxy/` of `/local/` tonen, en tekstattributen
worden bij 48 bytes afgekapt. De hoes van sommige spelers (bijvoorbeeld die van HASS.Agent,
`/api/media_player_proxy/...`) wordt daarom geweigerd.

Voor deze spelers is er de optie „Adres van de hoes” samen met een sjabloonafbeelding (template) in Home Assistant.

### De hoes via een sjabloonafbeelding

Een sjabloonafbeelding is een entiteit in Home Assistant die de huidige hoes van de speler bevat. De tegel toont die via
`/api/image_proxy/`, een adres dat de Tessera-app mag ophalen. Er is geen automatisering, geen bestand en geen wijziging
in `configuration.yaml` nodig.

1. Open in Home Assistant **Instellingen → Apparaten & diensten → Helpers → Helper aanmaken → Sjabloon → Afbeelding**.
   Geef hem een naam, bijvoorbeeld „SCMT Cover” (de entiteit heet dan `image.scmt_cover`), en vul als URL in:

```
http://127.0.0.1{{ state_attr('media_player.your_player', 'entity_picture') }}
```

   Vervang `media_player.your_player` door je speler. `http://127.0.0.1` is Home Assistant zoals het zichzelf
   bereikt; het adres van de afbeelding van de speler bevat zijn token al, dus Home Assistant mag het zonder inloggen
   ophalen.
2. Vul in de opties van de tegel als „Adres van de hoes” in:

```
/api/image_proxy/image.scmt_cover?v={v}
```

   De entiteit-ID moet precies overeenkomen met die van de helper (die staat in de instellingen van de helper).

Het adres wordt precies zo gebruikt als het is ingevuld, behalve `{v}`: de plugin vervangt dat door een getal dat bij
elk nummer verandert, zodat het scherm opnieuw vraagt (Home Assistant negeert het deel `?v=...`). Drie seconden na een
nieuw nummer vraagt het nog een keer, voor het geval de afbeelding iets later kwam. Een adres zonder `{v}` verandert
nooit, dus het scherm toont de eerste geladen afbeelding tot het adres verandert.

Open om te controleren de helper in Home Assistant: die toont de huidige hoes. Wie
`http://YOUR_HOME_ASSISTANT/api/image_proxy/image.scmt_cover` in een browser opent, krijgt „401: Unauthorized”; dat is
normaal, want het adres vraagt een token (`?token=...`), dat de Tessera-app zelf meestuurt.

Een bestand in de map `www` (`/local/...`) werkt niet met de huidige Tessera-app (0.4.88): die haalt `/local/` op via de
Supervisor, en die weigert het (403), dus de tegel toont een grijs blok
([MaxGramser/tessera-plugins#6](https://github.com/MaxGramser/tessera-plugins/issues/6)).

## Privacy

De plugin doet geen netwerkverzoeken. Ze leest alleen wat Home Assistant al weet over de gekozen entiteit en roept de
hierboven genoemde mediaspeleracties aan als je op een tegel tikt. De sjabloonafbeelding van de hoes draait in Home
Assistant, niet in de plugin.
