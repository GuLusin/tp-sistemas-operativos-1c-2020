#!usr/bin/sh

clear

cd Team

mv config-PruebaTeam-Parte1 config

echo ""
echo "Prueba 1, parte 1: config realizada."

cd Debug
./Team

echo ""
echo "Prueba 1, parte 1 realizada."

cd ..

mv config config-PruebaTeam-Parte1
echo ""
echo "Prueba 1, parte 1: config actualizada."