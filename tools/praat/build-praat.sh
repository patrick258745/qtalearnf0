cd tools/praat/
tar -zxvf praat-6.0.29.tar.gz
cd praat-6.0.29/
cp makefiles/makefile.defs.linux.barren ./makefile.defs
make
cp praat ../../qtaf0/
cd ..
rm -r praat-6.0.29/
