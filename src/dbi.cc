#include <stdshit.h>
#include "pdblib_.h"
#include "pdblib.h"
#include "dataPos.h"

using namespace PdbLib;

int PdbFile::_dbi_init()
{
	// Dbi Header
	DataPos dataPos(msf[3]);
	if(!dataPos.pGet<DbiHdr>(dbiHdr))
		return -1;

	// Module Info
	if(!dataPos._chkSetEnd(dbiHdr->ModInfoSize))
		return -2;
	while(dataPos.chk()) {
		auto& mi = modInfo.xnxalloc();
		if(!dataPos.pGet(mi.hdr)) return -2;
		if(!dataPos.pGetStr(mi.ModuleName)) return -2;
		if(!dataPos.pGetStr(mi.ObjFileName)) return -2;
		dataPos.align(4);
	}

	// Section Contribution
	dataPos.setEnd(msf[3]);
	if(!dataPos.pGetXa(sectCont, dbiHdr->SectionContributionSize))
		return -3;

	// skip unsuported
	if(!dataPos.get(dbiHdr->SectionMapSize)) return -4;
	if(!dataPos.get(dbiHdr->SourceInfoSize))  return -4;
	if(!dataPos.get(dbiHdr->TypeServerMapSize))  return -4;
	if(!dataPos.get(dbiHdr->ECSubstreamSize)) return -4;

	// optional Debug Header
	DbiOptHdr* _optHdr;
	if(!dataPos._pGet(_optHdr, dbiHdr->OptionalDbgHeaderSize))
		return -5;
	memncpy(&dbiOptHdr, _optHdr, sizeof(DbiOptHdr),
		dbiHdr->OptionalDbgHeaderSize, -1);

	printf("%X\n", dbiOptHdr.FPO);

	return 0;
}



#define X(name) \
   printf(#name": %d\n", index.name);


/*
void dbi_print(DbiIndex& index)
{
	X(FPO) X(Exception)	X(Fixup)
	X(Omap_To_Source)	X(Omap_From_Source)
	X(Section_Header)	X(Token_RID_Map)
	X(Xdata) X(Pdata) X(New_FPO)
	X(Org_Section_Header)

	// dbi header indexes
	X(GlobalStreamIndex)
	X(PublicStreamIndex)
	X(SymRecordStream)
	X(MFCTypeServerIndex)
}
*/