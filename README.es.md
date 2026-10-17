# SCMT - Simple Customizable Media Tile

[English](README.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Nederlands](README.nl.md)

Dos tarjetas para Tessera que muestran y controlan lo que reproduce un reproductor multimedia de Home Assistant.

## Tarjeta «SCMT»

El título en letras grandes, debajo el artista y, abajo, una barra de progreso opcional.

- Mientras suena algo: el título y el artista. En pausa: lo mismo, con el título atenuado.
- Cuando no suena nada: «Nada en reproducción»; mientras no se haya elegido reproductor: «Elegir reproductor».
- Un título demasiado ancho para la tarjeta recibe primero una letra más pequeña y después se desplaza como texto
  corrido. Igual un artista y un álbum demasiado anchos.
- La opción «Progreso» muestra una barra (su altura también es una opción, de 2 a 16 píxeles), el tiempo (reproducido /
  duración), ambos o nada. El reproductor debe informar de `media_duration` y `media_position`; si no, no se muestra
  ninguno.
- La opción «Indicador de volumen» muestra el volumen a la derecha del título: durante tres segundos tras un cambio,
  siempre o nunca. Necesita `volume_level`; un reproductor silenciado muestra «Silenciado».
- Un toque en la tarjeta reproduce o pausa.
- La carátula del álbum aparece junto al título cuando la tarjeta tiene al menos dos celdas de ancho y la pantalla puede
  mostrar imágenes (las placas pequeñas no pueden). La opción «Carátula del álbum» la pone a la izquierda (por defecto)
  o a la derecha del texto, o la desactiva.
- La opción «Botones» añade botones bajo el título: reproducción (anterior, reproducir/pausa, siguiente), volumen (más
  bajo, silencio, más alto) o ambos. Necesitan una tarjeta de dos filas o más. Con los botones de volumen, un toque en
  un indicador de volumen también silencia.
- Un toque en la barra de progreso salta a ese punto del tema.

### Más opciones de esta tarjeta

- **Posición del texto:** título, artista y álbum en el centro (por defecto), arriba o abajo junto a la carátula.
  Arriba o abajo, el texto se alinea con el borde superior o inferior de la carátula.
- **Indicador grande:** un indicador semitransparente sobre toda la tarjeta, para el volumen, para la pausa o para
  ambos. Desactivado por defecto.
  - Volumen: durante tres segundos tras un cambio de volumen, el número y un altavoz. Con los botones de volumen, un
    toque en él silencia.
  - Pausa: mientras el reproductor está en pausa, el símbolo de pausa.
- **Tamaño del texto:** pequeño, mediano (por defecto) o grande, para el título, el artista y el álbum.
- **Barra de volumen:** una barra vertical en el borde derecho de la tarjeta; un toque en ella ajusta el volumen a esa
  altura. Necesita dos columnas.
- **Nombre del álbum:** el álbum en una tercera línea, cuando la tarjeta es lo bastante alta.
- **Dirección de la carátula:** una dirección fija de un archivo de carátula en Home Assistant, que se muestra en lugar
  de la imagen del reproductor (véase «La carátula» más abajo).

## Tarjeta «Botón SCMT»

Un botón para el mismo reproductor: anterior, reproducir/pausa, siguiente, más alto o más bajo. Elige el botón en las
opciones de la tarjeta y coloca tantas de estas tarjetas como quieras, junto a la tarjeta «SCMT».

## Puesta en marcha

1. Añade el plugin a tu pantalla en Tessera (Plugins) y compila la pantalla.
2. Coloca las tarjetas en el diseño.
3. En el inspector de cada tarjeta, elige el reproductor. Sirve cualquier entidad `media_player`, por ejemplo la que
   HASS.Agent crea a partir de la sesión multimedia de Windows, o la integración de Spotify.

Los botones usan las acciones de Home Assistant `media_player.media_play_pause`, `media_next_track`,
`media_previous_track`, `volume_up`, `volume_down`, `media_seek`, `volume_mute` y `volume_set`. Un reproductor que no
admita alguna de ellas la ignora.

## La carátula

Por defecto, la carátula es la imagen que Home Assistant mismo tiene para el reproductor (su `entity_picture`); el
plugin no descarga nada de internet.

Una tarjeta solo puede mostrar imágenes de `/api/image/serve/`, `/api/image_proxy/` o `/local/`, y los atributos de
texto se cortan a los 48 bytes. La carátula de algunos reproductores (por ejemplo la de HASS.Agent,
`/api/media_player_proxy/...`) se rechaza por esa razón.

**Nota:** la opción «Dirección de la carátula» junto con la automatización de abajo estaba pensada para estos
reproductores, pero no funciona con la aplicación Tessera actual (0.4.88). La aplicación obtiene una dirección bajo
`/local/` a través del Supervisor (`http://supervisor/core/local/...`), que la rechaza (403, en el registro de la
aplicación «no picture from /local/cover.jpg»), así que la tarjeta muestra un bloque gris en lugar de la carátula, sea
cual sea el formato del archivo. Ambas siguen en el plugin hasta que Tessera obtenga `/local/` de otra forma o permita
la carátula propia del reproductor ([MaxGramser/tessera-plugins#6](https://github.com/MaxGramser/tessera-plugins/issues/6)).

### Automatización de la carátula (experimental)

La automatización guarda la carátula actual del reproductor como archivo en la carpeta `www` de Home Assistant. La
tarjeta muestra entonces ese archivo. La parte de Home Assistant funciona; que tu pantalla muestre el archivo depende
de tu versión de Tessera, así que es experimental. El mismo código está en `examples/cover_automation.yaml`.

1. Crea la carpeta `/config/www/` si no existe y reinicia Home Assistant una vez.
2. Añade el comando a tu `configuration.yaml` (después recarga los comandos de shell o reinicia). No lleva `&&` a
   propósito: Home Assistant ejecuta los comandos que contienen `{{ }}` sin shell.

```yaml
shell_command:
  cover_fetch: >-
    curl -sSfkL --connect-timeout 5 --max-time 20 -o /config/www/cover.jpg "{{ url }}"
```

3. Crea la automatización. Sustituye `media_player.your_player` por tu reproductor y `YOUR_HOME_ASSISTANT` por la
   dirección con la que abres Home Assistant, tal como la ve el ordenador de Home Assistant (por ejemplo
   `http://192.168.0.10:8123`, o `https://ha.example.com`, la opción `-k` ya está en el comando). Reacciona a un título
   nuevo, espera dos segundos y descarga la carátula tres veces (a los 2, 7 y 15 segundos), porque la carátula nueva
   suele llegar unos segundos después del título.

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

4. En las opciones de la tarjeta, escribe `/local/cover.jpg?v={v}` como «Dirección de la carátula».

La dirección se usa exactamente como se escribe, salvo `{v}`: el plugin lo sustituye por un número que cambia con
cada tema, para que la pantalla vuelva a pedirla (Home Assistant ignora la parte `?v=...` y envía el archivo). Tres
segundos después de un tema nuevo vuelve a pedirla una vez, por si el archivo se escribió con algo de retraso. Una
dirección sin `{v}` nunca cambia, así que la pantalla muestra la primera imagen cargada hasta que cambie la dirección.

Para comprobar el archivo, abre `http://YOUR_HOME_ASSISTANT/local/cover.jpg?x=1` en una ventana privada del navegador
(cambia el número cada vez, porque el navegador conserva la imagen).

## Privacidad

El plugin no hace peticiones de red. Solo lee lo que Home Assistant ya sabe de la entidad elegida y llama a las acciones
del reproductor mencionadas arriba cuando tocas una tarjeta. La automatización de la carátula se ejecuta en Home
Assistant, no en el plugin.
