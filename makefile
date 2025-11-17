exe: ./src/main.cpp ./lib/raylib/libraylib.a
	g++ ./src/main.cpp ./lib/raylib/libraylib.a -o exe

./lib/raylib/libraylib.a:
	make -C ./lib/raylib/
