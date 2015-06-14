engine: src/*.cpp
	$(CXX) -std=c++11 -Iheader/ext/freetype2 src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine

editor: src/*.cpp
	$(CXX) -std=c++11 -DEDITOR -Iheader/ext/freetype2 src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -o ./build/my-engine-editor

testing: src/*.cpp test/testing.cpp
	$(CXX) -std=c++11 -DTESTING -Iheader/ext/freetype2 src/*.cpp test/testing.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -o ./build/my-engine-test

engine_opt: src/*.cpp
	$(CXX) -std=c++11 -Iheader/ext/freetype2 -O2 src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -o ./build/my-engine