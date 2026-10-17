# Registro de cambios

## 0.12.0 - 2026-10-17

- Nueva opción «Tempo»: el tempo de la música (por ejemplo «138 BPM») en una línea bajo el artista. Viene de un sensor
  elegido por pantalla en Tessera (nueva entrada «Sensor de tempo»), por ejemplo el que HASS.Agent crea a partir de
  BPM2SCMT. Tras una pista nueva, la línea espera a que el sensor tenga el nuevo tempo.
- El plugin lee ahora él mismo esa entidad (permiso `read_entities`); Tessera lo pide al actualizar.
- El README describe un nuevo camino a la carátula para los reproductores cuya carátula se rechaza: una imagen de
  plantilla en Home Assistant, mostrada con la «Dirección de la carátula» `/api/image_proxy/image.scmt_cover?v={v}`.
  Sustituye a la automatización de la carátula con un archivo en `/local/`, que no funciona con Tessera 0.4.88 (403).
  El plugin no cambia para ello.

## 0.11.0 - 2026-10-17

- Nueva opción «Tamaño del texto»: pequeño (como antes de 0.10.3), mediano (por defecto, como ahora) o grande (artista
  y álbum un paso más grandes, un título largo se desplaza en la letra grande).
- Un toque en la barra de progreso siempre avanza; la opción «Avanzar tocando la barra» desaparece.
- Silencio forma parte de los botones de volumen, entre más bajo y más alto; la opción «Silencio» desaparece. Con los
  botones de volumen, un toque en un indicador de volumen también silencia.

## 0.10.3 - 2026-10-17

- Texto más grande: el título en la letra grande del valor de una tarjeta, el artista y el álbum en la letra de las
  palabras de un botón. Un título largo solo se reduce hasta el tamaño anterior y después se desplaza; en una tarjeta
  baja todo se reduce como antes.
- Corrección: el texto corrido ya no pasa sobre la carátula ni sobre el borde de la tarjeta.

## 0.10.2 - 2026-10-17

- La barra de volumen está siempre en el borde derecho, también cuando la carátula está a la derecha, y es un tercio
  más estrecha.
- Espaciado uniforme: la carátula, el texto, la barra de progreso y los botones están ahora tan separados entre sí como
  del borde de la tarjeta.

## 0.10.1 - 2026-10-17

- Corrección: en una tarjeta baja (la pantalla pequeña de 2,8 pulgadas) el artista ya no cubre el tiempo. El título
  toma también una letra más pequeña cuando es demasiado alto, y queda solo cuando no hay sitio para el artista.
- El README indica ahora que la opción «Dirección de la carátula» y la automatización de la carátula no funcionan con la
  aplicación Tessera actual (0.4.88): obtiene `/local/` a través del Supervisor, que lo rechaza (403), así que la
  tarjeta muestra un bloque gris.
- La imagen de la tienda de plugins muestra el nuevo aspecto.

## 0.10.0 - 2026-10-16

- Un título, artista o álbum demasiado ancho para la tarjeta se desplaza como texto corrido en lugar de cortarse con
  puntos.
- La barra de volumen está en vertical en el borde de la tarjeta, en el lado opuesto a la carátula (a la derecha, o a
  la izquierda cuando la carátula está a la derecha). Ahora necesita dos columnas en lugar de dos filas.
- «Avanzar tocando la barra» está activado por defecto.
- Espaciado uniforme: la misma separación entre la carátula, el texto, la barra de progreso y los botones, y los
  botones exteriores se alinean con los bordes.
- Con el texto arriba o abajo, se alinea con el borde superior o inferior de la carátula.

## 0.9.1 - 2026-10-16

- Corrección: el manifiesto se rechazaba («tiles[0].options: at most 12 entries») porque la tarjeta «SCMT» tenía 15
  opciones. Tres pares son ahora una sola elección cada uno, sin perder nada:
  - «Carátula del álbum»: izquierda, derecha o desactivada (era «Carátula del álbum» y «Posición de la carátula»).
  - «Botones»: ninguno, reproducción, volumen o ambos (era «Botones de reproducción» y «Botones de volumen»).
  - «Indicador grande»: ninguno, volumen, pausa o ambos (era «Indicador de volumen grande» e «Indicador de pausa»).
- Las tarjetas colocadas con una versión anterior conservan sus ajustes; la tarjeta sigue leyendo las opciones
  anteriores.

## 0.9.0 - 2026-10-16

- Nueva opción «Indicador de pausa»: mientras el reproductor está en pausa, un indicador semitransparente con el símbolo
  de pausa cubre toda la tarjeta.
- Nueva opción «Posición de la carátula»: la carátula del álbum a la izquierda o a la derecha del texto.
- Nueva opción «Posición del texto»: título, artista y álbum arriba, en el centro o abajo del espacio junto a la
  carátula.
- Más idiomas: francés, español, italiano y neerlandés, además de inglés y alemán. El README y este registro también
  existen en estos idiomas (`README.de.md`, `CHANGELOG.de.md`, etc.).
- El README describe la automatización que lleva la carátula de un álbum a la pantalla (experimental), con un ejemplo en
  `examples/cover_automation.yaml`.
- La tarjeta reserva algo más de memoria para el nuevo indicador.

## 0.8.1 - 2026-10-16

- La «Dirección de la carátula» se usa ahora exactamente como se escribe. Un `{v}` en ella se sustituye por un número
  que cambia con cada tema (por ejemplo `/local/cover.jpg?v={v}`); antes ese número se añadía siempre detrás de la
  dirección como `?v=`.
- Se añadió la imagen de la tienda `store.png` para la tienda de plugins.

## 0.8.0 - 2026-10-15

- Nueva opción «Dirección de la carátula»: una dirección fija de un archivo de carátula en Home Assistant (por ejemplo
  `/local/cover.jpg`), que se muestra en lugar de la imagen del propio reproductor. Para reproductores cuya carátula la
  tarjeta no puede cargar.

## 0.7.1 - 2026-10-15

- Corrección: la compilación fallaba con «Component not found: now_playing», porque `plugin.yaml` aún nombraba el
  componente antiguo. Ahora dice `scmt`, igual que el manifiesto y la carpeta.

## 0.7.0 - 2026-10-15

- El id del plugin es ahora `scmt` (antes `now_playing`), igual que su carpeta y su componente. La aplicación ve un
  plugin nuevo: quita el antiguo de la pantalla, añade este y vuelve a colocar sus tarjetas.

## 0.6.0 - 2026-10-15

- El plugin se llama SCMT (Simple Customizable Media Tile). Su id, `now_playing`, se mantiene, así que las tarjetas que
  ya están en una pantalla siguen funcionando.
- Nueva opción «Altura de la barra de progreso» (de 2 a 16 píxeles, 4 por defecto).

## 0.5.0 - 2026-10-15

- Nuevas opciones: «Avanzar tocando la barra», «Silencio», «Barra de volumen» (un toque ajusta el volumen), «Nombre del
  álbum» (una tercera línea).
- Nueva opción «Indicador de volumen grande»: durante tres segundos tras un cambio, un indicador semitransparente cubre
  toda la tarjeta, con el número en cifras grandes y un altavoz detrás. El indicador de volumen pequeño también tiene un
  altavoz.
- La tarjeta puede tener hasta 4x4 celdas (el editor ofrece lo que cabe en la cuadrícula de la pantalla).
- Necesita tres acciones más: `media_seek`, `volume_mute`, `volume_set`.

## 0.4.1 - 2026-10-14

- Corrección: el manifiesto se rechazaba porque el valor de elección `off` se leía como valor booleano; ahora cada valor
  de elección se escribe como texto.

## 0.4.0 - 2026-10-14

- Nuevo: la opción «Progreso» elige ahora una barra, el tiempo (1:36 / 7:36), ambos o nada.
- Nuevo: la opción «Indicador de volumen» muestra el volumen a la derecha del título: al cambiar (tres segundos),
  siempre o nunca. Un reproductor silenciado muestra «Silenciado».

## 0.3.2 - 2026-10-14

- Los botones de la tarjeta tienen un marco claro.

## 0.3.1 - 2026-10-14

- El hueco de la carátula solo se muestra cuando el reproductor tiene una imagen que la pantalla puede cargar; si no, el
  texto ocupa todo el ancho y no queda un cuadrado gris vacío.

## 0.3.0 - 2026-10-14

- Nuevo: la carátula del álbum junto al título (opción «Carátula del álbum»), desde dos celdas de ancho, en pantallas
  que muestran imágenes.
- Nuevo: botones de reproducción (anterior, reproducir/pausa, siguiente) y de volumen (más bajo, más alto) dentro de la
  tarjeta, como opciones. Necesitan una tarjeta de dos filas o más.
- Necesita la API de plugins 0.10.

## 0.2.0 - 2026-10-14

- Nuevo: una barra de progreso en la tarjeta (opción «Barra de progreso», activada por defecto), si el reproductor
  indica duración y posición.
- Nuevo: un toque en la tarjeta reproduce o pausa.
- Nuevo: la tarjeta «Botón multimedia» (anterior, reproducir/pausa, siguiente, más alto, más bajo), una tarjeta por
  botón.
- La tarjeta ya puede ser de 1x1; antes era al menos de 2x1.

## 0.1.1 - 2026-10-14

- Corrección: la compilación fallaba porque los colores estaban escritos como `tessera::theme::`; es `theme::`.

## 0.1.0 - 2026-10-14

- Primera versión: muestra el título y el artista de un reproductor multimedia.
