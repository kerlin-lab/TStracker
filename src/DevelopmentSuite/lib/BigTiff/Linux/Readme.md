##How to build this library
- cd into this folder and run make
- If you encounter error regarding some tiff-4.1 folder missing ,etc, rename the folder tiff-4.0 to tiff-4.1
- If you encounter error regarding some \*.h files, this means your include directory is not pointing to the right place
	- Open file ./tiff-4.1/libtiff/Makefile and change the line 47 (the variable INCLDIR) to point to the directory that contains your libz.h and libjpeg.h
