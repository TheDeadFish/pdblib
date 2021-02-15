#pragma once

static inline
size_t satAdd(size_t a, size_t b) {
	if(__builtin_add_overflow(a, b, &b))
		return -1; return b;
}

struct DataPos
{
	byte* data;
	size_t pos, end;
	
	DataPos(byte* d, size_t l) : data(d), end(l), pos(0) {}
	DataPos(xarray<byte> xa) : DataPos(xa.data, xa.len) {}
	
	size_t _rem() { return end-pos; }
	bool _chk(size_t size) { return _rem() >= size; }

	byte* _get(size_t size) {
		if(!_chk(size)) return NULL;
		byte* ret = data+pos; pos += size;
		if(!ret) __builtin_unreachable();
		return ret;
	}
	
	// template overloads
	template <class T=byte> T* get(size_t size=1) { return (T*)_get(sizeof(T)*size); }
	
	
	bool chk() { return pos < end; }
	
	
	void align(size_t size) {
		pos = (pos + size-1) & ~(size-1); }
	
	//template <class T> bool chk(size_t size=1) { return (T*)_chk(sizeof(T)*size); }
	
	
	
	
	
	
	// string processing
	char* getStr() {
		size_t curPos = pos;
		while(curPos < end) {
			byte ch = data[curPos]; curPos++;
			byte* ret = data+pos; pos = curPos;
			if(!ret) __builtin_unreachable();
			return (char*)ret;
		}
		
		return NULL;
	}
	
	char* getL8Str() {
		if(pos >= end) return NULL;
		return get<char>(data[pos]+1);
	}
	
	char* getL16Str() {
		if(pos >= (end-1)) return NULL;
		return get<char>(ref<u16>()+2);
	}
	
	// unchecked access
	byte* _ptr(size_t ofs) { return data+pos+ofs; }
	template <class T=byte> T* ptr(size_t ofs=0) { return (T*)_ptr(ofs*sizeof(T)); }
	template <class T=byte> T& ref(size_t ofs=0) { return *ptr<T>(ofs); }
	
};
