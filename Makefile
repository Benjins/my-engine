engine: src/*.cpp
	$(CXX) -std=c++11 -Iheader/ext/freetype2 src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine

editor: src/*.cpp
	$(CXX) -std=c++11 -DEDITOR -Iheader/ext/freetype2 src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine-editor

testing: src/*.cpp test/testing.cpp
	$(CXX) -std=c++11 -DTESTING -g -fprofile-arcs -ftest-coverage -Iheader/ext/freetype2 src/*.cpp test/testing.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine-test
	./build/my-engine-test

engine_opt: src/*.cpp
	$(CXX) -std=c++11 -Iheader/ext/freetype2 -O2 src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine

engine_profile: src/*.cpp
	$(CXX) -std=c++11 -Iheader/ext/freetype2 -O2 -pg src/*.cpp lib/*.a -lGL -lGLU -lglut -lGLEW -lfreetype -lopenal -o ./build/my-engine_prof
	./build/my-engine_prof

engine_profile_analyse:
	gprof --flat-profile -b ./build/my-engine_prof gmon.out > analysis.txt

test_coverage: src/*.cpp
	gcov -o ./ --relative-only src/*.cpp > test-coverage.txt 

check: src/*.cpp header/int/*.h
	cppcheck --force --inline-suppr --template '{file},{line},{severity},{id},{message}' --enable=all --std=c++11 -I header/ext/freetype2 src/*.cpp 2> cpp-check-analysis.txt

docs: src/*.cpp header/int/*.h docs_pages/*.dox
	doxygen .doxyconfig