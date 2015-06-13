engine: src/*.cpp
	g++ -std=c++11 -Iheader/ext/freetype2 src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -o ./build/my-engine