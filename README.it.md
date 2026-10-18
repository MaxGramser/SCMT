# SCMT - Simple Customizable Media Tile

[English](README.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Nederlands](README.nl.md)

Due tessere per Tessera che mostrano e controllano ciò che riproduce un lettore multimediale di Home Assistant.

## Tessera «SCMT»

Il titolo in caratteri grandi, sotto l'artista e in basso una barra di avanzamento facoltativa.

- Mentre qualcosa suona: titolo e artista. In pausa: lo stesso, con il titolo attenuato.
- Quando non suona nulla: «Nessuna riproduzione»; finché non è scelto un lettore: «Scegli un lettore».
- Un titolo troppo largo per la tessera riceve prima un carattere più piccolo, poi scorre come testo scorrevole. Lo
  stesso vale per un artista e un album troppo larghi.
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
  piano, muto, più forte) o entrambi. Serve una tessera di almeno due righe. Con i tasti del volume, anche un tocco su un
  indicatore del volume attiva il muto.
- Un tocco sulla barra di avanzamento salta a quel punto del brano.

### Altre opzioni di questa tessera

- **Posizione del testo:** titolo, artista e album al centro (predefinito), in alto o in basso accanto alla copertina.
  In alto o in basso, il testo si allinea al bordo superiore o inferiore della copertina.
- **Indicatore grande:** un indicatore semitrasparente su tutta la tessera, per il volume, per la pausa o per entrambi.
  Disattivato per impostazione predefinita.
  - Volume: per tre secondi dopo una modifica del volume, il numero e un altoparlante. Con i tasti del volume, un tocco
    su di esso attiva il muto.
  - Pausa: finché il lettore è in pausa, il simbolo di pausa.
- **Dimensione del testo:** piccola, media (predefinita) o grande, per titolo, artista e album.
- **Barra del volume:** una barra verticale sul bordo destro della tessera; un tocco su di essa imposta il volume a
  quell'altezza. Servono due colonne.
- **Colore delle barre:** il riempimento della barra di avanzamento e della barra del volume: il colore d'accento dello
  schermo (predefinito), verde, ambra, rosso, blu, viola o come il testo. Sono i colori del nucleo, leggibili in chiaro
  e in scuro; Tessera non permette colori propri.
- **Righe extra:** sotto l'artista, l'album, il tempo della musica (per esempio «138 BPM») o entrambi, quando la
  tessera è abbastanza alta. Il tempo viene dal sensore di tempo scelto per lo schermo (vedi «Tempo» più sotto).
- **Indirizzo della copertina:** l'indirizzo di un'immagine in Home Assistant, mostrata al posto dell'immagine del
  lettore, per esempio un'immagine modello con la copertina del lettore (vedi «La copertina» più sotto).

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

## Tempo

La scelta «Tempo» dell'opzione «Righe extra» della tessera «SCMT» mostra il tempo della musica su una riga sotto
l'artista. Home Assistant non conosce un tempo proprio, quindi viene da un sensore, per esempio quello che HASS.Agent
crea da [BPM2SCMT](https://github.com/Schumey/BPM2SCMT), che misura ciò che riproduce un PC Windows.

1. In Tessera (Plugins), scegli il sensore come «Sensore di tempo» del plugin per lo schermo e compila lo schermo. Il
   suo stato deve essere il tempo come numero (`138`); `unknown` o vuoto nasconde la riga.
2. Nelle opzioni della tessera, scegli «Tempo» o «Album e tempo» in «Righe extra».

La tessera mostra il tempo finché il lettore suona o è in pausa. Dopo un nuovo brano aspetta che il sensore abbia il
nuovo tempo (è stato vuoto una volta e ha di nuovo un valore), al massimo 12 secondi, per non mostrare il tempo del
brano precedente. Il sensore è uno per schermo, non per tessera: appartiene al PC che suona, qualunque lettore mostri la
tessera.

## La copertina

Per impostazione predefinita la copertina è l'immagine che Home Assistant stesso ha per il lettore (il suo
`entity_picture`); il plugin non scarica nulla da Internet.

Una tessera può mostrare solo immagini da `/api/image/serve/`, `/api/image_proxy/` o `/local/`, e gli attributi di
testo vengono tagliati a 48 byte. La copertina di alcuni lettori (per esempio quello di HASS.Agent,
`/api/media_player_proxy/...`) viene rifiutata per questo motivo.

Per questi lettori c'è l'opzione «Indirizzo della copertina» insieme a un'immagine modello (template) in Home
Assistant.

### La copertina tramite un'immagine modello

Un'immagine modello è un'entità di Home Assistant che contiene la copertina attuale del lettore. La tessera la mostra
tramite `/api/image_proxy/`, un indirizzo che l'app Tessera può recuperare. Non servono automazioni, file o modifiche a
`configuration.yaml`.

1. In Home Assistant apri **Impostazioni → Dispositivi e servizi → Aiutanti → Crea aiutante → Modello → Immagine**.
   Dagli un nome, per esempio «SCMT Cover» (l'entità sarà `image.scmt_cover`), e inserisci come URL:

```
http://127.0.0.1{{ state_attr('media_player.your_player', 'entity_picture') }}
```

   Sostituisci `media_player.your_player` con il tuo lettore. `http://127.0.0.1` è Home Assistant come raggiunge se
   stesso; l'indirizzo dell'immagine del lettore contiene già il suo token, quindi Home Assistant può recuperarla senza
   accesso.
2. Nelle opzioni della tessera inserisci come «Indirizzo della copertina»:

```
/api/image_proxy/image.scmt_cover?v={v}
```

   L'ID dell'entità deve corrispondere esattamente a quello dell'aiutante (si trova nelle impostazioni dell'aiutante).

L'indirizzo viene usato esattamente come scritto, tranne `{v}`: il plugin lo sostituisce con un numero che cambia a ogni
brano, così lo schermo lo richiede (Home Assistant ignora la parte `?v=...`). Tre secondi dopo un nuovo brano lo
richiede ancora una volta, nel caso l'immagine sia arrivata un po' tardi. Un indirizzo senza `{v}` non cambia mai,
quindi lo schermo mostra la prima immagine caricata finché l'indirizzo non cambia.

Per verificare, apri l'aiutante in Home Assistant: mostra la copertina attuale. Aprire
`http://YOUR_HOME_ASSISTANT/api/image_proxy/image.scmt_cover` in un browser dà «401: Unauthorized»; è normale, perché
l'indirizzo richiede un token (`?token=...`), che l'app Tessera invia da sé.

Un file nella cartella `www` (`/local/...`) non funziona con l'app Tessera attuale (0.4.88): recupera `/local/` tramite
il Supervisor, che lo rifiuta (403), quindi la tessera mostra un blocco grigio
([MaxGramser/tessera-plugins#6](https://github.com/MaxGramser/tessera-plugins/issues/6)).

## Privacy

Il plugin non fa richieste di rete. Legge solo ciò che Home Assistant già sa dell'entità scelta e chiama le azioni del
lettore citate sopra quando tocchi una tessera. L'immagine modello della copertina gira in Home Assistant, non nel
plugin.
