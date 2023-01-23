# OM3D

# Branch, TP 1 + TP 2 + IBL Deferred

ATTENTION CETTE BRANCHE CONTIENT TOUT NOS CHANGEMENT (Sauf la transparance).
Nous avons développé le projet sur Windows. Nous n'avons pas verifié si ce dernier compile sur Linux (nous n'avons pas de PC avec linux), mais le projet devrait compiler.

## TP 1

### Frustum culling

Pour le frustum culling, nous avons implémenté cet méthode sur la struct
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/Camera.cpp#L107-L119
Elle retourne vrais pour toute position ce trouvant dans un rayon donné.

Ce qui nous permet de cull les objets comme ce-ci:
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/Scene.cpp#L70-L75

### Instanciating

Comme conseillé dans le TP. Nous groupons les objets par texture, la fonction `Scene::add_object` a été révisé comme ce-ci :
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/Scene.cpp#L40-L45
Nous collectons l'objet une seul fous dans une hashmap, puis par la suite seulement les matrices transforms des autres objets pour économiser de la mémoire.

Venu le moment de render nos objets, nous parcourons chaque instance de la scène. On crée un buffer pour enregister les matrices transforms de chaque objet à afficher, on itére ensuite sur les matrices transforms de chaque objet et on les culls contre le frustum.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/Scene.cpp#L61-L79

Le tout sera ensuite interprété par notre vertex shader :
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/shaders/basic.vert#L22-L28
L'ancienne variable uniform `model`, a été supprimé.

## TP 2

### G-Buffer

Comme expliqué dans le TP, nous effectuons plusieurs sortie de notre fragment shader `g_buffer.frag`.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/shaders/g_buffer.frag#L5-L6
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/shaders/g_buffer.frag#L28-L29

### Le multi render-target (MRT)

https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L192-L194
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L199
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L234-L240

### Matériaux

La fichier `lit.frag` a été modifié pour lire entrée nos différentes textures.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/shaders/lit.frag#L9-L11
Enfin nous avons ajouté un material, pour pouvoir transférer les textures de notre G-Buffer au fragment shader final.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L208-L216
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L243-L244
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L258

### Vue de debug

Nous avons ajoutés des defines dans notre fragment shader, afin d'ajouter les différents modes de rendu.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L201-L206
Le program est changé "on the fly" à partir de notre menu de debug.

https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L289-L319

### Shading

Au lieu de donner nos lumieres dans le rendu de la scène, nous passons ce buffer beaucoup plus tard.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L243-L258

### Recalculer la position

Afin d'obtenir les coordonées dans l'intervale [0; 1], nous passons la taille de l'écran au fragment shader.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L250-L256
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/shaders/lit.frag#L41

Ce qui nous permet ensuite de déprojeter la position, en utilisant la matrice de projetion inversé de la caméra, ainsi que la valeur du depth fournis par notre G-Buffer.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/shaders/lit.frag#L55

### IBL Deferred

Pour IBL, nous avons pris des images du site http://www.hdrlabs.com/sibl/archive.html

Le chargement des cubemaps s'effectue dans la fonction main :
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L162-L167

Nous avons rajouté un paramétre au constructeur de Texture, pour blurr les mipmaps avec un kernel gaussien.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/Texture.cpp#L43
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/Texture.cpp#L52-L133
Nous conseillons de laisser le paramétre à faux, sinon cela va entrainer un temps de lancement du programme tres lent (le floutage est entierement fait par le CPU).

Nous passons notre cube au differed fragment shader de cette maniere :
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/src/main.cpp#L213
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/shaders/lit.frag#L12

Dans le differed fragment shader, si la depth est à 0 (donc aucun objet) nous disons que c'est le ciel.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/shaders/lit.frag#L43-L51

Et enfin nous avons changé la lumiere ambiente par cette formule, pour obtenir l'apport de lumiere venant du ciel.
https://github.com/husakihor/OM3D/blob/cc8161d9e9f4b2bfcb6a1696f8497ab1b82b0618/TP/shaders/lit.frag#L57-L64

# Readme from Fork

EPITA course for 5th year students

Slides: https://docs.google.com/presentation/d/1qJhH0cmEMVQRhF37UOYDdw0ZKtd00_oQXW1dZrUbSL4/edit

TP1: https://hackmd.io/@G2s9zdzlSd-RfTlBoN_x2A/ryY2BSgVo

TP2&3: https://hackmd.io/@G2s9zdzlSd-RfTlBoN_x2A/Bkm_EOFHo


### How to build
Requirements: cmake 3.20 minimum, C++17, and OpenGL 4.5.
```bash
# At the project root
mkdir -p TP/build/debug
cd TP/build/debug
cmake ../..
make
```

### Contact
If you have a problem, please send a mail to
- alexandre.lamure@epita.fr
- gregoire.angerand@gmail.com
