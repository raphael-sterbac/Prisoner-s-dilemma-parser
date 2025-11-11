# Descriptif du projet

Le projet est découpé en plusieurs fichiers :

- Le fichier `langlex.l` qui contient le code flex pour l'analyse lexicale du 
programme en entrée.

- Le fichier `lang.y` qui contient le code bison pour l'analyse syntaxique, et donc
la création de l'AST (abstract syntax tree). Ce fichier contient aussi tout le code
relatif à la simulation des stratégies, ainsi que celle des populations d'agents.

# Compilation et execution du projet

Pour compiler le projet, il faut lancer la commande `make langlex.c && make lang`,
qui donnera un exécutable `lang`. Pour lancer le projet, il faut donc appeler :
`./lang test` où test est un fichier de test contenant un programme.

# Choix d'implémentation

Nous avons choisi d'implémenter l'AST de la manière suivante :

- Une structure `strats` correspondant à la liste chainée des stratégies
définies dans le programme.

- Une structure `stmt` correspondant aux instructions de code interne aux stratégies,
avec des champs `left` et `right` qui correspondent aux sous-blocs gauche et droite.

- Une structure `expr` qui correspond aux expressions, avec un champ `left` et
`right`, pour les sous-expressions gauches et droites.

Des fonctions `make_strat`, `make_expr`, ... permettent de construires de telles
structures.

Des fonctions d'affichages de l'AST sont incluses dans le fichier : `print_strats`,
`print_stmt`...

Ensuite, la traduction de l'AST vers le code original est effectuée par les fonctions
`translate_prog`, `translate_strats`... La transformation est bien quasi-involutive.

Pour la priorité des opérateurs, on a choisi de donner une priorité droite
pour le `IF` et le `ELSE`, avec une priorité supérieure au `ELSE`. Cela permet
donc de traiter correctement le cas du "dangling else".

## Niveau 2

Pour le niveau 2, nous avons implémenté des fonctions `eval_term` et `execute`
qui interprètent une expression et exécutent le code des statements. Ainsi, dans 
la fonction `step`, on prend deux agents et on simule une partie entre-eux, en
attribuant les points en fonction de leur décision.

Le code du niveau 2 est contenu dans le fichier `niveau2.y`. Il est possible que
certains bugs aient été réglé à postériori dans le niveau 3, mais cela donne
une idée du travail fourni pour le niveau 2.

## Niveau 3

Pour le niveau 3, nous avons écrit une fonction `meet` qui organise les rencontres
entre les agents, et affiche les infomations dans l'entrée standard.

Des fonctions auxiliaires telles que `distrib_strat`, `ajt_agt`, `copy_agt`...
implémentent les fonctionalités de base pour la rencontre des agents.

Afin de stocker la population d'agents, qui peut croitre très rapidement en fonction
des conditions initiales, nous avons choisi un tableau adaptatif. Nous commençons par
alouer assez de place pour stocker le nombre d'agents initial, et lorsque l'on dépasse
la taille du tableau, on le réalloue avec une taille double. 
Lorsqu'un agent au milieu du tableau meurt, on le permute avec le dernier agent du tableau
afin de ne pas avoir des "trous" dans notre tableau. Ceci permet une exécution rapide et efficace
de la simulation (pouvant aller jusqu'à 100 milions d'agents en 2 min).

Pour afficher les résultats, on peut rediriger l'entrée standard dans un fichier log :
`./lang > log`, et lire ensuite les résultats tour par tour dans le fichier. Néanmoins,
pour des raisons de lisibilité, un graphique peut être généré avec un script python
utilisant matplotlib, contenu dans le fichier `graph.py`.

On peut donc l'exécuter avec `python3 graph.py`, ou bien faire un batch de simulation qui
affiche les valeurs moyennes sur plusieurs exécutions avec `python3 batch.py`.

Des exemples de graphiques d'exécution sont présents dans le dossier du projet (`Figure_1.png`,...).
