#!usr/bin/sh

clear

cd Team

mv config-PruebaCompleta-Parte2-Team1 config

echo ""
echo "Prueba Final, parte 2: config realizada."

cd Debug
./Team

echo ""
echo "Prueba Final, parte 2 realizada."

cd ..

mv config config-PruebaCompleta-Parte2-Team1 
echo ""
echo "Prueba Final, parte 2: config actualizada."