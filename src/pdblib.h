#pragma once
#include "msf.h"
#include "dbi.h"

namespace PdbLib
{

struct ModInfo {
	DbiModInfo* hdr;
	char* ObjFileName;
};

struct Fixup {
	u16 Type;
	u16 Flags;
	u32 Rva;
	u32 RvaTarget;
};

};




struct PdbFile;

struct Pdb_OMAP {
	DWORD from, to; };
	
struct Pdb_SectOfs {
	DWORD iSect, offset; };

struct Pdb_Symb : Pdb_SectOfs
{
	DWORD flags; xstr name;
	
	DWORD src_rva(xarray<IMAGE_SECTION_HEADER>& srcSect) {
		return srcSect[iSect].VirtualAddress + offset; }
	DWORD rva(PdbFile& pdb);
	DWORD map_rva(PdbFile& pdb);
};



struct Pdb_Contrib
{
	int32_t Section;
	int32_t Offset;
	int32_t Size;
	uint32_t Characteristics;
	
	DWORD src_rva(xarray<IMAGE_SECTION_HEADER>& srcSect) {
		return srcSect[Section].VirtualAddress + Offset; }
	DWORD rva(PdbFile& pdb);
	DWORD map_rva(PdbFile& pdb);
	
	cch* get_name(PdbFile& pdb);
};

struct Pdb_Module
{
	xstr name1, name2;
	xArray<Pdb_Contrib> con;
};

// pdb data query
u32 pdb_omap_get(xarray<Pdb_OMAP>& xa, u32 rva);

struct PdbFile
{
	// dbi data
	PdbLib::DbiHdr* dbiHdr;
	xArray<PdbLib::ModInfo> modInfo;
	xarray<PdbLib::DbiSectContr> sectCont;
	PdbLib::DbiOptHdr dbiOptHdr;

	xarray<PdbLib::Fixup> fixup;







	MsfFile msf;

	

	xarray<IMAGE_SECTION_HEADER> orgSects;
	xarray<IMAGE_SECTION_HEADER> peSects;
	
	xarray<Pdb_OMAP> fromSrc;
	xarray<Pdb_OMAP> toSrc;
	
	
	
	
	xArray<Pdb_Symb> symb;
	
	xArray<Pdb_Module> modules;
	
	// symbol functions
	Pdb_Symb* name_get_symb(cch* name);
	int name_get_rva(cch* name, bool map=0);
	Pdb_Symb* rva_get_symb(int rva, bool map=0);
	cch* rva_get_name(int rva, bool map=0);
	
	//xarray<Pdb_Symb> rva_get_symb(int rva, int size, bool map=0);
	xarray<Pdb_Symb> rva_get_symb(Pdb_SectOfs sectOfs, int32_t size);
	
	// rva helpers
	int rva_from_src(int rva) { return pdb_omap_get(fromSrc, rva); }
	int rva_to_src(int rva) { return pdb_omap_get(toSrc, rva); }
	
	
	// sorting functions
	void sortSymbByRva();
	
	
	
	
	
	int load(cch* name);
private:
	int _dbi_init();
};

inline DWORD Pdb_Symb::rva(PdbFile& pdb) { return src_rva(pdb.orgSects); }
inline DWORD Pdb_Symb::map_rva(PdbFile& pdb) { return pdb.rva_from_src(rva(pdb)); }
inline DWORD Pdb_Contrib::rva(PdbFile& pdb) { return src_rva(pdb.orgSects); }
inline DWORD Pdb_Contrib::map_rva(PdbFile& pdb) { return pdb.rva_from_src(rva(pdb)); }
