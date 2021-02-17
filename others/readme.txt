Skin Detection using OpenCV 
This program can detect the skin in images.

## Dependencies:
- OpenCV
- CMake

## Creation of the database
    > base 
        > peau (Folder which contains the skin histograms)
        > non-peau (Folder which contains non-skin histograms)
        > test (Folder which contains the original images of our two types of histograms)


## Program execution:
- Compiling the program and generating the executable (in the terminal), this will create an executable named "skin_dection".
    $ make

- Start the program with the generated executable followed by the scale, the threshold and the name of the test image:    
    $ ./dection_peau (scale) (threshold) (image_name.jpg)
    
    e.g: $ ./dection_peau 32 0.4 image_name.jpg


*********************************************




Skin Detection using OpenCV 
Notre programme a pour fonction de prendre en entree une image et de retourner comme resultat les histogrammes peau et non-peau humaine de cette image source apres detection d'une existance de peau ou non-peau humaine, un masque de detection de l'image d'entree se trouvant dans notre base d'apprentissage.

## Les dependences:
- OpenCV


## Creation de la base de donnees
    > base
        > peau (Dossier contenant les histogrammes peau)
        > non-peau (Dossier contenant les hihistogrammes non peau)
        > test (Dossier contenant les images orignales de nos deux types d'histogrammes)


## Execution du programme:
- Compilation du programme et generation de l'executable (dans le terminal), cela creera un executable du nom "dection_peau"
    $ make

- Lancer l'executable generer suivi de l'echelle, le seuil et le nom de l'image de teste:    
    $ ./dection_peau (scale) (threshold) (image_name.jpg)
    
    Exple: $ ./dection_peau 32 0.4 image_name.jpg

