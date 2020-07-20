#!usr/bin/sh

clear

cd Broker

mv config-Prueba2-Consolidacion-Parte2 config

echo ""
echo "Prueba 2, parte 2: config realizada."

cd Debug
./Broker

echo ""
echo "Prueba 2, parte 2 realizada."

cd ..

mv config config-Prueba2-Consolidacion-Parte2
echo ""
echo "Prueba 2, parte 2: config actualizada."