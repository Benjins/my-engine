engine: src/*.cpp
	$(CXX) -std=c++11 -Iheader/ext/freetype2 src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine

editor: src/*.cpp
	$(CXX) -std=c++11 -DEDITOR -Iheader/ext/freetype2 src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine-editor

testing: src/*.cpp test/testing.cpp
	$(CXX) -std=c++11 -DTESTING -Iheader/ext/freetype2 src/*.cpp test/testing.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine-test

engine_opt: src/*.cpp
	$(CXX) -std=c++11 -Iheader/ext/freetype2 -O2 src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine

engine_profile:
	$(CXX) -std=c++11 -Iheader/ext/freetype2 -O2 -pg src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine_prof
	./build/my-engine_prof

engine_profile_analyse:
	gprof ./build/my-engine_prof gmon.out > analysis.txt

check:
	cppcheck --enable=all --std=c++11 -I header/ext/freetype2 src/*.cpp 2> cpp-check-analysis.txt

docs:
	doxygen .doxyconfig