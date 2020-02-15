#include <stdshit.h>
#include "pdblib_.h"

struct RecordHeader {
  uint16_t RecordLen;  // Record length, not including this 2 byte field.
  uint16_t RecordKind; // Record kind enum.
};

struct PublicSymb {
	RecordHeader rec;
	uint32_t flags;
	uint32_t offset;
	uint16_t section;
	char name[];
};

bool pdb_symb_parse(PdbFile& pdb, xarray<byte> file)
{
	auto pos = xRngPtr<byte>(file.data, file.len);
	while(pos.chk()) {
	
		// validate the record
		RecordHeader* rec = Void(pos.data);
		int len = rec->RecordLen+2;
		if((len < 4)||(pos.count() < len))
			return false;
			
		if(rec->RecordKind == 0x110E) {
			// validate the symbol
			PublicSymb* symb = Void(rec);
			char* name = ovfStrChk_(pos.data+len, symb->name);
			if(!name) return false;
			
			// insert the symbol
			pdb.symb.push_back(
				symb->section-1, symb->offset,
				symb->flags, xstrdup(name));
		}
		
		pos.data += len;
	}

	return true;
}
