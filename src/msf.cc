#include <stdshit.h>

const char MsfMagic[] = "Microsoft C/C++ MSF 7.00\r\n\x1A""DS\0\0";
#define MAX_STREAM 65536
#define MAX_SIZE 1073741824

static inline
int ceilDiv(int x, int y) {
	return 1 + ((x - 1) / y); }

struct SuperBlock {
  char FileMagic[32];        // 0x00
  u32 BlockSize;             // 0x20        
  u32 FreeBlockMapBlock;     // 0x24
  u32 NumBlocks;             // 0x28
  u32 NumDirectoryBytes;     // 0x2C
  u32 Unknown;               // 0x30
  u32 BlockMapAddr;          // 0x34
	
	bool chk1(u32 size) {	
		return !((size < sizeof(SuperBlock))
		||memcmp(FileMagic, MsfMagic, 32)); }
		
	bool chk2(u32 size) {
		return (is_one_of(BlockSize, 512, 1024, 2048, 4096)
		&&(size/BlockSize == NumBlocks)
		&&(int(NumDirectoryBytes) >= 4) && !(NumDirectoryBytes & 3)
		&&(BlockMapAddr <= NumBlocks-ceilDiv(
			NumDirectoryBytes, BlockSize*BlockSize/4)));
	}

	bool chkBlock(u32 block) { return block < NumBlocks; }
	Void getBlock(u32 block) { return Void(this, BlockSize*block); }
	u32 dirGet(u32 index);
};

u32 SuperBlock::dirGet(u32 index)
{
	index *= 4;
	if(index >= NumDirectoryBytes) return -1;
	int* blkMap = getBlock(BlockMapAddr);
	int iBlk = blkMap[index / BlockSize];
	if(!chkBlock(iBlk)) return -1;
	int* blk = getBlock(iBlk);
	return RI(blk, index % BlockSize);
}

struct MsfFile : 
	xArray<xArray<byte>>
{
	int load(cch* file);
};

int MsfFile::load(cch* file)
{
	// load the file
	xArray<byte> data = loadFile(file);
	if(!data) return 1;
	
	// check the header
	SuperBlock* sb = Void(data.data);
	if(!sb->chk1(data.len)) return 2;
	if(!sb->chk2(data.len)) return 3;
	
	// get strem count
	u32 count = sb->dirGet(0);
	if(count > MAX_STREAM)
		return 3;
		
	u32 pos = count;	
	for(int i = 1; i <= count; i++)
	{
		// alllocate stream
		auto& s = xnxcalloc();
		s.len = sb->dirGet(i);
		if(s.len > MAX_SIZE) return 3;
		s.data = malloc(s.len);
		if(!s.data) return 4;
		
		// read stream data
		byte* dst = s.data; int size = s.len;
		while(size > 0) {
			u32 iBlk = sb->dirGet(++pos);
			if(!sb->chkBlock(iBlk)) return 3;
			byte* src = sb->getBlock(iBlk);			
			memcpy(dst, src, min(size, sb->BlockSize));
			dst += sb->BlockSize; size -= sb->BlockSize;
		}
	}
	
	return 0;
}
