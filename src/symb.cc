#include <stdshit.h>
#include "pdblib_.h"
#include "cvinfo.h"
#include "dataPos.h"


u8 value_list[] = {
	1, 2, 2, 4, 4, 4, 8, 10, 16,
	8, 8, 6, 8, 16, 20, 32, 0, 
	16, 16, 12, 8
};


static inline
bool value_size(DataPos& dataPos) {
	int value = dataPos.ref<s16>(-1);
	if(value < 0) {
		if(value > LF_UTF8STRING) return false;
		if(value == LF_UTF8STRING) return dataPos.getStr();
		if(value == LF_VARSTRING) return dataPos.getL16Str();
		return dataPos.get(value_list[value & 0x7FFF]);
	}
	return true;
}

enum {
	MODE_LPS = 1, // length prefixed string
	MODE_NTS = 2, // null terminated string
	
	MODE_VALUE = 4
};

struct SymbInfo {
	u16 rectyp; u8 reclen, mode;
};

#define X(rectyp, reclen, mode) \
	{ rectyp, sizeof(reclen)-sizeof(SYMTYPE), mode},

SymbInfo symbInfo[] = {
	X(S_CONSTANT_ST, CONSTSYM, MODE_LPS|MODE_VALUE)
	X(S_UDT_ST, UDTSYM, MODE_LPS)
	X(S_PUB32_ST, PUBSYM32, MODE_LPS)
	X(S_REGREL32_ST, REGREL32, MODE_LPS)
	X(S_GDATA32_ST, DATASYM32, MODE_LPS)
	X(S_PROCREF_ST, REFSYM, MODE_LPS)
	X(S_LDATA32_ST, DATASYM32, MODE_LPS)
	X(S_LPROCREF_ST, REFSYM, MODE_LPS)
	
	
	
};

SymbInfo* lookupSymb(SYMTYPE* rec) {
	for(auto& x : symbInfo) {
		if(x.rectyp == rec->rectyp)
			return &x;
	}
	
	return NULL;
}

bool pdb_symb_parse(PdbFile& pdb, xarray<byte> file)
{
	DataPos dataPos(file);
	while(1) 
	{
		/* get record */
		dataPos.align(4);
		if(!dataPos.chk()) break;
		SYMTYPE* rec = dataPos.get<SYMTYPE>();
		
		
		auto* symb = lookupSymb(rec);
		if(symb) {
		
			/* validate record body */
			if(!dataPos.get(symb->reclen))
				return false;
				
			/* validate value */
			if(symb->mode & MODE_VALUE) {
				if(!value_size(dataPos))
					return false;
			}
				
			/* validate strings */
			if(symb->mode & MODE_LPS) {
				if(!dataPos.getL8Str())	return false;
			}
			ei(symb->mode & MODE_NTS) {
				if(!dataPos.getStr()) return false;
			}
			
		} else {

			/* skip unsupported record */
			if(!dataPos.get(rec->reclen-2))
				return false;
		}
	}
	
	exit(1);

	return true;
}
