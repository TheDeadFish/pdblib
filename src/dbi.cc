#include <stdshit.h>
#include "pdblib_.h"

  struct SectionContribEntry {
    uint16_t Section;
    char Padding1[2];
    int32_t Offset;
    int32_t Size;
    uint32_t Characteristics;
    uint16_t ModuleIndex;
    char Padding2[2];
    uint32_t DataCrc;
    uint32_t RelocCrc;
  } ;


struct ModInfo {
  uint32_t Unused1;	
	SectionContribEntry SC;
  uint16_t Flags;
  uint16_t ModuleSymStream;
  uint32_t SymByteSize;
  uint32_t C11ByteSize;
  uint32_t C13ByteSize;
  uint16_t SourceFileCount;
  char Padding[2];
  uint32_t Unused2;
  uint32_t SourceFileNameIndex;
  uint32_t PdbFilePathNameIndex;
  char ModuleName[];
	
	
	
	DEF_RETPAIR(chk_t, ModInfo*, next, char*, ObjFileName);
	
	chk_t chk(void* end);
};


ModInfo::chk_t ModInfo::chk(void* end)
{
	// string validation loop
	char* pos = ModuleName;
	char* ObjFileName = 0;
	while(1) {
		if(pos >= (char*)end) return {0,0};
		if(*pos++ != '\0') continue;
		if(ObjFileName) break;
		ObjFileName = pos;
	}
	
	// return position aligned by 4
	pos = Void(this, ALIGN4(PTRDIFF(pos, this)));
	return {(ModInfo*)pos, ObjFileName};
}


struct DbiStreamHeader {
  int32_t VersionSignature;         // 0x00
  uint32_t VersionHeader;           // 0x04
  uint32_t Age;                     // 0x08
  uint16_t GlobalStreamIndex;       // 0x0C
  uint16_t BuildNumber;             // 0x0E
  uint16_t PublicStreamIndex;       // 0x10
  uint16_t PdbDllVersion;           // 0x12
  uint16_t SymRecordStream;         // 0x14
  uint16_t PdbDllRbld;              // 0x16
  int32_t ModInfoSize;              // 0x18
  int32_t SectionContributionSize;  // 0x1C
  int32_t SectionMapSize;           // 0x20
  int32_t SourceInfoSize;           // 0x24
  int32_t TypeServerMapSize;        // 0x28
  uint32_t MFCTypeServerIndex;      // 0x2C
  int32_t OptionalDbgHeaderSize;    // 0x30
  int32_t ECSubstreamSize;          // 0x34
  uint16_t Flags;                   // 0x38
  uint16_t Machine;                 // 0x3A
  uint32_t Padding;                 // 0x3C
	
	bool chk(u32 size) { return true; }
	
	xRngPtr<ModInfo> modInfo() {
		return {(ModInfo*)Void(this+1), 
			(ModInfo*)Void(this+1, ModInfoSize)};
	}
	
	xRngPtr<SectionContribEntry> conInfo(void* p) {
		return {(SectionContribEntry*)Void(p,4), 
			(SectionContribEntry*)Void(p, SectionContributionSize)};
	}

	
};

bool dbi_parse(PdbFile& pdb, DbiIndex& index, xarray<byte> file)
{
	DbiStreamHeader* dbi = Void(file.data);
	if(!dbi->chk(file.len)) return false;
	
	// Module Info
	auto modLst = dbi->modInfo();
	while(modLst.chk()) {
		auto chk = modLst->chk(modLst.end());
		if(!chk) return false;
		pdb.modules.push_back(
			xstrdup(modLst->ModuleName),
			xstrdup(chk.ObjFileName));
		modLst.data = chk.next;
	}
	
	//Section Contribution
	auto conLst = dbi->conInfo(modLst.data);
	for(;conLst.chk(); conLst.fi()) {
		pdb.modules[conLst->ModuleIndex].con.push_back(
			conLst->Section-1, conLst->Offset, 
			conLst->Size, conLst->Characteristics);
	}
	
	// unsupported stuff
	char* pos = Void(conLst.data);
	pos += dbi->SectionMapSize;
	pos += dbi->SourceInfoSize;
	pos += dbi->TypeServerMapSize;
	pos += dbi->ECSubstreamSize;
	
	// stream indexes
	memset(&index, 0, sizeof(DbiIndex));
	index.GlobalStreamIndex = dbi->GlobalStreamIndex;
	index.PublicStreamIndex = dbi->PublicStreamIndex;
	index.SymRecordStream = dbi->SymRecordStream;
	index.MFCTypeServerIndex = dbi->MFCTypeServerIndex;
	memcpy(&index, pos, min(22,	dbi->OptionalDbgHeaderSize));
	
	return true;
}


#define X(name) \
   printf(#name": %d\n", index.name);


void dbi_print(DbiIndex& index)
{
	X(FPO) X(Exception)	X(Fixup)
	X(Omap_To_Source)	X(Omap_From_Source)
	X(Section_Header)	X(Token_RID_Map)
	X(Xdata) X(Pdata) X(New_FPO)
	X(Org_Section_Header)
	
	// dbi header indexes
	X(GlobalStreamIndex)
	X(PublicStreamIndex)
	X(SymRecordStream)
	X(MFCTypeServerIndex)
}
