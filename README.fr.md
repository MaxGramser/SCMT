# SCMT - Simple Customizable Media Tile

[English](README.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Nederlands](README.nl.md)

Deux tuiles pour Tessera qui affichent et commandent ce que lit un lecteur multimédia de Home Assistant.

## Tuile « SCMT »

Le titre en grandes lettres, l'artiste dessous, et en bas une barre de progression facultative.

- Pendant la lecture : le titre et l'artiste. En pause : la même chose, avec le titre atténué.
- Quand rien ne joue : « Aucune lecture » ; tant qu'aucun lecteur n'est choisi : « Choisir un lecteur ».
- Un titre trop large pour la tuile reçoit d'abord une police plus petite, puis défile comme un texte déroulant. De
  même pour un artiste et un album trop larges.
- L'option « Progression » affiche une barre (sa hauteur est aussi une option, de 2 à 16 pixels), le temps (écoulé /
  durée), les deux, ou rien. Le lecteur doit indiquer `media_duration` et `media_position` ; sinon rien ne s'affiche.
- L'option « Affichage du volume » montre le volume à droite du titre : pendant trois secondes après un changement,
  toujours, ou jamais. Elle demande `volume_level` ; un lecteur en sourdine affiche « Muet ».
- Un appui sur la tuile lance ou met en pause la lecture.
- La pochette de l'album se place à côté du titre quand la tuile fait au moins deux cellules de large et que l'écran
  peut afficher des images (les petites cartes ne le peuvent pas). L'option « Pochette de l'album » la place à gauche
  (par défaut) ou à droite du texte, ou la désactive.
- L'option « Boutons » ajoute des boutons sous le titre : lecture (précédent, lecture/pause, suivant), volume (moins
  fort, muet, plus fort) ou les deux. Ils demandent une tuile d'au moins deux lignes. Avec les boutons de volume, un
  appui sur un affichage du volume coupe aussi le son.
- Un appui sur la barre de progression saute à cet endroit du morceau.

### Autres options de cette tuile

- **Position du texte :** titre, artiste et album au milieu (par défaut), en haut ou en bas à côté de la pochette. En
  haut ou en bas, le texte s'aligne sur le bord supérieur ou inférieur de la pochette.
- **Grand affichage :** un affichage semi-transparent sur toute la tuile, pour le volume, pour la pause ou pour les
  deux. Désactivé par défaut.
  - Volume : pendant trois secondes après un changement de volume, le nombre et un haut-parleur. Avec les boutons de volume,
    un appui dessus coupe le son.
  - Pause : tant que le lecteur est en pause, le symbole de pause.
- **Taille du texte :** petite, moyenne (par défaut) ou grande, pour le titre, l'artiste et l'album.
- **Barre de volume :** une barre verticale au bord droit de la tuile ; un appui dessus règle le volume à cette
  hauteur. Demande deux colonnes.
- **Nom de l'album :** l'album sur une troisième ligne, quand la tuile est assez haute.
- **Tempo :** le tempo de la musique (par exemple « 138 BPM ») sur une ligne sous l'artiste, quand la tuile est
  assez haute. Il vient du capteur de tempo choisi pour l'écran (voir « Tempo » plus bas).
- **Adresse de la pochette :** l'adresse d'une image dans Home Assistant, affichée à la place de l'image du lecteur,
  par exemple une image de modèle avec la pochette du lecteur (voir « La pochette » plus bas).

## Tuile « Bouton SCMT »

Un bouton pour le même lecteur : précédent, lecture/pause, suivant, plus fort ou moins fort. Choisissez le bouton dans
les options de la tuile et placez autant de ces tuiles que vous voulez, à côté de la tuile « SCMT ».

## Mise en place

1. Ajoutez le plugin à votre écran dans Tessera (Plugins) et compilez l'écran.
2. Placez les tuiles dans la mise en page.
3. Dans l'inspecteur de chaque tuile, choisissez le lecteur. Toute entité `media_player` convient, par exemple celle que
   HASS.Agent crée à partir de la session multimédia de Windows, ou l'intégration Spotify.

Les boutons utilisent les actions Home Assistant `media_player.media_play_pause`, `media_next_track`,
`media_previous_track`, `volume_up`, `volume_down`, `media_seek`, `volume_mute` et `volume_set`. Un lecteur qui ne
prend pas l'une d'elles en charge l'ignore.

## Tempo

L'option « Tempo » de la tuile « SCMT » affiche le tempo de la musique sur une ligne sous l'artiste. Home Assistant ne
connaît pas de tempo par lui-même : il vient donc d'un capteur, par exemple celui que HASS.Agent crée à partir de
[BPM2SCMT](https://github.com/Schumey/BPM2SCMT), qui mesure ce que joue un PC Windows.

1. Dans Tessera (Plugins), choisissez le capteur comme « Capteur de tempo » du plugin pour l'écran, puis construisez
   l'écran. Son état doit être le tempo sous forme de nombre (`138`) ; `unknown` ou vide masque la ligne.
2. Activez « Tempo » dans les options de la tuile.

La tuile affiche le tempo tant que le lecteur joue ou est en pause. Après un nouveau morceau, elle attend que le capteur
ait le nouveau tempo (il a été vide une fois et a de nouveau une valeur), au plus 12 secondes, pour ne pas afficher le
tempo du morceau précédent. Le capteur est unique par écran, pas par tuile : il appartient au PC qui joue, quel que soit
le lecteur que montre la tuile.

## La pochette

Par défaut, la pochette est l'image que Home Assistant possède lui-même pour le lecteur (son `entity_picture`) ; le
plugin ne charge rien depuis Internet.

Une tuile ne peut afficher que des images venant de `/api/image/serve/`, `/api/image_proxy/` ou `/local/`, et les
attributs texte sont coupés à 48 octets. La pochette de certains lecteurs (par exemple celui de HASS.Agent,
`/api/media_player_proxy/...`) est refusée pour cette raison.

Pour ces lecteurs, il y a l'option « Adresse de la pochette » avec une image de modèle (template) dans Home Assistant.

### La pochette par une image de modèle

Une image de modèle est une entité de Home Assistant qui contient la pochette actuelle du lecteur. La tuile l'affiche
via `/api/image_proxy/`, une adresse que l'application Tessera peut récupérer. Il ne faut ni automatisation, ni
fichier, ni modification de `configuration.yaml`.

1. Dans Home Assistant, ouvrez **Paramètres → Appareils et services → Entrées → Créer une entrée → Modèle → Image**.
   Donnez-lui un nom, par exemple « SCMT Cover » (l'entité est alors `image.scmt_cover`), et saisissez comme URL :

```
http://127.0.0.1{{ state_attr('media_player.your_player', 'entity_picture') }}
```

   Remplacez `media_player.your_player` par votre lecteur. `http://127.0.0.1` est Home Assistant tel qu'il se joint
   lui-même ; l'adresse de l'image du lecteur contient déjà son jeton, donc Home Assistant peut la récupérer sans
   connexion.
2. Dans les options de la tuile, saisissez comme « Adresse de la pochette » :

```
/api/image_proxy/image.scmt_cover?v={v}
```

   L'ID d'entité doit correspondre exactement à celui de l'entrée (il figure dans les paramètres de l'entrée).

L'adresse est utilisée exactement comme saisie, sauf `{v}` : le plugin le remplace par un nombre qui change à chaque
titre, pour que l'écran redemande (Home Assistant ignore la partie `?v=...`). Trois secondes après un nouveau titre, il
redemande une fois, au cas où l'image serait arrivée un peu tard. Une adresse sans `{v}` ne change jamais, donc l'écran
montre la première image chargée jusqu'à ce que l'adresse change.

Pour vérifier, ouvrez l'entrée dans Home Assistant : elle montre la pochette actuelle. Ouvrir
`http://YOUR_HOME_ASSISTANT/api/image_proxy/image.scmt_cover` dans un navigateur donne « 401: Unauthorized » ; c'est
normal, car l'adresse demande un jeton (`?token=...`), que l'application Tessera envoie elle-même.

Un fichier dans le dossier `www` (`/local/...`) ne fonctionne pas avec l'application Tessera actuelle (0.4.88) : elle
récupère `/local/` via le Supervisor, qui le refuse (403), donc la tuile affiche un bloc gris
([MaxGramser/tessera-plugins#6](https://github.com/MaxGramser/tessera-plugins/issues/6)).

## Confidentialité

Le plugin ne fait aucune requête réseau. Il lit seulement ce que Home Assistant sait déjà de l'entité choisie, et
appelle les actions du lecteur citées plus haut quand vous appuyez sur une tuile. L'image de modèle de la pochette
s'exécute dans Home Assistant, pas dans le plugin.
