if test -f "output/i8080emu" ;
then
rm output/i8080emu
fi
gcc src/* -Iinclude -O3 -o output/i8080emu -pg -DDEBUG
./output/i8080emu