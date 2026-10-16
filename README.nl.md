# SCMT - Simple Customizable Media Tile

[English](README.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Nederlands](README.nl.md)

Twee tegels voor Tessera die tonen en bedienen wat een mediaspeler in Home Assistant afspeelt.

## Tegel „SCMT”

De titel in grote letters, daaronder de artiest en onderaan een optionele voortgangsbalk.

- Zolang er iets speelt: titel en artiest. Bij pauze hetzelfde, met een gedempte titel.
- Als er niets speelt: „Er wordt niets afgespeeld”; zolang er nog geen speler is gekozen: „Kies een speler”.
- Een titel die te breed is voor de tegel krijgt eerst een kleiner lettertype en loopt daarna als lichtkrant door.
  Net zo een artiest en een album die te breed zijn.
- De optie „Voortgang” toont een balk (de hoogte is ook een optie, 2 tot 16 pixels), de tijd (gespeeld / duur), beide of
  niets. De speler moet `media_duration` en `media_position` doorgeven; anders verschijnt geen van beide.
- De optie „Volumeweergave” toont het volume rechts van de titel: drie seconden na een wijziging, altijd of nooit. Ze
  heeft `volume_level` nodig; een gedempte speler toont „Gedempt”.
- Een tik op de tegel start of pauzeert het afspelen.
- De albumhoes staat naast de titel als de tegel minstens twee cellen breed is en het scherm afbeeldingen kan tonen (de
  kleine borden kunnen dat niet). De optie „Albumhoes” zet hem links (standaard) of rechts van de tekst, of zet hem uit.
- De optie „Knoppen” voegt knoppen onder de titel toe: afspelen (vorige, afspelen/pauze, volgende), volume (zachter,
  harder) of allebei. Ze vragen een tegel van twee rijen of meer.

### Meer opties van deze tegel

- **Positie van de tekst:** titel, artiest en album in het midden (standaard), boven of onderaan naast de hoes. Boven
  of onderaan sluit de tekst gelijk met de boven- of onderrand van de hoes.
- **Grote weergave:** een half doorzichtige weergave over de hele tegel, voor het volume, voor de pauze of voor allebei.
  Standaard uit.
  - Volume: drie seconden nadat het volume is veranderd, het getal en een luidspreker. Met de optie „Dempen” dempt een
    tik erop.
  - Pauze: zolang de speler gepauzeerd is, het pauzeteken.
- **Spoelen door op de balk te tikken:** een tik op de voortgangsbalk springt naar die plek in het nummer. Standaard
  aan.
- **Dempen:** een dempknop tussen de knoppen; een tik op de volumeweergave dempt of heft het dempen op.
- **Volumebalk:** een staande balk langs de rechterrand van de tegel; een tik erop stelt het volume op die hoogte in.
  Vraagt twee kolommen.
- **Albumnaam:** het album op een derde regel, als de tegel hoog genoeg is.
- **Adres van de hoes:** een vast adres van een hoesbestand in Home Assistant, getoond in plaats van de afbeelding van
  de speler (zie „De hoes” hieronder).

## Tegel „SCMT-knop”

Een knop voor dezelfde speler: vorige, afspelen/pauze, volgende, harder of zachter. Kies de knop in de opties van de
tegel en plaats zoveel van deze tegels als je wilt, naast de tegel „SCMT”.

## Instellen

1. Voeg de plugin toe aan je scherm in Tessera (Plugins) en bouw het scherm.
2. Plaats de tegels in de indeling.
3. Kies in de inspecteur van elke tegel de speler. Elke `media_player`-entiteit werkt, bijvoorbeeld die welke
   HASS.Agent maakt van de Windows-mediasessie, of de Spotify-integratie.

De knoppen gebruiken de Home Assistant-acties `media_player.media_play_pause`, `media_next_track`,
`media_previous_track`, `volume_up`, `volume_down`, `media_seek`, `volume_mute` en `volume_set`. Een speler die er een
niet ondersteunt, negeert die.

## De hoes

Standaard is de hoes de afbeelding die Home Assistant zelf voor de speler heeft (zijn `entity_picture`); de plugin haalt
niets van internet.

Een tegel mag alleen afbeeldingen van `/api/image/serve/`, `/api/image_proxy/` of `/local/` tonen, en tekstattributen
worden bij 48 bytes afgekapt. De hoes van sommige spelers (bijvoorbeeld die van HASS.Agent,
`/api/media_player_proxy/...`) wordt daarom geweigerd.

**Let op:** de optie „Adres van de hoes” samen met de automatisering hieronder was voor deze spelers bedoeld, maar werkt
niet met de huidige Tessera-app (0.4.88). De app haalt een adres onder `/local/` op via de Supervisor
(`http://supervisor/core/local/...`), en die weigert het (403, in het log van de app „no picture from /local/cover.jpg”),
dus de tegel toont een grijs blok in plaats van de hoes, ongeacht het formaat van het bestand. Beide blijven in de
plugin tot Tessera `/local/` op een andere manier ophaalt of de eigen hoes van de speler toestaat
([MaxGramser/tessera-plugins#6](https://github.com/MaxGramser/tessera-plugins/issues/6)).

### Hoesautomatisering (experimenteel)

De automatisering slaat de huidige hoes van de speler op als bestand in de map `www` van Home Assistant. De tegel toont
dan dat bestand. De kant van Home Assistant werkt; of je scherm het bestand toont, hangt af van je Tessera-versie, dus
het is experimenteel. Dezelfde code staat in `examples/cover_automation.yaml`.

1. Maak de map `/config/www/` aan als die ontbreekt en start Home Assistant één keer opnieuw.
2. Voeg het commando toe aan je `configuration.yaml` (laad daarna de shell-commando's opnieuw of herstart). Er staat
   bewust geen `&&` in: Home Assistant voert commando's met `{{ }}` zonder shell uit.

```yaml
shell_command:
  cover_fetch: >-
    curl -sSfkL --connect-timeout 5 --max-time 20 -o /config/www/cover.jpg "{{ url }}"
```

3. Maak de automatisering. Vervang `media_player.your_player` door je speler en `YOUR_HOME_ASSISTANT` door het adres
   waarmee je Home Assistant opent, zoals de computer van Home Assistant het ziet (bijvoorbeeld
   `http://192.168.0.10:8123`, of `https://ha.example.com`, de optie `-k` staat al in het commando). Ze reageert op een
   nieuwe titel, wacht twee seconden en haalt de hoes driemaal op (na 2, 7 en 15 seconden), omdat de nieuwe hoes vaak
   enkele seconden na de titel aankomt.

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

4. Voer in de opties van de tegel `/local/cover.jpg?v={v}` in als „Adres van de hoes”.

Het adres wordt precies zo gebruikt als het is ingevoerd, behalve `{v}`: de plugin vervangt dat door een getal dat bij
elk nummer verandert, zodat het scherm opnieuw vraagt (Home Assistant negeert het deel `?v=...` en stuurt het bestand).
Drie seconden na een nieuw nummer vraagt het nog een keer, voor het geval het bestand iets te laat is geschreven. Een
adres zonder `{v}` verandert nooit, dus het scherm toont dan de eerste geladen afbeelding totdat het adres verandert.

Open om het bestand te controleren `http://YOUR_HOME_ASSISTANT/local/cover.jpg?x=1` in een privévenster van de browser
(verander het getal elke keer, want de browser onthoudt de afbeelding).

## Privacy

De plugin doet geen netwerkverzoeken. Ze leest alleen wat Home Assistant al weet over de gekozen entiteit en roept de
hierboven genoemde mediaspeleracties aan als je op een tegel tikt. De hoesautomatisering draait in Home Assistant, niet
in de plugin.
