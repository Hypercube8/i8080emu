if test -f "output/i8080emu" ;
then
rm output/i8080emu
fi
gcc src/* -Iinclude -o output/i8080emu -DDEBUG
./output/i8080emu