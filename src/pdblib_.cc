#include <stdshit.h>
#include "pdblib_.h"
#include "msf.h"
#include "dump.h"
//#define DEBUG_DUMP

#define ERRDEF(step, file) ((step|0x100)|(file<<16))

int PdbFile::load(cch* name)
{
	ZINIT;


	// load msf container
	IFRET(msf.load(name));
	if(msf.len < 4) return 0x100|1;
	
	// parse dbi stream
	if(_dbi_init())
		return ERRDEF(2, 3);
		
	// parse symbols
	if(dbiHdr->SymRecordStream != 0xFFFF) {
		if((!msf.chk(dbiHdr->SymRecordStream))
		||(!pdb_symb_parse(*this, msf[dbiHdr->SymRecordStream])))
			return ERRDEF(3, dbiHdr->SymRecordStream);

	if(msf.chk(dbiOptHdr.Fixup)) {
		fixup.initb(msf[dbiOptHdr.Fixup]);
#ifdef DEBUG_DUMP
		for(auto& f : fixup)
			PdbLib::dump_fixup(&f);
#endif
	}
	
	// mapping
	if(msf.chk(dbiOptHdr.Omap_To_Source))
		toSrc.initb(msf[dbiOptHdr.Omap_To_Source]);
	if(msf.chk(dbiOptHdr.Omap_From_Source))
		fromSrc.initb(msf[dbiOptHdr.Omap_From_Source]);

	// sections
	if(msf.chk(dbiOptHdr.Section_Header))
		peSects.initb(msf[dbiOptHdr.Omap_To_Source]);
	if(msf.chk(dbiOptHdr.Org_Section_Header))
		orgSects.initb(msf[dbiOptHdr.Omap_From_Source]);	

	return 0;
}
