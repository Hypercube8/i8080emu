if test -f "output/i8080emu" ;
then
rm output/i8080emu
fi
gcc src/* -Iinclude -O3 -o output/i8080emu 
./output/i8080emu