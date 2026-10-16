# SCMT - Simple Customizable Media Tile

[English](README.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Nederlands](README.nl.md)

Due tessere per Tessera che mostrano e controllano ciò che riproduce un lettore multimediale di Home Assistant.

## Tessera «SCMT»

Il titolo in caratteri grandi, sotto l'artista e in basso una barra di avanzamento facoltativa.

- Mentre qualcosa suona: titolo e artista. In pausa: lo stesso, con il titolo attenuato.
- Quando non suona nulla: «Nessuna riproduzione»; finché non è scelto un lettore: «Scegli un lettore».
- Un titolo troppo largo per la tessera riceve prima un carattere più piccolo, poi viene tagliato con dei puntini.
- L'opzione «Avanzamento» mostra una barra (anche la sua altezza è un'opzione, da 2 a 16 pixel), il tempo (trascorso /
  durata), entrambi o niente. Il lettore deve indicare `media_duration` e `media_position`; altrimenti non compare né
  l'una né l'altro.
- L'opzione «Indicatore del volume» mostra il volume a destra del titolo: per tre secondi dopo una modifica, sempre o
  mai. Serve `volume_level`; un lettore in muto mostra «Muto».
- Un tocco sulla tessera avvia o mette in pausa la riproduzione.
- La copertina dell'album sta accanto al titolo quando la tessera è larga almeno due celle e lo schermo può mostrare
  immagini (le schede piccole non possono). L'opzione «Copertina dell'album» la mette a sinistra (predefinito) o a
  destra del testo, oppure la disattiva.
- L'opzione «Tasti» aggiunge tasti sotto il titolo: riproduzione (precedente, riproduci/pausa, successivo), volume (più
  piano, più forte) o entrambi. Servono una tessera di almeno due righe.

### Altre opzioni di questa tessera

- **Posizione del testo:** titolo, artista e album al centro (predefinito), in alto o in basso nello spazio accanto alla
  copertina.
- **Indicatore grande:** un indicatore semitrasparente su tutta la tessera, per il volume, per la pausa o per entrambi.
  Disattivato per impostazione predefinita.
  - Volume: per tre secondi dopo una modifica del volume, il numero e un altoparlante. Con l'opzione «Muto», un tocco
    su di esso attiva il muto.
  - Pausa: finché il lettore è in pausa, il simbolo di pausa.
- **Cerca toccando la barra:** un tocco sulla barra di avanzamento salta a quel punto del brano.
- **Muto:** un tasto muto tra i tasti; un tocco sull'indicatore del volume attiva o toglie il muto.
- **Barra del volume:** una barra sotto il testo; un tocco su di essa imposta il volume in quel punto. Servono due righe.
- **Nome dell'album:** l'album su una terza riga, quando la tessera è abbastanza alta.
- **Indirizzo della copertina:** un indirizzo fisso di un file di copertina in Home Assistant, mostrato al posto
  dell'immagine del lettore (vedi «La copertina» più sotto).

## Tessera «Tasto SCMT»

Un tasto per lo stesso lettore: precedente, riproduci/pausa, successivo, più forte o più piano. Scegli il tasto nelle
opzioni della tessera e metti tutte le tessere che vuoi, accanto alla tessera «SCMT».

## Configurazione

1. Aggiungi il plugin al tuo schermo in Tessera (Plugins) e compila lo schermo.
2. Posiziona le tessere nel layout.
3. Nell'ispettore di ogni tessera scegli il lettore. Va bene qualsiasi entità `media_player`, per esempio quella che
   HASS.Agent crea dalla sessione multimediale di Windows, o l'integrazione Spotify.

I tasti usano le azioni di Home Assistant `media_player.media_play_pause`, `media_next_track`, `media_previous_track`,
`volume_up`, `volume_down`, `media_seek`, `volume_mute` e `volume_set`. Un lettore che non ne supporta una la ignora.

## La copertina

Per impostazione predefinita la copertina è l'immagine che Home Assistant stesso ha per il lettore (il suo
`entity_picture`); il plugin non scarica nulla da Internet.

Una tessera può mostrare solo immagini da `/api/image/serve/`, `/api/image_proxy/` o `/local/`, e gli attributi di
testo vengono tagliati a 48 byte. La copertina di alcuni lettori (per esempio quello di HASS.Agent,
`/api/media_player_proxy/...`) viene rifiutata per questo motivo. Per questi, usa l'opzione «Indirizzo della copertina»
insieme all'automazione qui sotto.

### Automazione della copertina (sperimentale)

L'automazione salva la copertina attuale del lettore come file nella cartella `www` di Home Assistant. La tessera
mostra poi quel file. Il lato Home Assistant funziona; che il tuo schermo mostri il file dipende dalla tua versione di
Tessera, quindi è sperimentale. Lo stesso codice è in `examples/cover_automation.yaml`.

1. Crea la cartella `/config/www/` se manca e riavvia Home Assistant una volta.
2. Aggiungi il comando al tuo `configuration.yaml` (poi ricarica i comandi shell o riavvia). Non c'è `&&` di proposito:
   Home Assistant esegue i comandi che contengono `{{ }}` senza shell.

```yaml
shell_command:
  cover_fetch: >-
    curl -sSfkL --connect-timeout 5 --max-time 20 -o /config/www/cover.jpg "{{ url }}"
```

3. Crea l'automazione. Sostituisci `media_player.your_player` con il tuo lettore e `YOUR_HOME_ASSISTANT` con l'indirizzo
   con cui apri Home Assistant, come lo vede il computer di Home Assistant (per esempio `http://192.168.0.10:8123`, o
   `https://ha.example.com`, l'opzione `-k` è già nel comando). Reagisce a un nuovo titolo, aspetta due secondi e scarica
   la copertina tre volte (dopo 2, 7 e 15 secondi), perché la nuova copertina spesso arriva qualche secondo dopo il
   titolo.

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

4. Nelle opzioni della tessera inserisci `/local/cover.jpg?v={v}` come «Indirizzo della copertina».

L'indirizzo viene usato esattamente come scritto, tranne `{v}`: il plugin lo sostituisce con un numero che cambia a
ogni brano, così lo schermo chiede di nuovo (Home Assistant ignora la parte `?v=...` e invia il file). Tre secondi dopo
un nuovo brano chiede ancora una volta, nel caso il file sia stato scritto un attimo in ritardo. Un indirizzo senza
`{v}` non cambia mai, quindi lo schermo mostra la prima immagine caricata finché l'indirizzo non cambia.

Per controllare il file apri `http://YOUR_HOME_ASSISTANT/local/cover.jpg?x=1` in una finestra privata del browser
(cambia il numero ogni volta, perché il browser conserva l'immagine).

## Privacy

Il plugin non fa richieste di rete. Legge solo ciò che Home Assistant già sa dell'entità scelta e chiama le azioni del
lettore citate sopra quando tocchi una tessera. L'automazione della copertina gira in Home Assistant, non nel plugin.
