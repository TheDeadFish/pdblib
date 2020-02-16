#pragma once

struct Pdb_OMAP {
	DWORD from, to; };
	
struct Pdb_Sect {
	DWORD addr, size, flags;
	char name[8];
};


struct Pdb_Symb {
	DWORD iSect, offset;
	DWORD flags; xstr name;
	
	DWORD src_rva(xarray<Pdb_Sect>& srcSect) {
		return srcSect[iSect].addr + offset; }
};



struct Pdb_Contrib
{
	uint16_t ModuleIndex;
	uint16_t Section;
	int32_t Offset;
	int32_t Size;
	uint32_t Characteristics;
};

struct Pdb_Module
{
	xstr name1, name2;
	xArray<Pdb_Contrib> con;
};

// pdb data query
u32 pdb_omap_get(xarray<Pdb_OMAP>& xa, u32 rva);
Pdb_Symb* pdb_symb_get(xarray<Pdb_Symb>& xa, cch* name);

struct PdbFile
{
	xArray<Pdb_Sect> orgSects;
	xArray<Pdb_Sect> peSects;
	
	xArray<Pdb_OMAP> fromSrc;
	xArray<Pdb_OMAP> toSrc;
	xArray<Pdb_Symb> symb;
	
	xArray<Pdb_Module> modules;
	
	
	Pdb_Symb* symb_get(cch* name) {
		return pdb_symb_get(symb, name); }
	int symb_get_rva(cch* name, bool org=0);
	int symb_get_rva(Pdb_Symb* symb, bool org=0);
	
	
	
	
	
	int load(cch* name);
};



