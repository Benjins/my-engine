#include "../../header/int/SourceParser.h"

int main(int argc, char** argv){
	ParseSourceFiles(&argv[1], argc - 1);
	return 0;

}