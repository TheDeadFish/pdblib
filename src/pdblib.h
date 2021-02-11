#pragma once
struct PdbFile;

struct Pdb_OMAP {
	DWORD from, to; };
	
struct Pdb_Sect {
	DWORD addr, size, flags;
	char name[8];
};

struct Pdb_SectOfs {
	DWORD iSect, offset; };

struct Pdb_Symb : Pdb_SectOfs
{
	DWORD flags; xstr name;
	
	DWORD src_rva(xarray<Pdb_Sect>& srcSect) {
		return srcSect[iSect].addr + offset; }
	DWORD rva(PdbFile& pdb);
	DWORD map_rva(PdbFile& pdb);
};



struct Pdb_Contrib
{
	int32_t Section;
	int32_t Offset;
	int32_t Size;
	uint32_t Characteristics;
	
	DWORD src_rva(xarray<Pdb_Sect>& srcSect) {
		return srcSect[Section].addr + Offset; }
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
	xArray<Pdb_Sect> orgSects;
	xArray<Pdb_Sect> peSects;
	
	xArray<Pdb_OMAP> fromSrc;
	xArray<Pdb_OMAP> toSrc;
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
};

inline DWORD Pdb_Symb::rva(PdbFile& pdb) { return src_rva(pdb.orgSects); }
inline DWORD Pdb_Symb::map_rva(PdbFile& pdb) { return pdb.rva_from_src(rva(pdb)); }
inline DWORD Pdb_Contrib::rva(PdbFile& pdb) { return src_rva(pdb.orgSects); }
inline DWORD Pdb_Contrib::map_rva(PdbFile& pdb) { return pdb.rva_from_src(rva(pdb)); }
