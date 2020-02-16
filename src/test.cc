#include <stdshit.h>
#include "pdblib.h"
const char progName[] = "test";

void contrib_print(PdbFile& pdb, int imod, u64 base)
{
	auto& mod = pdb.modules[imod];
	printf("@@ %s\n", mod.name1.data);
	
	for(auto& con : mod.con)
	{
		printf("%X: %X, %X\n", con.Section, con.Offset, con.Size);
		cch* name = con.get_name(pdb);
		printf("@ %s\n", name);
		int rva = con.map_rva(pdb);
		if(rva)	printf("%I64X\n", base+rva);
	}
}

int main()
{
	// load the pdb file
	PdbFile pdb;
	int ec = pdb.load("ntdll.pdb");
	if(ec) { printf("%d\n", ec); return 0; }
	
	// symbol lookup tests
	int rva = pdb.name_get_rva("LdrpShutdownInProgress",1);
	printf("%I64X\n", 0x78E50000LL+rva);
	rva = pdb.name_get_rva("RtlSetThreadPoolStartFunc",1);
	printf("%I64X\n", 0x78E50000LL+rva);	
	
	
	contrib_print(pdb, 19, 0x78E50000LL);

}