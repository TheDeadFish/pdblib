#include <stdshit.h>
#include "pdblib_.h"
#include "msf.h"


bool pdb_omap_load(xArray<Pdb_OMAP>& omap, MsfFile& msf, u16 index)
{
	if(!msf.chk(index)) return false;
	auto& data = msf[index];
	omap.xalloc(data.len / 8);
	memcpy(omap.data, data.data, data.len);
	return true;
}

bool pdb_sect_load(xArray<Pdb_Sect>& sect, MsfFile& msf, u16 index)
{
	if(!msf.chk(index)) return false;
	auto& data = msf[index];
	sect.xalloc(data.len / 40);
	IMAGE_SECTION_HEADER* ish = Void(data.data);
	for(auto& s : sect) {
		s.addr = ish->VirtualAddress;
		s.size = ish->Misc.VirtualSize;
		s.flags = ish->Characteristics;
		memcpy(s.name, ish->Name, 8);
		ish++;
	}
	
	return true;
}

#define ERRDEF(step, file) ((step|0x100)|(file<<16))

int PdbFile::load(cch* name)
{
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
	}
	
	// dbiOptHdr mapping
	if((dbiOptHdr.Omap_To_Source != 0xFFFF)
	&&(!pdb_omap_load(toSrc, msf, dbiOptHdr.Omap_To_Source)))
		return ERRDEF(4, dbiOptHdr.Omap_To_Source);
	if((dbiOptHdr.Omap_From_Source != 0xFFFF)		
	&&(!pdb_omap_load(fromSrc, msf, dbiOptHdr.Omap_From_Source)))
		return ERRDEF(5, dbiOptHdr.Omap_From_Source);
		
	// sections
	if(!pdb_sect_load(peSects, msf, dbiOptHdr.Section_Header))
		return ERRDEF(6, dbiOptHdr.Section_Header);
	if((dbiOptHdr.Org_Section_Header != 0xFFFF)
	&&(!pdb_sect_load(orgSects, msf, dbiOptHdr.Org_Section_Header)))
		return ERRDEF(7, dbiOptHdr.Org_Section_Header);

	return 0;
}
