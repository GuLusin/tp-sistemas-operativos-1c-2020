#!usr/bin/sh

clear

cd Broker

mv config-Prueba3-BuddySystem-Parte2 config

echo ""
echo "Prueba 4, parte 2: config realizada."

cd Debug
./Broker

echo ""
echo "Prueba 4, parte 2 realizada."

cd ..

mv config config-Prueba3-BuddySystem-Parte2
echo ""
echo "Prueba 4 parte 2: config actualizada."