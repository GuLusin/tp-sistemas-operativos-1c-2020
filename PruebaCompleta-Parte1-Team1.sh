#!usr/bin/sh

clear

cd Team

mv config-PruebaCompleta-Parte1-Team1 config

echo ""
echo "Prueba Final, parte 1: config realizada."

cd Debug
./Team

echo ""
echo "Prueba Final, parte 1 realizada."

cd ..

mv config config-PruebaCompleta-Parte1-Team1 
echo ""
echo "Prueba Final, parte 1: config actualizada."