#include <stdshit.h>
#include "pdblib.h"

u32 pdb_omap_get(xarray<Pdb_OMAP>& xa, u32 rva)
{
	Pdb_OMAP cm = {0,0};
	for(auto& x : xa) {
		if(x.from > rva) break; cm = x; } 
	if(cm.to == 0) return 0;
	return rva-cm.from+cm.to;
}

Pdb_Symb* pdb_symb_get(xarray<Pdb_Symb>& xa, cch* name)
{
	for(auto& x : xa) {
		if(!strcmp(x.name, name)) return &x; }
	return NULL;
}

int PdbFile::symb_get_rva(cch* name, bool org)
{
	return symb_get_rva(symb_get(name));
}

int PdbFile::symb_get_rva(Pdb_Symb* symb, bool org)
{
	if(!symb) return -1;
	DWORD rva = symb->src_rva(orgSects);
	if(!org) rva = pdb_omap_get(fromSrc, rva);
	return rva;
}