#!usr/bin/sh

clear

cd Team

mv config-PruebaTeam-Parte4 config

echo ""
echo "Prueba 1, parte 4: config realizada."

cd Debug
./Team

echo ""
echo "Prueba 1, parte 4 realizada."

cd ..

mv config config-PruebaTeam-Parte4
echo ""
echo "Prueba 1, parte 4: config actualizada."
