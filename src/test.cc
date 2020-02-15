#include <stdshit.h>
#include "pdblib.h"
const char progName[] = "test";

int main()
{
	// load the pdb file
	PdbFile pdb;
	int ec = pdb.load("ntdll.pdb");
	if(ec) { printf("%d\n", ec); return 0; }
	
	// symbol lookup tests
	int rva = pdb.symb_get_rva("LdrpShutdownInProgress");
	printf("%I64X\n", 0x78E50000LL+rva);
	rva = pdb.symb_get_rva("RtlSetThreadPoolStartFunc");
	printf("%I64X\n", 0x78E50000LL+rva);	

}