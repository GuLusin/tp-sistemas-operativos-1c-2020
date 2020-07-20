#!usr/bin/sh

clear

cd Team

mv config-PruebaTeam-Parte3 config

echo ""
echo "Prueba 1, parte 3: config realizada."

cd Debug
./Team

echo ""
echo "Prueba 1, parte 3 realizada."

cd ..

mv config config-PruebaTeam-Parte3
echo ""
echo "Prueba 1, parte 3: config actualizada."
