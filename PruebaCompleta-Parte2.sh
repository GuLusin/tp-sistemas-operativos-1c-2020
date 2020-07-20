#!usr/bin/sh

clear

cd Broker

mv config-PruebaCompleta-Parte2 config

echo ""
echo "Prueba Final, parte 2: config realizada."

cd Debug
./Broker

echo ""
echo "Prueba Final, parte 2 realizada."

cd ..

mv config config-PruebaCompleta-Parte2
echo ""
echo "Prueba Final, parte 2: config actualizada."