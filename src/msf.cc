#include <stdshit.h>
#include "dfsw/cFile.h"
#include "msf.h"

static const char szHdrMagic[0x2C] = "Microsoft C/C++ program database 2.00\r\n\x1a""JG\0";
static const char szBigHdrMagic[0x20] = "Microsoft C/C++ MSF 7.00\r\n\x1A""DS\0\0";

#define MAX_STREAM 65536
#define MAX_SIZE 1073741824

static inline
int ceilDiv(int x, int y) {
	return 1 + ((x - 1) / y); }

struct BIGMSF_HDR
{
  char FileMagic[0x20];      // 0x00
  u32 BlockSize;             // 0x20
  u32 FreeBlockMapBlock;     // 0x24
  u32 NumBlocks;             // 0x28
  u32 NumDirectoryBytes;     // 0x2C
  u32 Unknown;               // 0x30
  u32 BlockMapAddr;          // 0x34
};

struct MSF_HDR {
  char FileMagic[0x2C];      // 0x00
  u32 BlockSize;             // 0x2C
  u16 FreeBlockMapBlock;     // 0x30
  u16 NumBlocks;             // 0x32
  u32 NumDirectoryBytes;     // 0x34
  u32 Unknown;               // 0x38
};

struct MsfLoader
{
	CFile fp;
	byte* dir = NULL;
	int indexSize;

	byte* blocks;
	byte* blockEnd;

	union {	BIGMSF_HDR bHdr; MSF_HDR hdr; };
	~MsfLoader() { free(dir);  }

	void blockSeek(u32 iBlock) {
		if(__builtin_mul_overflow(bHdr.BlockSize, iBlock, &iBlock))
			iBlock = 0xFFFFFFFF;
		fp.seek(iBlock);
	}

	int blockRead(byte** dst, size_t size)
	{
		byte* tmp = (byte*)malloc(size);
		if(!tmp) return ENOMEM;
		SCOPE_EXIT(free_ref(dst, tmp));

		for(size_t i = 0; i < size; i += bHdr.BlockSize)
		{
			// read the block index
			byte* blockNext = blocks + indexSize;
			if(blockNext > blockEnd) return -1;
			u32 iBlock = (indexSize & 4) ? RI(blocks) : RW(blocks);
			blocks = blockNext; blockSeek(iBlock);

			// read the block
			size_t blockSize = min(size-i, bHdr.BlockSize);
			IFRET(fp._read(tmp+i, blockSize));
		}

		return 0;
	}

	ALWAYS_INLINE
	int load(MsfFile& self, cch* file)
	{
		int skipSize;

		// read the header
		IFRET(fp.open(file));
		IFRET(fp.read(&hdr));

		if(!memcmp(hdr.FileMagic, szHdrMagic, sizeof(szHdrMagic)))
		{
			bHdr.BlockSize = hdr.BlockSize;
			bHdr.NumDirectoryBytes = hdr.NumDirectoryBytes;
			bHdr.NumBlocks = hdr.NumBlocks;
			indexSize = 2; skipSize = 8;
		}

		ei(!memcmp(bHdr.FileMagic, szBigHdrMagic, sizeof(szBigHdrMagic)))
		{
			blockSeek(bHdr.BlockMapAddr);
			indexSize = 4; skipSize = 4;
		} else {
			return -1;
		}

		// validate header
		size_t fileSize = fp.getSize();
		if(!is_one_of(bHdr.BlockSize, 512, 1024, 2048, 4096)
		||(bHdr.NumDirectoryBytes < 4)
		||(bHdr.NumDirectoryBytes >= fileSize)
		||((fileSize/bHdr.BlockSize) != bHdr.NumBlocks))
			return -2;

		// read directory
		int nBlocks = ceilDiv(bHdr.NumDirectoryBytes, bHdr.BlockSize);
		IFRET(fp.mread(&dir, nBlocks*indexSize));
		blocks = dir; blockEnd = (byte*)-1;
		IFRET(blockRead(&dir, bHdr.NumDirectoryBytes));

		// validate directory
		u32 count = RU(dir);
		if(count > ((bHdr.NumDirectoryBytes-4)/skipSize))
			return -1;
		blocks = dir+(count*skipSize)+4;
		blockEnd = dir+bHdr.NumDirectoryBytes;

		// allocate file list
		self.data = Void(calloc(count, sizeof(xArray<byte>)));
		if(!self.data) return ENOMEM;
		self.len = count;

		// read file data
		byte* dirPos = dir+4;
		for(auto& xa : self)
		{
			u32 size = RU(dirPos);
			dirPos += skipSize;
			if(size >= fileSize) return -1;
			IFRET(blockRead(&xa.data, size));
			xa.len = size;
		}

		return 0;
	}
};

int MsfFile::load(cch* file)
{
	MsfLoader ls;
	return ls.load(*this, file);
}
