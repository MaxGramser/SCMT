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
  fort, plus fort) ou les deux. Ils demandent une tuile d'au moins deux lignes.

### Autres options de cette tuile

- **Position du texte :** titre, artiste et album au milieu (par défaut), en haut ou en bas à côté de la pochette. En
  haut ou en bas, le texte s'aligne sur le bord supérieur ou inférieur de la pochette.
- **Grand affichage :** un affichage semi-transparent sur toute la tuile, pour le volume, pour la pause ou pour les
  deux. Désactivé par défaut.
  - Volume : pendant trois secondes après un changement de volume, le nombre et un haut-parleur. Avec l'option « Muet »,
    un appui dessus coupe le son.
  - Pause : tant que le lecteur est en pause, le symbole de pause.
- **Avancer en touchant la barre :** un appui sur la barre de progression saute à cet endroit du morceau. Activé par
  défaut.
- **Muet :** un bouton muet parmi les boutons ; un appui sur l'affichage du volume active ou coupe le muet.
- **Barre de volume :** une barre verticale au bord de la tuile, du côté opposé à la pochette (à droite, ou à gauche
  quand la pochette est à droite) ; un appui dessus règle le volume à cette hauteur. Demande deux colonnes.
- **Nom de l'album :** l'album sur une troisième ligne, quand la tuile est assez haute.
- **Adresse de la pochette :** une adresse fixe d'un fichier de pochette dans Home Assistant, affichée à la place de
  l'image du lecteur (voir « La pochette » plus bas).

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

## La pochette

Par défaut, la pochette est l'image que Home Assistant possède lui-même pour le lecteur (son `entity_picture`) ; le
plugin ne charge rien depuis Internet.

Une tuile ne peut afficher que des images venant de `/api/image/serve/`, `/api/image_proxy/` ou `/local/`, et les
attributs texte sont coupés à 48 octets. La pochette de certains lecteurs (par exemple celui de HASS.Agent,
`/api/media_player_proxy/...`) est refusée pour cette raison.

**Remarque :** l'option « Adresse de la pochette » avec l'automatisation ci-dessous était prévue pour ces lecteurs, mais
elle ne fonctionne pas avec l'application Tessera actuelle (0.4.88). L'application récupère une adresse sous `/local/`
via le Supervisor (`http://supervisor/core/local/...`), qui la refuse (403, dans le journal de l'application « no
picture from /local/cover.jpg »), donc la tuile affiche un bloc gris au lieu de la pochette, quel que soit le format du
fichier. Les deux restent dans le plugin jusqu'à ce que Tessera récupère `/local/` autrement ou autorise la pochette
propre au lecteur ([MaxGramser/tessera-plugins#6](https://github.com/MaxGramser/tessera-plugins/issues/6)).

### Automatisation de la pochette (expérimental)

L'automatisation enregistre la pochette actuelle du lecteur comme fichier dans le dossier `www` de Home Assistant. La
tuile affiche alors ce fichier. Le côté Home Assistant fonctionne ; que votre écran affiche le fichier dépend de votre
version de Tessera, donc c'est expérimental. Le même code se trouve dans `examples/cover_automation.yaml`.

1. Créez le dossier `/config/www/` s'il n'existe pas et redémarrez Home Assistant une fois.
2. Ajoutez la commande à votre `configuration.yaml` (puis rechargez les commandes shell ou redémarrez). Il n'y a pas de
   `&&` volontairement : Home Assistant exécute les commandes contenant `{{ }}` sans shell.

```yaml
shell_command:
  cover_fetch: >-
    curl -sSfkL --connect-timeout 5 --max-time 20 -o /config/www/cover.jpg "{{ url }}"
```

3. Créez l'automatisation. Remplacez `media_player.your_player` par votre lecteur et `YOUR_HOME_ASSISTANT` par
   l'adresse sous laquelle vous ouvrez Home Assistant, telle que l'ordinateur de Home Assistant la voit (par exemple
   `http://192.168.0.10:8123`, ou `https://ha.example.com`, l'option `-k` est déjà dans la commande). Elle réagit à un
   nouveau titre, attend deux secondes et récupère la pochette trois fois (après 2, 7 et 15 secondes), car la nouvelle
   pochette arrive souvent quelques secondes après le titre.

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

4. Dans les options de la tuile, saisissez `/local/cover.jpg?v={v}` comme « Adresse de la pochette ».

L'adresse est utilisée exactement comme saisie, sauf `{v}` : le plugin le remplace par un nombre qui change à chaque
titre, pour que l'écran redemande (Home Assistant ignore la partie `?v=...` et envoie le fichier). Trois secondes après
un nouveau titre, il redemande une fois, au cas où le fichier serait écrit un peu en retard. Une adresse sans `{v}` ne
change jamais : l'écran garde alors la première image chargée jusqu'à ce que l'adresse change.

Pour vérifier le fichier, ouvrez `http://YOUR_HOME_ASSISTANT/local/cover.jpg?x=1` dans une fenêtre de navigation privée
(changez le nombre à chaque fois, car le navigateur garde l'image).

## Confidentialité

Le plugin ne fait aucune requête réseau. Il lit seulement ce que Home Assistant sait déjà de l'entité choisie, et appelle
les actions du lecteur citées plus haut quand vous appuyez sur une tuile. L'automatisation de la pochette s'exécute dans
Home Assistant, pas dans le plugin.
