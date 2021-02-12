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
	MsfFile msf;
	IFRET(msf.load(name));
	if(msf.len < 4) return 0x100|1;
	
	// parse dbi stream
	DbiIndex index;
	if(!dbi_parse(*this, index, msf[3]))
		return ERRDEF(2, 3);
	//dbi_print(index);
		
	// parse symbols
	if(index.SymRecordStream != 0xFFFF) {
		if((!msf.chk(index.SymRecordStream))
		||(!pdb_symb_parse(*this, msf[index.SymRecordStream])))
			return ERRDEF(3, index.SymRecordStream);
	}
	
	// adress mapping
	if((index.Omap_To_Source != 0xFFFF)
	&&(!pdb_omap_load(toSrc, msf, index.Omap_To_Source)))
		return ERRDEF(4, index.Omap_To_Source);
	if((index.Omap_From_Source != 0xFFFF)		
	&&(!pdb_omap_load(fromSrc, msf, index.Omap_From_Source)))
		return ERRDEF(5, index.Omap_From_Source);
		
	// sections
	if(!pdb_sect_load(peSects, msf, index.Section_Header))
		return ERRDEF(6, index.Section_Header);
	if((index.Org_Section_Header != 0xFFFF)
	&&(!pdb_sect_load(orgSects, msf, index.Org_Section_Header)))
		return ERRDEF(7, index.Org_Section_Header);

	return 0;
}
