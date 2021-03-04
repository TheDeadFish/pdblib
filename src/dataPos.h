#pragma once

#if defined(__x86_64__)
	#define SYSTEM_MAX_ADDRESS_VALUE  0x7FFFFFFFFFFFLL;
#elif defined(_WIN32)
	#define SYSTEM_MAX_ADDRESS_VALUE 0xFFFF0000
#endif

static inline
size_t satAdd(size_t a, size_t b) {
	if(__builtin_add_overflow(a, b, &b))
		return -1; return b;
}


struct DataPos
{
	size_t pos, _end;

	DataPos(byte* d, size_t l) : pos(size_t(d)), _end(size_t(d+l)) {}
	DataPos(xarray<byte> xa) : DataPos(xa.data, xa.len) {}
	void setEnd(xarray<byte> xa) { _end = (size_t)xa.end(); }


	bool chk() { return pos < _end; }



	ALWAYS_INLINE
	bool _chk(size_t& newPos, size_t size)
	{
		//if((__builtin_constant_p(size))
		//&&(size < 65536)) newPos = pos + size;
		if(__builtin_add_overflow(pos, size, &newPos))
			return false;

		nothing();

		if(newPos > _end) return false;
		return true;
	}

	bool _chk(size_t size) {
		size_t newPos;
		return _chk(newPos, size);
	}

	ALWAYS_INLINE
	byte* _get(size_t size) {
		size_t newPos;
		if(!_chk(newPos, size)) return NULL;
		size_t ret = pos; pos += size;
		if(!ret) __builtin_unreachable();
		return (byte*)ret;
	}

	/*





	// template overloads






	void align(size_t size) {
		pos = (pos + size-1) & ~(size-1); }

	//template <class T> bool chk(size_t size=1) { return (T*)_chk(sizeof(T)*size); }


	void setEnd(size_t l) { len = l; }

	bool _chkSetEnd(size_t len, size_t size) {
		if(size > (len-pos)) return false;
		this->len = pos+size; return true; }
	bool _chkSetEnd(size_t size) { return _chkSetEnd(len, size); }


	//bool _chkEnd(size_t len, size_t size) { return (size <= (len-pos)); }



  */



	char* getL8Str() {
		if(pos >= _end) return NULL;
		return get<char>(ref<u8>()+1);
	}

	char* getL16Str() {
		if(pos >= (_end-1)) return NULL;
		return get<char>(ref<u16>()+2);
	}

	// unchecked access
	byte* _ptr(size_t ofs=0) { return (byte*)pos+ofs; }
	byte* _nptr(size_t size) { byte* tmp = (byte*)pos; pos += size; return tmp; }

	// special modes
	template <class T=byte> T* ptr(size_t ofs=0) { return (T*)_ptr(ofs*sizeof(T)); }
	template <class T=byte> T& ref(size_t ofs=0) { return *ptr<T>(ofs); }
	


	bool _chkSetEnd(size_t size) {
		if(!_chk(size)) return false;
		_end = pos+size; return true;
	}

	void align(size_t size) {
		pos = (pos + size-1) & ~(size-1); }


	ALWAYS_INLINE
	bool __pGet(size_t* p, size_t size) {



		size_t newPos;
		if(!_chk(newPos, size)) return false;
		*p = pos; pos = newPos; return true;
	}



	// string processing
	ALWAYS_INLINE
	char* getStr() {
		size_t curPos = pos;
		while(curPos < _end) {
			char ch = *(char*)curPos; curPos++;
			if(!ch) {
				size_t ret = pos; pos = curPos;
				if(!ret) __builtin_unreachable();
				return (char*)ret;
			}
		}

		return NULL;
	}


	ALWAYS_INLINE
	bool pGetStr(char*& p) { char* tmp = getStr();
		if(tmp) { p = tmp; return true; } return false; }


	// template overloads

	template <class T=byte> ALWAYS_INLINE
	bool _pGet(T*& p, size_t size) {
		return __pGet((size_t*)&p, size); }

	template <class T=byte> ALWAYS_INLINE
	bool pGet(T*& p, size_t size = 1) {
		return _pGet(p, size*sizeof(T)); }

	template <class T=byte> ALWAYS_INLINE
	T* get(size_t size=1) { return (T*)_get(sizeof(T)*size); }



	template <class T>
	bool pGetXa(xarray<T>& xa, size_t size) {
		byte* tmp = _get(size); if(!tmp) return false;
		xa.init((T*)tmp, size/sizeof(T)); return true; }



};
