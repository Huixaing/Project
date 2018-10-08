
/////////////////////////////////////////////////////////////////////////////
//
// Implementation of parmeterized Map
//
/////////////////////////////////////////////////////////////////////////////

#include "basemsg.h"
#include "clbth_utility.h"

CLBTH_NAMESPACE_BEGIN


/////////////////////////////////////////////////////////////////////
// Plex used to alloc same size memory
/////////////////////////////////////////////////////////////////////
Plex* PASCAL Plex::Create(Plex** pHead, UINT nMax, UINT cbElement)
{
	ASSERT(NULL != pHead);
	ASSERT(nMax > 0 && cbElement > 0);
	Plex* p = (Plex*) new BYTE[sizeof(Plex) + nMax * cbElement];
			// may throw exception
	p->pNext = *pHead;
	*pHead = p;  // change head (adds in reverse order for simplicity)
	return p;
}

void Plex::FreeDataChain()     // free this one and links
{
	Plex* p = this;
	while (p != NULL)
	{
		BYTE* bytes = (BYTE*) p;
		Plex* pNext = p->pNext;
		delete[] bytes;
		p = pNext;
	}
}
/////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////
// class MapWrodToPtr declaration
/////////////////////////////////////////////////////////////////////
MapWordToPtr::MapWordToPtr(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

inline UINT MapWordToPtr::HashKey(WORD key) const
{
	// default identity hash - works for most primitive values
	return ((UINT)(void*)(DWORD)key) >> 4;
}

void MapWordToPtr::InitHashTable(UINT nHashSize, BOOL bAllocNow)
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	AssertValid();
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new Assoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(Assoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

void MapWordToPtr::RemoveAll()
{
	AssertValid();

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

MapWordToPtr::~MapWordToPtr()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Assoc helpers
// same as CList implementation except we store CAssoc's not CNode's
//    and CAssoc's are singly linked all the time
MapWordToPtr::Assoc*  MapWordToPtr::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		Plex* newBlock = Plex::Create(&m_pBlocks, m_nBlockSize, sizeof(Assoc));
		// chain them into free list
		Assoc* pAssoc = (Assoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	Assoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow


	pAssoc->key = 0;
	pAssoc->value = 0;

	return pAssoc;
}

void MapWordToPtr::FreeAssoc(Assoc* pAssoc)
{
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

MapWordToPtr::Assoc*
MapWordToPtr::GetAssocAt(WORD key, UINT& nHash) const
// find association (or return NULL)
{
	nHash = HashKey(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	Assoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->key == key)
			return pAssoc;
	}
	return NULL;
}



/////////////////////////////////////////////////////////////////////////////

BOOL MapWordToPtr::Lookup(WORD key, void** rValue) const
{
	ASSERT(NULL != rValue);
	AssertValid();

	UINT nHash;
	Assoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	*rValue = pAssoc->value;
	return TRUE;
}

BOOL MapWordToPtr::LookupKey(const WORD& key) const
{
	AssertValid();

	UINT nHash;
	Assoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	return TRUE;
}

#if defined(__GNUC__) || defined(_MSC_VER)
void*& MapWordToPtr::operator[](const WORD& key)
{
	AssertValid();

	UINT nHash;
	Assoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();

		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}
#endif

void MapWordToPtr::SetAt(const WORD& key, void* newValue)
{
#if defined(__GNUC__) || defined(_MSC_VER)
	(*this)[key] = newValue;
#else
	AssertValid();

	UINT nHash;
	Assoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();

		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	pAssoc->value = newValue;
#endif
}

void* MapWordToPtr::GetAt(const WORD& key)
{
#if defined(__GNUC__) || defined(_MSC_VER)
	return (*this)[key];
#else
	AssertValid();

	UINT nHash;
	Assoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();

		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
#endif
}

BOOL MapWordToPtr::RemoveKey(WORD key)
// remove key - return TRUE if removed
{
	AssertValid();

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	Assoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey(key) % m_nHashTableSize];

	Assoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->key == key)
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}


/////////////////////////////////////////////////////////////////////////////
// Iterating

void MapWordToPtr::GetNextAssoc(POSITION& rNextPosition,
	WORD& rKey, void** rValue) const
{
	ASSERT(NULL != rValue);
	AssertValid();
	ASSERT(m_pHashTable != NULL);  // never call on empty map

	Assoc* pAssocRet = (Assoc*)rNextPosition;
	ASSERT(pAssocRet != NULL);

	if (pAssocRet == (Assoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
		ASSERT(pAssocRet != NULL);  // must find something
	}

	// find next association
	//ASSERT(AfxIsValidAddress(pAssocRet, sizeof(Assoc)));
	Assoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket

		for (UINT nBucket = (HashKey(pAssocRet->key) % m_nHashTableSize) + 1;

		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	*rValue = pAssocRet->value;
}


/////////////////////////////////////////////////////////////////////////////
// Diagnostics

void MapWordToPtr::AssertValid() const
{
//	CObject::AssertValid();

	ASSERT(m_nHashTableSize > 0);
	ASSERT(m_nCount == 0 || m_pHashTable != NULL);
		// non-empty map should have hash table
}

int MapWordToPtr::GetCount() const
	{ return m_nCount; }
BOOL MapWordToPtr::IsEmpty() const
	{ return m_nCount == 0; }
POSITION MapWordToPtr::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
UINT MapWordToPtr::GetHashTableSize() const
	{ return m_nHashTableSize; }
/////////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////////////////
// class Archive Implementation 
/////////////////////////////////////////////////////////////////////

// minimum buffer size
//enum { nBufSizeMin = 128 };
//enum { nGrowSize = 64 };
//enum { nHashSize = 137 };
//enum { nBlockSize = 16 };

////////////////////////////////////////////////////////////////////////////
Archive::Archive(UINT nMode, int initialCapacity, int capacityIncrement, int fPercentIncrement)
{
	m_nMode = nMode;
	if(m_nMode & bNoByteSwap)
		byteswap.SetSwapMode(ExchangeBetweenLSBandMSB::bExchangeByte);
	
	m_nBufSize = initialCapacity;
	BYTE* p = new BYTE[m_nBufSize];
	ASSERT(NULL != p);
	m_lpBufStart = m_lpBufCur = m_lpBufMax = p;

	m_nCapacityIncrement = capacityIncrement;
	m_fPercentIncrement = fPercentIncrement;

	bNoFreeOnDestruct = FALSE;
	bNoOverFlow = TRUE;
}

Archive::Archive(UINT nMode, int nBufSize, void* lpBuf)
{
	// initialize members not dependent on allocated buffer
	m_nMode = nMode;
	if(m_nMode & bNoByteSwap)
		byteswap.SetSwapMode(ExchangeBetweenLSBandMSB::bExchangeByte);
	ASSERT(NULL!=lpBuf && nBufSize != 0);
	bNoFreeOnDestruct = TRUE;
	bNoOverFlow = TRUE;

	m_nBufSize = nBufSize;
	m_lpBufStart = m_lpBufCur = (BYTE*)lpBuf;
	if(isLoading())
		m_lpBufMax = m_lpBufStart + m_nBufSize;
	else
		m_lpBufMax = m_lpBufStart;

	m_nCapacityIncrement = m_fPercentIncrement = 0;
}

Archive::~Archive()
{
	if(bNoFreeOnDestruct == FALSE)
	{
		ASSERT(m_lpBufStart != NULL);
		delete [] m_lpBufStart;
	}
}

BOOL Archive::ensureCapacityHelper(int minCapacity)
{
	ASSERT(isStoring());

	if(minCapacity <= m_nBufSize)
		return TRUE;
	if(bNoFreeOnDestruct == TRUE)
	{
		bNoOverFlow = FALSE;
		return FALSE;
		//ASSERT(FALSE);
	}
	
	UINT  nNewSize = minCapacity + (m_nBufSize * m_fPercentIncrement + 99)/ 100 + m_nCapacityIncrement;
	nNewSize = (nNewSize + m_nCapacityIncrement + 32 - 1) & ~(32 - 1);
	if(nNewSize == m_nBufSize)
		nNewSize *= 2;
	BYTE *p = new BYTE[nNewSize];
	if(NULL == p)
		return FALSE;
	
	m_lpBufMax = m_lpBufMax - m_lpBufStart + p;
	m_lpBufCur = m_lpBufCur - m_lpBufStart + p;
	memcpy(p, m_lpBufStart, m_nBufSize);
	m_nBufSize = nNewSize;
	delete [] m_lpBufStart;
	m_lpBufStart = p;
	return TRUE;
}

UINT Archive::read(void* lpBuf, UINT nMax)
{
	if (nMax == 0)
		return 0;
	ASSERT( lpBuf != NULL );

	UINT  nBytesCopy;
	if(m_lpBufCur + nMax <= m_lpBufMax)
		nBytesCopy = nMax;
	else
		nBytesCopy = m_lpBufMax - m_lpBufCur;

	memcpy(lpBuf, m_lpBufCur, nBytesCopy);
	m_lpBufCur += nBytesCopy;
	
	return nBytesCopy;
}

UINT Archive::write(const void* lpBuf, UINT nMax)
{
	if (nMax == 0)
		return 0;

	ASSERT(lpBuf != NULL
		&& m_lpBufStart != NULL
		&& m_lpBufCur != NULL
		&& m_lpBufMax != NULL);
	ASSERT(isStoring());

	if(!ensureCapacityHelper(m_lpBufCur - m_lpBufStart + nMax))
		return 0;

	memcpy(m_lpBufCur, lpBuf, nMax);
	m_lpBufCur += nMax;
	if(m_lpBufMax < m_lpBufCur)
		m_lpBufMax = m_lpBufCur;
	
	return nMax;
}


LONG Archive::seek(enum SeekMode mode, LONG  lOff  )
{
	switch(mode)
	{
	case BEGIN:
		m_lpBufCur = m_lpBufStart + lOff;
		break;
	case CURRENT:
		m_lpBufCur += lOff;
		break;
	case END:
		m_lpBufCur = m_lpBufMax + lOff;
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	LONG  nOffset = lOff;
	if(m_lpBufCur < m_lpBufStart)// 越头界
	{
		nOffset = lOff - (m_lpBufCur - m_lpBufStart);
		m_lpBufCur = m_lpBufStart;
	}
	else if(m_lpBufCur > m_lpBufMax)	// 越尾界
	{
		if(bNoFreeOnDestruct)	// Fixed Memory
		{
			if(m_lpBufCur < m_lpBufStart + m_nBufSize)
				m_lpBufMax = m_lpBufCur;
			else
			{
				nOffset = lOff - (m_lpBufCur - m_lpBufStart - m_nBufSize);
				m_lpBufMax = m_lpBufStart + m_nBufSize;
				m_lpBufCur = m_lpBufMax;
			}
		}
		else	// Auto Allocate Memory
		{
			nOffset = lOff - (m_lpBufCur - m_lpBufMax);
			m_lpBufCur = m_lpBufMax;
		}
	}
	return nOffset;

	//return isLoading() && m_lpBufCur <= m_lpBufMax
	//	|| isStoring() && m_lpBufCur < m_lpBufStart + m_nBufSize;
}

/////////////////////////////////////////////////////////////////////////////


// Archive
BOOL Archive::isLoading() const
	{ return (m_nMode & load) != 0; }
BOOL Archive::isStoring() const
	{ return (m_nMode & load) == 0; }
void Archive::setMode(enum Mode mode)
	{ m_nMode = mode; }
inline BOOL Archive::isByteSwapping() const
	{ return (m_nMode & Archive::bNoByteSwap) == 0; }
BOOL Archive::isBufferEmpty() const
	{ return m_lpBufCur == m_lpBufMax; }
INT  Archive::getCount()
	{ return m_lpBufMax - m_lpBufStart; }
void Archive::shrink(INT nCount)
{
	ASSERT(nCount >= 0 && nCount <= getCount());
	nCount = getCount() - nCount;

	if(m_lpBufMax > m_lpBufStart + nCount)
		m_lpBufMax = m_lpBufStart + nCount;
	if(m_lpBufCur > m_lpBufMax)
		m_lpBufCur = m_lpBufMax;
	
	bNoOverFlow = TRUE;
}
INT  Archive::getBufSize()
	{ return m_nBufSize; }
INT  Archive::getSurplusCount()
	{ return m_lpBufMax - m_lpBufCur; }
UINT  Archive::getSurplusBufSize()
{
	if(bNoFreeOnDestruct == TRUE)
		return m_lpBufStart + m_nBufSize - m_lpBufCur;
	else return (UINT)-1;
}
BOOL Archive::eof()
	{ return m_lpBufCur == m_lpBufMax; }
UINT Archive::getCurrentPos()
	{ return m_lpBufCur - m_lpBufStart; }
BYTE* Archive::getBuffer()
	{ return m_lpBufStart; }
BYTE* Archive::getCurrentPtr()
	{ return m_lpBufCur; }
BOOL  Archive::good()
	{ return bNoOverFlow; }




void Archive::NumberToByteStream(ULONG l, char nBytes)
{
	ASSERT(m_lpBufStart != NULL
		&& m_lpBufCur != NULL
		&& m_lpBufMax != NULL);
	ASSERT(isStoring());
	
	if(!ensureCapacityHelper(m_lpBufCur - m_lpBufStart + nBytes))
		return;
	
	while(nBytes-- > 0)
	{
		*m_lpBufCur ++ = (BYTE)l;// & 0xFF;
		l = l >> 8;
	}
	if(m_lpBufMax < m_lpBufCur)
		m_lpBufMax = m_lpBufCur;
}
Archive& Archive::operator<<(int i)
	{ return Archive::operator<<((LONG)i); }
Archive& Archive::operator<<(unsigned u)
	{ return Archive::operator<<((LONG)u); }
Archive& Archive::operator<<(short w)
	{ return Archive::operator<<((WORD)w); }
Archive& Archive::operator<<(char ch)
	{ return Archive::operator<<((BYTE)ch); }
Archive& Archive::operator<<(BYTE by)
{
	NumberToByteStream(by, sizeof(BYTE));
	return *this;
}

Archive& Archive::operator<<(WORD w)
{
	NumberToByteStream(w, sizeof(WORD));
	return *this;
}
Archive& Archive::operator<<(LONG l)
{
	NumberToByteStream(l, sizeof(LONG));
	return *this;
}
Archive& Archive::operator<<(DWORD dw)
{
	NumberToByteStream(dw, sizeof(DWORD));
	return *this;
}
// INHIBIT Archive& Archive::operator<<(float f)
// INHIBIT {
// INHIBIT 	if(!reallocBuffer(sizeof(float)))
// INHIBIT 		return *this;
// INHIBIT 
// INHIBIT 	byteswap((void*)&f, sizeof(float));
// INHIBIT 	*(float*)m_lpBufCur = *(float*)&f;
// INHIBIT 	m_lpBufCur += sizeof(float);
// INHIBIT 	if(m_lpBufMax < m_lpBufCur)
// INHIBIT 		m_lpBufMax = m_lpBufCur;
// INHIBIT 	return *this;
// INHIBIT }
// INHIBIT Archive& Archive::operator<<(double d)
// INHIBIT {
// INHIBIT 	if(!reallocBuffer(sizeof(double)))
// INHIBIT 		return *this;
// INHIBIT 
// INHIBIT 	byteswap((void*)&d, sizeof(double));
// INHIBIT 	*(double*)m_lpBufCur = *(double*)&d;
// INHIBIT 	m_lpBufCur += sizeof(double);
// INHIBIT 	if(m_lpBufMax < m_lpBufCur)
// INHIBIT 		m_lpBufMax = m_lpBufCur;
// INHIBIT 	return *this;
// INHIBIT }


ULONG Archive::ByteStreamToNumber(char nBytes)
{
	ASSERT(nBytes <= sizeof(ULONG));
	ASSERT(m_lpBufStart != NULL
		&& m_lpBufCur != NULL
		&& m_lpBufMax != NULL);
	
	ULONG ul=0;
	if (m_lpBufCur + nBytes > m_lpBufMax)
	{
		ul = 0;
		bNoOverFlow = FALSE;
		//ASSERT(FALSE);
	}
	else
	{
		ul = 0;
		m_lpBufCur += nBytes;
		
		BYTE  *pTemp = m_lpBufCur - 1;
		while(nBytes-- > 0)
		{
			ul = ul << 8;
			ul += *pTemp--;
		}
	}
	return ul; 
}

Archive& Archive::operator>>(int& i)
	{ return Archive::operator>>((LONG&)i); }
Archive& Archive::operator>>(unsigned& u)
	{ return Archive::operator>>((LONG&)u); }
Archive& Archive::operator>>(short& w)
	{ return Archive::operator>>((WORD&)w); }
Archive& Archive::operator>>(char& ch)
	{ return Archive::operator>>((BYTE&)ch); }
Archive& Archive::operator>>(BYTE& by)
{
	//if (m_lpBufCur + sizeof(BYTE) > m_lpBufMax)
	//{
	//	memset(&by, 0, sizeof(BYTE));
	//	bNoOverFlow = FALSE;
		//ASSERT(FALSE);
	//}
	//else
	//	by = (BYTE)ByteStreamToNumber((void**)&m_lpBufCur, sizeof(BYTE));
	by = (BYTE)ByteStreamToNumber(sizeof(BYTE));
	return *this; 
}

Archive& Archive::operator>>(WORD& w)
{
	//if (m_lpBufCur + sizeof(WORD) > m_lpBufMax)
	//{
	//	memset(&w, 0, sizeof(WORD));
	//	bNoOverFlow = FALSE;
		//ASSERT(FALSE);
	//}
	//else
	//	w = (WORD)ByteStreamToNumber((void**)&m_lpBufCur, sizeof(WORD));
	w = (WORD)ByteStreamToNumber(sizeof(WORD));
	return *this;
}
Archive& Archive::operator>>(DWORD& dw)
{
	//if (m_lpBufCur + sizeof(DWORD) > m_lpBufMax)
	//{
	//	memset(&dw, 0, sizeof(DWORD));
	//	bNoOverFlow = FALSE;
		//ASSERT(FALSE);
	//}
	//else
	//	dw = (DWORD)ByteStreamToNumber((void**)&m_lpBufCur, sizeof(DWORD));
	dw = (DWORD)ByteStreamToNumber(sizeof(DWORD));
	return *this; 
}
// INHIBIT Archive& Archive::operator>>(float& f)
// INHIBIT {
// INHIBIT 	if (m_lpBufCur + sizeof(float) > m_lpBufMax)
// INHIBIT 	{
// INHIBIT 		memset(&f, 0, sizeof(float));
// INHIBIT 		bNoOverFlow = FALSE;
// INHIBIT 		//ASSERT(FALSE);
// INHIBIT 	}
// INHIBIT 	else
// INHIBIT 	{
// INHIBIT 		*(float*)&f = *(float*)m_lpBufCur;
// INHIBIT 		byteswap((void*)&f, sizeof(float));
// INHIBIT 		m_lpBufCur += sizeof(float);
// INHIBIT 	}
// INHIBIT 	return *this; 
// INHIBIT }
// INHIBIT Archive& Archive::operator>>(double& d)
// INHIBIT {
// INHIBIT 	if (m_lpBufCur + sizeof(double) > m_lpBufMax)
// INHIBIT 	{
// INHIBIT 		memset(&d, 0, sizeof(double));
// INHIBIT 		bNoOverFlow = FALSE;
// INHIBIT 		//ASSERT(FALSE);
// INHIBIT 	}
// INHIBIT 	else
// INHIBIT 	{
// INHIBIT 		*(double*)&d = *(double*)m_lpBufCur;
// INHIBIT 		byteswap((void*)&d, sizeof(double));
// INHIBIT 		m_lpBufCur += sizeof(double);
// INHIBIT 	}
// INHIBIT 	return *this; 
// INHIBIT }
Archive& Archive::operator>>(LONG& l)
{
	//if (m_lpBufCur + sizeof(LONG) > m_lpBufMax)
	//{
	//	memset(&l, 0, sizeof(LONG));
	//	bNoOverFlow = FALSE;
		//ASSERT(FALSE);
	//}
	//else
	//	l = (LONG)ByteStreamToNumber((void**)&m_lpBufCur, sizeof(LONG));
	l = (LONG)ByteStreamToNumber(sizeof(LONG));
	return *this; 
}

Archive& Archive::WriteString(LPCSTR lpsz, size_t len)
{
	ASSERT(getSurplusBufSize() >= len + sizeof(USHORT));
	*this << (USHORT)strlen(lpsz);
	write(lpsz, strlen(lpsz) * sizeof(CHAR));
	return *this;
}

Archive& Archive::ReadString(LPSTR lpsz, size_t& len)
{
	USHORT  nLen;
	*this >> nLen;	// string's length
	ASSERT(len >= nLen + 1);
	ASSERT(getSurplusCount() >= nLen);
	read((void *)lpsz, nLen);
	*(lpsz + nLen) = '\0';
	return *this;
}


////////////////////////////////////////////////////////////////////////////
// Archive support for polymorphic reading/writing of CObjects
void Archive::writeObject(const BaseMsg* pBaseMsg)
{
	// object can be NULL
	ASSERT(isStoring());    // proper direction

		// cause the object to serialize itself
	((BaseMsg*)pBaseMsg)->serialize(*this);
}

BaseMsg* Archive::readObject(WORD szClassIDRequested)
{
	ASSERT(szClassIDRequested == 0
		|| BaseMsgFactory::hasClassID(szClassIDRequested));

	ASSERT(isLoading());    // proper direction
	BaseMsg *pBaseMsg = BaseMsgFactory::readObject(*this);
	ASSERT(pBaseMsg != NULL);
	ASSERT(szClassIDRequested == pBaseMsg->getTypeIDVirtual());

	return pBaseMsg;
}
////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
// static function of Archive
/////////////////////////////////////////////////////////////////////
void Archive::Write(void** buf, void *p, size_t len)
{
	memcpy(*buf, p, len);
	*(BYTE**)buf += len;
}

void Archive::Read(void** buf, void *p, size_t len)
{
	memcpy(p, *buf, len);
	*(BYTE**)buf += len;
}

void Archive::WriteString(void** buf, LPCSTR lpsz)
{
	Write(buf, (INT)strlen(lpsz));
	Write(buf, (void*)lpsz, strlen(lpsz)*sizeof(CHAR));
}

void Archive::ReadString(void** buf, LPSTR lpsz, size_t& len)
{
	INT  nLen;
	Read(buf, nLen);
	ASSERT(len >= nLen + 1);
	Read(buf, lpsz, nLen);
	*(lpsz + nLen) = '\0';
	len = nLen;
}
/////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////




CLBTH_NAMESPACE_END
