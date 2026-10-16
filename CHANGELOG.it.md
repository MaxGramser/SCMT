# Registro delle modifiche

## 0.9.0 - 2026-10-16

- Nuova opzione «Indicatore di pausa»: finché il lettore è in pausa, un indicatore semitrasparente con il simbolo di
  pausa copre tutta la tessera.
- Nuova opzione «Posizione della copertina»: la copertina dell'album a sinistra o a destra del testo.
- Nuova opzione «Posizione del testo»: titolo, artista e album in alto, al centro o in basso nello spazio accanto alla
  copertina.
- Più lingue: francese, spagnolo, italiano e olandese, oltre a inglese e tedesco. Anche il README e questo registro
  esistono in queste lingue (`README.de.md`, `CHANGELOG.de.md` e così via).
- Il README descrive l'automazione che porta la copertina di un album sul display (sperimentale), con un esempio in
  `examples/cover_automation.yaml`.
- La tessera riserva un po' più di memoria per il nuovo indicatore.

## 0.8.1.1 - 2026-10-16

- È stata aggiunta l'immagine dello store `store.png` per lo store dei plugin.

## 0.8.1 - 2026-10-16

- L'«Indirizzo della copertina» viene ora usato esattamente come scritto. Un `{v}` al suo interno viene sostituito da un
  numero che cambia a ogni brano (per esempio `/local/cover.jpg?v={v}`); prima quel numero veniva sempre aggiunto dopo
  l'indirizzo come `?v=`.

## 0.8.0 - 2026-10-15

- Nuova opzione «Indirizzo della copertina»: un indirizzo fisso di un file di copertina in Home Assistant (per esempio
  `/local/cover.jpg`), mostrato al posto dell'immagine del lettore. Per i lettori la cui copertina la tessera non può
  caricare.

## 0.7.1 - 2026-10-15

- Correzione: la compilazione falliva con «Component not found: now_playing», perché `plugin.yaml` nominava ancora il
  vecchio componente. Ora dice `scmt`, come il manifesto e la cartella.

## 0.7.0 - 2026-10-15

- L'id del plugin è ora `scmt` (prima `now_playing`), come la sua cartella e il suo componente. L'app vede un nuovo
  plugin: rimuovi il vecchio dallo schermo, aggiungi questo e posiziona di nuovo le sue tessere.

## 0.6.0 - 2026-10-15

- Il plugin si chiama SCMT (Simple Customizable Media Tile). Il suo id, `now_playing`, resta, quindi le tessere già
  presenti su uno schermo continuano a funzionare.
- Nuova opzione «Altezza della barra di avanzamento» (da 2 a 16 pixel, 4 per impostazione predefinita).

## 0.5.0 - 2026-10-15

- Nuove opzioni: «Cerca toccando la barra», «Muto», «Barra del volume» (un tocco imposta il volume), «Nome dell'album»
  (una terza riga).
- Nuova opzione «Indicatore del volume grande»: per tre secondi dopo una modifica, un indicatore semitrasparente copre
  tutta la tessera, con il numero in cifre grandi e un altoparlante dopo. Anche l'indicatore del volume piccolo ha un
  altoparlante.
- La tessera può arrivare a 4x4 celle (l'editor offre ciò che entra nella griglia dello schermo).
- Servono altre tre azioni: `media_seek`, `volume_mute`, `volume_set`.

## 0.4.1 - 2026-10-14

- Correzione: il manifesto veniva rifiutato perché il valore di scelta `off` veniva letto come valore booleano; ora ogni
  valore di scelta è scritto come testo.

## 0.4.0 - 2026-10-14

- Novità: l'opzione «Avanzamento» sceglie ora una barra, il tempo (1:36 / 7:36), entrambi o niente.
- Novità: l'opzione «Indicatore del volume» mostra il volume a destra del titolo: quando cambia (tre secondi), sempre o
  mai. Un lettore in muto mostra «Muto».

## 0.3.2 - 2026-10-14

- I tasti nella tessera hanno una cornice chiara.

## 0.3.1 - 2026-10-14

- Lo spazio della copertina viene mostrato solo se il lettore ha un'immagine che lo schermo può caricare; altrimenti il
  testo occupa tutta la larghezza e non resta un quadrato grigio vuoto.

## 0.3.0 - 2026-10-14

- Novità: la copertina dell'album accanto al titolo (opzione «Copertina dell'album»), da due celle di larghezza, sugli
  schermi che mostrano immagini.
- Novità: tasti di riproduzione (precedente, riproduci/pausa, successivo) e tasti del volume (più piano, più forte)
  nella tessera, come opzioni. Servono una tessera di almeno due righe.
- Serve l'API plugin 0.10.

## 0.2.0 - 2026-10-14

- Novità: una barra di avanzamento nella tessera (opzione «Barra di avanzamento», attiva per impostazione predefinita),
  se il lettore indica durata e posizione.
- Novità: un tocco sulla tessera avvia o mette in pausa la riproduzione.
- Novità: la tessera «Tasto multimediale» (precedente, riproduci/pausa, successivo, più forte, più piano), una tessera
  per tasto.
- La tessera può ora essere 1x1; prima era almeno 2x1.

## 0.1.1 - 2026-10-14

- Correzione: la compilazione falliva perché i colori erano scritti `tessera::theme::`; è `theme::`.

## 0.1.0 - 2026-10-14

- Prima versione: mostra titolo e artista di un lettore multimediale.
