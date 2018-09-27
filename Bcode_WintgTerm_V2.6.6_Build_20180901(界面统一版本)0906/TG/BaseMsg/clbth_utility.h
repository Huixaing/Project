
#ifndef __CLBTH_UTILITY_H__
#define __CLBTH_UTILITY_H__

#include "clbth_config.h"

CLBTH_NAMESPACE_BEGIN


/////////////////////////////////////////////////////////////////////
// Plex used to alloc same size memory
/////////////////////////////////////////////////////////////////////
struct Plex     // warning variable length structure
{
	Plex* pNext;
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static Plex* PASCAL Create(Plex** head, UINT nMax, UINT cbElement);
			// like 'calloc' but no zero fill
			// may throw memory exceptions

	void FreeDataChain();       // free this one and links
};
/////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////
// class MapWrodToPtr declaration
/////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS MapWordToPtr
{
public:
	struct AutoInsertItem
	{
		AutoInsertItem(WORD nID, void *creator, MapWordToPtr& map)
		{
			ASSERT(NULL == map.GetAt(nID));
			map.SetAt(nID, creator);
		}
	};
protected:
	// Association
	struct Assoc
	{
		Assoc* pNext;

		WORD key;
		void* value;
	};

public:

// Construction
	MapWordToPtr(int nBlockSize = 10);

// Attributes
	// number of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(WORD key, void** rValue) const;
	BOOL LookupKey(const WORD& key) const;

// Operations
	// Lookup and add if not there
#if defined(__GNUC__) || defined(_MSC_VER)
	void*& operator[](const WORD& key);
#endif
	// add a new (key, value) pair
	void SetAt(const WORD& key, void* newValue);
	void* GetAt(const WORD& key);

	// removing existing (key, ?) pair
	BOOL RemoveKey(WORD key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSITION GetStartPosition() const;
	void GetNextAssoc(POSITION& rNextPosition, WORD& rKey, void** rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(WORD key) const;

// Implementation
protected:
	Assoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	Assoc* m_pFreeList;
	struct Plex* m_pBlocks;
	int m_nBlockSize;

	Assoc* NewAssoc();
	void FreeAssoc(Assoc*);
	Assoc* GetAssocAt(WORD, UINT&) const;

public:
	~MapWordToPtr();
	void AssertValid() const;

public:
	// local typedefs for CTypedPtrMap class template
	typedef WORD BASE_KEY;
	typedef WORD BASE_ARG_KEY;
	typedef void* BASE_VALUE;
	typedef void* BASE_ARG_VALUE;
};
/////////////////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////////////////
// class Archive declaration
/////////////////////////////////////////////////////////////////////
class BaseMsg;

class AFX_EXT_CLASS Archive
{
public:
	// Operation System Memory Addressing traits
	// OS2 and MS Windows is LowByte in LowAddress (ie. LSB)
	//     such as  (0x01234)=0xABCD, (0x1234)=0xCD, (0x1235)=0xAB
	// AIX is LowByte in HighAddress (ie. MSB)
	//     such as  (0x01234)=0xABCD, (0x1234)=0xAB, (0x1235)=0xCD
	struct ExchangeBetweenLSBandMSB
	{
		enum SWAPMODE{
			bExchangeByte = 1,
			bExchangeBit = 2
		};

		ExchangeBetweenLSBandMSB(INT mode = 0)
			: m_nMode(mode)	{}

		void SetSwapMode(INT mode = 0)
		{
			m_nMode = mode;
		}

		// 字节交换
		void operator()(void *pVal, UINT size)
		{
			if( !(m_nMode & (bExchangeByte | bExchangeBit)) )
				return ;
			
			register char  *p = (char*)pVal;
			register char  *q = p + size -1;
			
			if(m_nMode & bExchangeBit)
			{
				while(p<=q)	(*this)(p++);
				p = (char*)pVal;
			}
			if( !(m_nMode & bExchangeByte) )
				return;

			while( p<q ) {
				*p = *p ^ *q;	*q = *p ^ *q;	*p = *q ^ *p;
				p++, q--;
			}
		}

		// 位交换
		void operator()(char *pVal)
		{
			if( !(m_nMode & bExchangeBit) ) return;

			register char  p = *pVal;
			register char  q = 0;
			register int i=0;
			do{
				q <<= 1;
				q |= p & 0x01;
				p >>= 1;
			} while(++i<8);
			*pVal = q;
		}

	private:
		INT m_nMode;
	};

public:
// Flag values
	enum SeekMode { CURRENT = 0, BEGIN = 1, END = 2 };
	enum Mode { store = 0, load = 1, bNoFlushOnDelete = 2, bNoByteSwap = 4 };

	Archive(UINT nMode, int nBufSize, void* lpBuf);
	Archive(UINT nMode = store, int initialCapacity = 256, int capacityIncrement = 32, int fPercentIncrement = 10);
	~Archive();

// Attributes
	BOOL isLoading() const;
	BOOL isStoring() const;
	void setMode(enum Mode mode);
	BOOL isBufferEmpty() const;

// Operations
	UINT read(void* lpBuf, UINT nMax);
	UINT write(const void* lpBuf, UINT nMax);
	UINT getCurrentPos();
	LONG seek(enum SeekMode mode, LONG  lOff  );
	BOOL eof();
	BYTE* getBuffer();
	BYTE* getCurrentPtr();
	INT  getCount();
	void shrink(INT nCount);
	INT  getBufSize();
	INT  getSurplusCount();
	UINT getSurplusBufSize();
	BOOL good();
private:
	BOOL isByteSwapping() const;
	BOOL ensureCapacityHelper(int minCapacity);
	
private:
	// 数字转换为字节流, nBytes指明字节数
	void NumberToByteStream(ULONG l, char nBytes);
	// 从字节流中抽取数字, nBytes指明字节数
	ULONG ByteStreamToNumber(char nBytes);
public:
	// insertion operations
	Archive& operator<<(BYTE by);
	Archive& operator<<(WORD w);
	Archive& operator<<(LONG l);
	Archive& operator<<(DWORD dw);
	Archive& operator<<(float f);
	Archive& operator<<(double d);
	Archive& WriteString(LPCSTR lpsz, size_t len);

	Archive& operator<<(int i);
	Archive& operator<<(short w);
	Archive& operator<<(char ch);
	Archive& operator<<(unsigned u);

	// extraction operations
	Archive& operator>>(BYTE& by);
	Archive& operator>>(WORD& w);
	Archive& operator>>(DWORD& dw);
	Archive& operator>>(LONG& l);
	Archive& operator>>(float& f);
	Archive& operator>>(double& d);

	Archive& operator>>(int& i);
	Archive& operator>>(short& w);
	Archive& operator>>(char& ch);
	Archive& operator>>(unsigned& u);
	Archive& ReadString(LPSTR lpsz, size_t& len);

	// object read/write
	BaseMsg* readObject(WORD szClassIDRequested);
	void writeObject(const BaseMsg* pBaseMsg);

// Implementation
private:
	// archive objects cannot be copied or assigned
	Archive(const Archive& arSrc);
	void operator=(const Archive& arSrc);

	BOOL m_nMode;
	int m_nBufSize;
	BYTE* m_lpBufCur;
	BYTE* m_lpBufMax;
	BYTE* m_lpBufStart;
	BOOL bNoFreeOnDestruct;
	BOOL bNoOverFlow;
	int  m_nCapacityIncrement;
	int  m_fPercentIncrement;

	ExchangeBetweenLSBandMSB  byteswap;



	// operations on direct memory buffer, for somebody's habit
private:
	static void NumberToByteStream(void** buf, ULONG l, char nBytes)
		{
			ASSERT(NULL != buf && NULL != *(BYTE**)buf
				&& nBytes <= sizeof(ULONG));
			while(nBytes-- > 0)
			{
				**(BYTE**)buf = (BYTE)l;// & 0xFF;
				(*(BYTE**)buf) ++;
				l = l >> 8;
			}
		}
	static ULONG ByteStreamToNumber(void** buf, char nBytes)
		{
			ASSERT(NULL != buf && NULL != *(BYTE**)buf
				&& nBytes <= sizeof(ULONG));
			ULONG num = 0;
			BYTE  *pTemp = (*(BYTE**)buf + nBytes) - 1;
			*(BYTE**)buf += nBytes;

			while(nBytes-- > 0)
			{
				num = num << 8;
				num += *pTemp--;
			}
			return num;
		}
public:
	static void Write(void** buf, BYTE by)
		{ NumberToByteStream(buf, by, sizeof(BYTE)); }
	static void Write(void** buf, WORD w)
		{ NumberToByteStream(buf, w, sizeof(WORD)); }
	static void Write(void** buf, LONG l)
		{ NumberToByteStream(buf, l, sizeof(LONG)); }
	static void Write(void** buf, DWORD dw)
		{ NumberToByteStream(buf, dw, sizeof(DWORD)); }
	static void Write(void** buf, float f);
	static void Write(void** buf, double d);

	static void Write(void** buf, int i)
		{ NumberToByteStream(buf, i, sizeof(int)); }
	static void Write(void** buf, short w)
		{ NumberToByteStream(buf, w, sizeof(short)); }
	static void Write(void** buf, char ch)
		{ NumberToByteStream(buf, ch, sizeof(char)); }
	static void Write(void** buf, unsigned u)
		{ NumberToByteStream(buf, u, sizeof(unsigned)); }

	// 纯内存写入
	static void Write(void** buf, void *p, size_t len);
	// 内定字符串写入
	static void WriteString(void** buf, LPCSTR lpsz);

	// extraction operations
	static void Read(void** buf, BYTE& by)
		{ by = (BYTE)ByteStreamToNumber(buf, sizeof(BYTE)); }
	static void Read(void** buf, WORD& w)
		{ w = (WORD)ByteStreamToNumber(buf, sizeof(WORD)); }
	static void Read(void** buf, DWORD& dw)
		{ dw = (DWORD)ByteStreamToNumber(buf, sizeof(DWORD)); }
	static void Read(void** buf, LONG& l)
		{ l = (LONG)ByteStreamToNumber(buf, sizeof(LONG)); }
	static void Read(void** buf, float& f);
	static void Read(void** buf, double& d);

	static void Read(void** buf, int& i)
		{ i = (int)ByteStreamToNumber(buf, sizeof(int)); }
	static void Read(void** buf, short& w)
		{ w = (short)ByteStreamToNumber(buf, sizeof(short)); }
	static void Read(void** buf, char& ch)
		{ ch = (char)ByteStreamToNumber(buf, sizeof(char)); }
	static void Read(void** buf, unsigned& u)
		{ u = (unsigned)ByteStreamToNumber(buf, sizeof(unsigned)); }

	// 纯内存读出
	static void Read(void** buf, void *p, size_t len);
	// 内定字符串读出
	static void ReadString(void** buf, LPSTR lpsz, size_t& len);

	// object read/write
	static BaseMsg* readObject(void** buf, WORD szClassIDRequested);
	static void writeObject(void** buf, const BaseMsg* pBaseMsg);
};
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////

CLBTH_NAMESPACE_END

#endif //#ifndef __CLBTH_UTILITY_H__
