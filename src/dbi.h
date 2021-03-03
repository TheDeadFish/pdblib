#pragma once
namespace PdbLib
{

struct DbiHdr {
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
};


struct DbiSectContr {
	uint16_t Section;
	char Padding1[2];
	int32_t Offset;
	int32_t Size;
	uint32_t Characteristics;
	uint16_t ModuleIndex;
	char Padding2[2];
	uint32_t DataCrc;
	uint32_t RelocCrc;
};

struct DbiModInfo {
  uint32_t Unused1;	
	DbiSectContr SC;
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
};

struct DbiOptHdr {
	u16 FPO; 
  u16 Exception;
  u16 Fixup;
	u16 Omap_To_Source;	
  u16 Omap_From_Source;
	u16 Section_Header;	
  u16 Token_RID_Map;
	u16 Xdata; 
  u16 Pdata; 
  u16 New_FPO;
	u16 Org_Section_Header;
};

struct Dbi {
	DbiHdr* hdr;
	
	
	struct ModInfo {
		DbiModInfo* hdr;
		char* ModuleName;
		char* ObjFileName;
	};
	
	xArray<ModInfo> modInfo;
	xarray<DbiSectContr> sectCont;
  
  
  DWORD SCversion;
  
  
  DbiOptHdr optHdr;
  
  
  

	
	
	
	
	
	int init(xarray<byte> xa, int nFiles);
};

}
