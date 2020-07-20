#!usr/bin/sh

clear

cd Broker

mv config-PruebaCompleta-Parte1 config

echo ""
echo "Prueba Final, parte 1: config realizada."

cd Debug
./Broker

echo ""
echo "Prueba Final, parte 1 realizada."

cd ..

mv config config-PruebaCompleta-Parte1
echo ""
echo "Prueba Final, parte 1: config actualizada."