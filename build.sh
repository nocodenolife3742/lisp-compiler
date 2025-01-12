mkdir build
cd build
cmake ..
make
cd ..
cp build/lisp-compiler .
rm -rf build
chmod +x lisp-compiler
