#!usr/bin/sh

clear

cd Team

mv config-PruebaTeam-Parte2 config

echo ""
echo "Prueba 1, parte 2: config realizada."

cd Debug
./Team

echo ""
echo "Prueba 1, parte 2 realizada."

cd ..

mv config config-PruebaTeam-Parte2
echo ""
echo "Prueba 1, parte 2: config actualizada."