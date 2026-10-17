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
- **Tempo:** el tempo de la música (por ejemplo «138 BPM») en una línea bajo el artista, cuando la tarjeta es lo
  bastante alta. Viene del sensor de tempo elegido para la pantalla (ver «Tempo» más abajo).
- **Dirección de la carátula:** la dirección de una imagen en Home Assistant, que se muestra en lugar de la imagen
  del reproductor, por ejemplo una imagen de plantilla con la carátula del reproductor (véase «La carátula» más abajo).

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

## Tempo

La opción «Tempo» de la tarjeta «SCMT» muestra el tempo de la música en una línea bajo el artista. Home Assistant no
conoce un tempo propio, así que viene de un sensor, por ejemplo el que HASS.Agent crea a partir de
[BPM2SCMT](https://github.com/Schumey/BPM2SCMT), que mide lo que reproduce un PC con Windows.

1. En Tessera (Plugins), elige el sensor como «Sensor de tempo» del plugin para la pantalla y compila la pantalla. Su
   estado debe ser el tempo como número (`138`); `unknown` o vacío oculta la línea.
2. Activa «Tempo» en las opciones de la tarjeta.

La tarjeta muestra el tempo mientras el reproductor suena o está en pausa. Tras una pista nueva espera a que el sensor
tenga el nuevo tempo (estuvo vacío una vez y vuelve a tener un valor), como mucho 12 segundos, para no mostrar el tempo
de la pista anterior. El sensor es uno por pantalla, no por tarjeta: pertenece al PC que reproduce, sea cual sea el
reproductor que muestra la tarjeta.

## La carátula

Por defecto, la carátula es la imagen que Home Assistant mismo tiene para el reproductor (su `entity_picture`); el
plugin no descarga nada de internet.

Una tarjeta solo puede mostrar imágenes de `/api/image/serve/`, `/api/image_proxy/` o `/local/`, y los atributos de
texto se cortan a los 48 bytes. La carátula de algunos reproductores (por ejemplo la de HASS.Agent,
`/api/media_player_proxy/...`) se rechaza por esa razón.

Para estos reproductores existe la opción «Dirección de la carátula» junto con una imagen de plantilla en Home
Assistant.

### La carátula mediante una imagen de plantilla

Una imagen de plantilla es una entidad de Home Assistant que contiene la carátula actual del reproductor. La tarjeta la
muestra a través de `/api/image_proxy/`, una dirección que la aplicación Tessera puede obtener. No hace falta ninguna
automatización, ningún archivo ni ningún cambio en `configuration.yaml`.

1. En Home Assistant, abre **Ajustes → Dispositivos y servicios → Ayudantes → Crear ayudante → Plantilla → Imagen**.
   Ponle un nombre, por ejemplo «SCMT Cover» (la entidad será `image.scmt_cover`), e introduce como URL:

```
http://127.0.0.1{{ state_attr('media_player.your_player', 'entity_picture') }}
```

   Sustituye `media_player.your_player` por tu reproductor. `http://127.0.0.1` es Home Assistant tal como se
   alcanza a sí mismo; la dirección de la imagen del reproductor ya lleva su token, así que Home Assistant puede
   obtenerla sin iniciar sesión.
2. En las opciones de la tarjeta, introduce como «Dirección de la carátula»:

```
/api/image_proxy/image.scmt_cover?v={v}
```

   El ID de la entidad debe coincidir exactamente con el del ayudante (aparece en los ajustes del ayudante).

La dirección se usa exactamente como se escribe, salvo `{v}`: el plugin lo sustituye por un número que cambia con cada
tema, para que la pantalla vuelva a pedirla (Home Assistant ignora la parte `?v=...`). Tres segundos después de un tema
nuevo la pide una vez más, por si la imagen llegó un poco tarde. Una dirección sin `{v}` no cambia nunca, así que la
pantalla muestra la primera imagen que cargó hasta que cambie la dirección.

Para comprobarlo, abre el ayudante en Home Assistant: muestra la carátula actual. Abrir
`http://YOUR_HOME_ASSISTANT/api/image_proxy/image.scmt_cover` en un navegador da «401: Unauthorized»; es normal, porque
la dirección necesita un token (`?token=...`), que la aplicación Tessera envía por sí misma.

Un archivo en la carpeta `www` (`/local/...`) no funciona con la aplicación Tessera actual (0.4.88): obtiene `/local/` a
través del Supervisor, que lo rechaza (403), así que la tarjeta muestra un bloque gris
([MaxGramser/tessera-plugins#6](https://github.com/MaxGramser/tessera-plugins/issues/6)).

## Privacidad

El plugin no hace peticiones de red. Solo lee lo que Home Assistant ya sabe de la entidad elegida y llama a las acciones
del reproductor mencionadas arriba cuando tocas una tarjeta. La imagen de plantilla de la carátula se ejecuta en Home
Assistant, no en el plugin.
