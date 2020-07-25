#!usr/bin/sh

clear

cd Broker

mv config-Prueba4-BuddySystem-Parte1 config

echo ""
echo "Prueba 4, parte 1: config realizada."

cd Debug
./Broker

echo ""
echo "Prueba 4, parte 1 realizada."

cd ..

mv config config-Prueba4-BuddySystem-Parte1
echo ""
echo "Prueba 4 parte 1: config actualizada."