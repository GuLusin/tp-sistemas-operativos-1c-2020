#!usr/bin/sh

clear

cd Broker

mv config-Prueba2-Compactacion-Parte1 config

echo ""
echo "Prueba 3, parte 1: config realizada."

cd Debug
./Broker

echo ""
echo "Prueba 3, parte 1 realizada."

cd ..

mv config config-Prueba2-Compactacion-Parte1
echo ""
echo "Prueba 3 parte 1: config actualizada."