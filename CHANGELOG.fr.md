# Journal des modifications

## 0.10.0 - 2026-10-16

- Un titre, un artiste ou un album trop large pour la tuile défile comme un texte déroulant au lieu d'être coupé avec
  des points.
- La barre de volume se dresse au bord de la tuile, du côté opposé à la pochette (à droite, ou à gauche quand la
  pochette est à droite). Elle demande maintenant deux colonnes au lieu de deux lignes.
- « Avancer en touchant la barre » est activé par défaut.
- Espacement régulier : le même écart entre la pochette, le texte, la barre de progression et les boutons, et les
  boutons extérieurs s'alignent sur les bords.
- Avec le texte en haut ou en bas, il s'aligne sur le bord supérieur ou inférieur de la pochette.

## 0.9.1 - 2026-10-16

- Correction : le manifeste était refusé (« tiles[0].options: at most 12 entries »), parce que la tuile « SCMT » avait
  15 options. Trois paires sont maintenant un seul choix chacune, sans rien perdre :
  - « Pochette de l'album » : à gauche, à droite ou désactivée (était « Pochette de l'album » et « Position de la
    pochette »).
  - « Boutons » : aucun, lecture, volume ou les deux (était « Boutons de lecture » et « Boutons de volume »).
  - « Grand affichage » : aucun, volume, pause ou les deux (était « Grand affichage du volume » et « Affichage de la
    pause »).
- Les tuiles placées avec une version plus ancienne gardent leurs réglages ; la tuile lit toujours les anciennes
  options.

## 0.9.0 - 2026-10-16

- Nouvelle option « Affichage de la pause » : tant que le lecteur est en pause, un affichage semi-transparent avec le
  symbole de pause recouvre toute la tuile.
- Nouvelle option « Position de la pochette » : la pochette de l'album à gauche ou à droite du texte.
- Nouvelle option « Position du texte » : titre, artiste et album en haut, au milieu ou en bas de l'espace à côté de la
  pochette.
- Plus de langues : français, espagnol, italien et néerlandais, en plus de l'anglais et de l'allemand. Le README et ce
  journal existent aussi dans ces langues (`README.de.md`, `CHANGELOG.de.md`, etc.).
- Le README décrit l'automatisation qui amène la pochette d'un album sur l'écran (expérimental), avec un exemple dans
  `examples/cover_automation.yaml`.
- La tuile réserve un peu plus de mémoire pour le nouvel affichage.

## 0.8.1 - 2026-10-16

- L'« Adresse de la pochette » est maintenant utilisée exactement comme saisie. Un `{v}` dedans est remplacé par un
  nombre qui change à chaque titre (par exemple `/local/cover.jpg?v={v}`) ; avant, ce nombre était toujours ajouté
  derrière l'adresse sous la forme `?v=`.
- L'image de la boutique `store.png` pour la boutique de plugins a été ajoutée.

## 0.8.0 - 2026-10-15

- Nouvelle option « Adresse de la pochette » : une adresse fixe d'un fichier de pochette dans Home Assistant (par
  exemple `/local/cover.jpg`), affichée à la place de l'image du lecteur. Pour les lecteurs dont la tuile n'a pas le
  droit de charger la pochette.

## 0.7.1 - 2026-10-15

- Correction : la compilation échouait avec « Component not found: now_playing », parce que `plugin.yaml` nommait encore
  l'ancien composant. Il indique maintenant `scmt`, comme le manifeste et le dossier.

## 0.7.0 - 2026-10-15

- L'identifiant du plugin est maintenant `scmt` (il était `now_playing`), comme son dossier et son composant.
  L'application voit un nouveau plugin : retirez l'ancien de l'écran, ajoutez celui-ci et replacez ses tuiles.

## 0.6.0 - 2026-10-15

- Le plugin s'appelle SCMT (Simple Customizable Media Tile). Son identifiant, `now_playing`, reste, donc les tuiles déjà
  placées sur un écran continuent de fonctionner.
- Nouvelle option « Hauteur de la barre de progression » (de 2 à 16 pixels, 4 par défaut).

## 0.5.0 - 2026-10-15

- Nouvelles options : « Avancer en touchant la barre », « Muet », « Barre de volume » (un appui règle le volume), « Nom
  de l'album » (une troisième ligne).
- Nouvelle option « Grand affichage du volume » : pendant trois secondes après un changement, un affichage
  semi-transparent recouvre toute la tuile, avec le nombre en grands chiffres suivi d'un haut-parleur. Le petit
  affichage du volume a aussi un haut-parleur.
- La tuile peut atteindre 4x4 cellules (l'éditeur propose ce qui rentre dans la grille de l'écran).
- Demande trois actions de plus : `media_seek`, `volume_mute`, `volume_set`.

## 0.4.1 - 2026-10-14

- Correction : le manifeste était refusé parce que la valeur de choix `off` était lue comme une valeur booléenne ;
  chaque valeur de choix est maintenant écrite comme du texte.

## 0.4.0 - 2026-10-14

- Nouveau : l'option « Progression » choisit maintenant une barre, le temps (1:36 / 7:36), les deux, ou rien.
- Nouveau : l'option « Affichage du volume » montre le volume à droite du titre : quand il change (trois secondes),
  toujours, ou jamais. Un lecteur en sourdine affiche « Muet ».

## 0.3.2 - 2026-10-14

- Les boutons de la tuile ont un cadre clair.

## 0.3.1 - 2026-10-14

- L'emplacement de la pochette n'est affiché que si le lecteur a une image que l'écran a le droit de charger ; sinon le
  texte prend toute la largeur et aucun carré gris vide ne reste.

## 0.3.0 - 2026-10-14

- Nouveau : la pochette de l'album à côté du titre (option « Pochette de l'album »), à partir de deux cellules de large,
  sur les écrans qui affichent des images.
- Nouveau : boutons de lecture (précédent, lecture/pause, suivant) et boutons de volume (moins fort, plus fort) dans la
  tuile, en options. Ils demandent une tuile d'au moins deux lignes.
- Demande l'API de plugin 0.10.

## 0.2.0 - 2026-10-14

- Nouveau : une barre de progression sur la tuile (option « Barre de progression », activée par défaut), si le lecteur
  indique la durée et la position.
- Nouveau : un appui sur la tuile lance ou met en pause la lecture.
- Nouveau : la tuile « Bouton média » (précédent, lecture/pause, suivant, plus fort, moins fort), une tuile par bouton.
- La tuile peut maintenant faire 1x1 ; elle faisait au moins 2x1.

## 0.1.1 - 2026-10-14

- Correction : la compilation échouait parce que les couleurs étaient écrites `tessera::theme::` ; c'est `theme::`.

## 0.1.0 - 2026-10-14

- Première version : affiche le titre et l'artiste d'un lecteur multimédia.
