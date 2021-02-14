#include <stdshit.h>
#include "pdblib_.h"
#include "cvinfo.h"

bool pdb_symb_parse(PdbFile& pdb, xarray<byte> file)
{
	auto pos = xRngPtr<byte>(file.data, file.len);
	while(1) {
	
		// validate the record
		pos.data += (file.data-pos.data)&3;
		if(!pos.chk()) break;
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
		
		// strings not in reclen
		if((rec->rectyp == S_PROCREF_ST)
		||(rec->rectyp == S_LPROCREF_ST)) {
			if(!pos.chk()) return false;
			len = *pos.data++;
			if(pos.count() < len) return false;
			pos.data += len;
		}
	}

	return true;
}
