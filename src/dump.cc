#include <stdshit.h>
#include "pdblib.h"
namespace PdbLib {

static int indent;
static FILE* file;
static void* base;

struct Field { int type; cch* name; };
#define FIELD(type, name) { TYPE_##type, #name },

enum {
	TYPE_NONE,
	TYPE_i32,
	TYPE_u32,
	TYPE_u16,
	TYPE_str,
};

static
void print_indent()
{
	for(int i = 0; i < indent; i++)
		printf(" ");
}

static
void print_offset(void* data, int len)
{
	if(base) {
		int pad = max(0, 68-(indent+len));
		printf("%*s// %02X", pad, "", PTRDIFF(data,base));
	}
}

static
void* print_field(Field* field, void* data)
{
	print_indent();

	int len; int sData;
	if(field->type == TYPE_str) {
		sData = strlen((char*)data)+1;
		len = printf("%s: %s", field->name, (char*)data);

	} else {

		int iData;
		if(field->type == TYPE_u16) {
			iData = RW(data); sData = 2;
		} else {
			iData = RI(data); sData = 4;
		}

		len = printf("%s: %X", field->name, iData);
	}

	print_offset(data, len);
	printf("\n");
	return data + sData;
}

static
void* print_field(int type, cch* name, void* data)
{
	Field f = {type, name};
	return print_field(&f, data);
}

static
void enter_scope(cch* name)
{
	if(name) {
		if(!indent) printf("\n");
		print_indent(); indent += 2;
		printf("%s:\n", name);
	}
}

static
void leave_scope()
{
	indent -= 2;
}

static
void* print_fields(Field* fields, void* data, cch* name)
{
	enter_scope(name);
	for(; fields->type; fields++)
		data = print_field(fields, data);
	if(name) leave_scope();
	return data;
}

static Field dbiHdr[] = {
	FIELD(i32, VersionSignature)
	FIELD(u32, VersionHeader)
	FIELD(u32, Age)
	FIELD(u16, GlobalStreamIndex)
	FIELD(u16, BuildNumber)
	FIELD(u16, PublicStreamIndex)
	FIELD(u16, PdbDllVersion)
	FIELD(u16, SymRecordStream)
	FIELD(u16, PdbDllRbld)
	FIELD(i32, ModInfoSize)
	FIELD(i32, SectionContributionSize)
	FIELD(i32, SectionMapSize)
	FIELD(i32, SourceInfoSize)
	FIELD(i32, TypeServerMapSize)
	FIELD(u32, MFCTypeServerIndex)
	FIELD(i32, OptionalDbgHeaderSize)
	FIELD(i32, ECSubstreamSize)
	FIELD(u16, Flags)
	FIELD(u16, Machine)
	FIELD(u32, Padding) {}
};

static Field sectCont[] = {
	FIELD(u16, Section)
	FIELD(u16, Padding1)
	FIELD(i32, Offset)
	FIELD(i32, Size)
	FIELD(u32, Characteristics)
	FIELD(u16, ModuleIndex)
	FIELD(u16, Padding2)
	FIELD(u32, DataCrc)
	FIELD(u32, RelocCrc) {}
};

static Field modInfo[] = {
	FIELD(u16, Flags)
	FIELD(u16, ModuleSymStream)
	FIELD(u32, SymByteSize)
	FIELD(u32, C11ByteSize)
	FIELD(u32, C13ByteSize)
	FIELD(u16, SourceFileCount)
	FIELD(u16, Padding)
	FIELD(u32, Unused2)
	FIELD(u32, SourceFileNameIndex)
	FIELD(u32, PdbFilePathNameIndex)
	FIELD(str, ModuleName)
	FIELD(str, ObjFileName) {}
};

void dump_dbiHdr(void* data)
{
	base = data;
	print_fields(dbiHdr, data, "Dbi Header");
};

void dump_modInfo(void* data)
{
	enter_scope("DbiModInfo");
	data = print_field(TYPE_u32, "Unused1", data);
	data = print_fields(sectCont, data, "sectCont");
	data = print_fields(modInfo, data, NULL);
	leave_scope();
}


}
