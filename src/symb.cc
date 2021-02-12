#include <stdshit.h>
#include "pdblib_.h"
#include "cvinfo.h"

bool pdb_symb_parse(PdbFile& pdb, xarray<byte> file)
{
	auto pos = xRngPtr<byte>(file.data, file.len);
	while(pos.chk()) {
	
		// validate the record
		SYMTYPE* rec = Void(pos.data);
		int len = rec->reclen+2;
		if((len < 4)||(pos.count() < len))
			return false;
			
		if(rec->rectyp == S_PUB32) {
			// validate the symbol
			PUBSYM32* symb = Void(rec);
			char* name = ovfStrChk_(pos.data+len, symb->name);
			if(!name) return false;
			
			// insert the symbol
			pdb.symb.push_back(
				symb->seg-1, symb->off,
				symb->pubsymflags.grfFlags, xstrdup(name));
		}
		
		pos.data += len;
	}

	return true;
}
