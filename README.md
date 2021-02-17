# Skin Detection using OpenCV 

This program can detect the skin in images.


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



