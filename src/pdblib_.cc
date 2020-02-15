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

int PdbFile::load(cch* name)
{
	// load msf container
	MsfFile msf;
	IFRET(msf.load(name));
	if(msf.len < 4) return 3;
	
	// parse dbi stream
	DbiIndex index;
	if(!dbi_parse(*this, index, msf[3]))
		return 3;
		
	// parse symbols
	if(index.SymRecordStream != 0xFFFF) {
		if((!msf.chk(index.SymRecordStream))
		||(!pdb_symb_parse(*this, msf[index.SymRecordStream])))
			return 3;
	}
	
	// adress mapping
	if((!pdb_omap_load(toSrc, msf, index.Omap_To_Source))
	||(!pdb_omap_load(fromSrc, msf, index.Omap_From_Source)))
		return 3;
		
	// sections
	if((!pdb_sect_load(peSects, msf, index.Section_Header))
	||(!pdb_sect_load(orgSects, msf, index.Org_Section_Header)))
		return 3;	

	return 0;
}
