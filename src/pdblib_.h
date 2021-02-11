#pragma once
#include "pdblib.h"
#include "msf.h"

struct DbiIndex
{
	// optional header indexes
	u16 FPO; u16 Exception;	u16 Fixup;
	u16 Omap_To_Source;	u16 Omap_From_Source;
	u16 Section_Header;	u16 Token_RID_Map;
	u16 Xdata; u16 Pdata; u16 New_FPO;
	u16 Org_Section_Header;
	
	// dbi header indexes
	u16 GlobalStreamIndex;
	u16 PublicStreamIndex;
	u16 SymRecordStream;
	u16 MFCTypeServerIndex;
};

bool dbi_parse(PdbFile& pdb, DbiIndex& index,
	xarray<byte> file);
	
void dbi_print(DbiIndex& index);

bool pdb_symb_parse(PdbFile& pdb, xarray<byte> file);