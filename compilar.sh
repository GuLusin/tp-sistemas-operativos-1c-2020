#!usr/bin/sh

clear

cd utils
cd Debug
make clean
make all
cd ..
cd ..

cd Broker
cd Debug
make clean
make all
cd ..
cd ..

cd Team
cd Debug
make clean
make all
cd ..
cd ..

cd Gamecard
cd Debug
make clean
make all
cd ..
cd ..

cd Gameboy
cd Debug
make clean
make all
cd ..
cd ..

echo ""
echo "Proyecto Compilado."
