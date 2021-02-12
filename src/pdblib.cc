#include <stdshit.h>
#include "pdblib.h"

u32 pdb_omap_get(xarray<Pdb_OMAP>& xa, u32 rva)
{
	if(xa.len == 0) 
		return rva;

	Pdb_OMAP cm = {0,0};
	for(auto& x : xa) {
		if(x.from > rva) break; cm = x; } 
	if(cm.to == 0) return 0;
	return rva-cm.from+cm.to;
}

Pdb_Symb* PdbFile::name_get_symb(cch* name)
{
	for(auto& x : symb) {
		if(!strcmp(x.name, name)) return &x; }
	return NULL;
}

int PdbFile::name_get_rva(cch* name, bool map)
{
	auto* symb = name_get_symb(name);
	if(!symb) return 0;
	int rva = symb->rva(*this);
	if(map) rva = rva_from_src(rva);
	return rva;
}

Pdb_Symb* PdbFile::rva_get_symb(int rva, bool map)
{
	if(map) rva = rva_to_src(rva);
	if(rva) {
		for(auto& x : symb) {
			if(x.rva(*this) == rva)
				return &x;
	}}

	return NULL;		
}

cch* PdbFile::rva_get_name(int rva, bool map)
{
	auto* symb = rva_get_symb(rva, map);
	if(!symb) return NULL;	
	return symb->name;
}

cch* Pdb_Contrib::get_name(PdbFile& pdb)
{
	return pdb.rva_get_name(rva(pdb));
}

static
int symbSortRvaFn(const Pdb_Symb a, const Pdb_Symb& b) {
	IFRET(a.iSect-b.iSect); return a.offset-b.offset;
}

void PdbFile::sortSymbByRva()
{
	qsort(symb, symbSortRvaFn);
}


xarray<Pdb_Symb> PdbFile::rva_get_symb(
	Pdb_SectOfs sectOfs, int32_t size)
{
	auto* beg = bsearch_lower(&sectOfs, symb, symbSortRvaFn);
	sectOfs.offset += size;
	auto* end = bsearch_lower(&sectOfs, symb, symbSortRvaFn);
	return {beg, end};
}
