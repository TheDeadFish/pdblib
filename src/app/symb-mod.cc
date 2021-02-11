#include <stdshit.h>
#include "pdblib.h"
const char progName[] = "test";

int main(int argc, char** argv)
{
	// load the pdb file
	PdbFile pdb;
	int ec = pdb.load(argv[1]);
	if(ec) { printf("%d\n", ec); return 0; }

	pdb.sortSymbByRva();

	for(auto& mod : pdb.modules) {
		printf("module: %s, %s\n",
			mod.name1.data, mod.name2.data);


		for(auto& sect : mod.con) {
		//	printf("  sect: %X, %X\n", sect.Size, sect.Characteristics);

		auto symbLst = pdb.rva_get_symb({sect.Section, sect.Offset}, sect.Size);
		for(auto& symb : symbLst) {
			printf("    symb: %s\n", symb.name.data);


		}

		}
















	}


}