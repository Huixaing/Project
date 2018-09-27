/*
 *
 * Copyright (c) 2003
 * Author: Dawnov  Dec 1, 2002
 *
 */


#if _MSC_VER > 1000
#pragma once
#endif


#ifndef  __DAWNOV_INL_
#define  __DAWNOV_INL_

#pragma warning(disable:4786)

#include "Threads.h"
#include <cstdlib>
#include <cassert>
#include <map>	// MFC STL
using namespace std;



//namespace  dawnov
//{








////////////////////////////////////////////////
//
// Create an object using new operator, corresponding
// free it using delete
//
////////////////////////////////////////////////
template <class T> struct CreateUsingNew
{
protected:
	static T* Create()	{ return new T; }
	static void Destroy(T* pObj) { delete pObj; }
};


////////////////////////////////////////////////
//
// Create an object being seperated into two steps:
//    malloc suitable memory, and then construct the 
//    object in the malloced memory.
// Free an object also has two corresponding steps:
//    destruct the object, and then free its memory.
//
////////////////////////////////////////////////
template <class T> struct CreateUsingMalloc
{
protected:
	static T* Create()
	{
		void *p = malloc(sizeof(T));
		if( !p )	return 0;
		return new (p) T;
	}
	static void Destroy(T* pObj)
	{
		pObj->~T();
		free(pObj);
	}
};

////////////////////////////////////////////////
//
// CreateStatic Creating object also being seperated 
// into two steps, but the memory object resided is 
// alloced by run-time-library when startuping and
// will be freed when exiting.
// So, we just only to construct and destruct the object
// in the designated memroy.
//
////////////////////////////////////////////////
template <class T> struct CreateStatic
{
    union MaxAlign
    {
        char t_[sizeof(T)];
        short int shortInt_;
        int int_;
        long int longInt_;
        float float_;
        double double_;
        long double longDouble_;
        struct Test;
        int Test::* pMember_;
        int (Test::*pMemberFn_)(int);
    };

protected:
	static T* Create()
	{
		static MaxAlign  staticMemory__;
		return new(&staticMemory__) T;
	}
	static void Destroy(T* pObj) {	pObj->~T();	}
};


////////////////////////////////////////////////
//
// When to destruct the singleton?
// What should do when accessing an destructed singleton?
//
// DefaultLifetime destructs singleton at exiting time,
// and raise an exception when accessing an dead singleton.
//
// PhoenixSingleton destructs singleton at exiting time,
// and constructs it again accessing an dead singleton. But 
// when to destructs the latter object? How to do with it?
//
// NoDestroy never destructs singleton.
//
////////////////////////////////////////////////
template <class T> struct DefaultLifetime
{
	static void ScheduleDestruction(T*, void (*pDtorFunc)())
	{
		atexit(pDtorFunc);
	}
	static void OnDeadReference()
    {
		throw std::logic_error("Dead Reference Detected");
	}
};

template <class T> struct PhoenixSingleton
{
	static void ScheduleDestruction(T*, void (*pDtorFunc)())
	{
#ifndef ATEXIT_FIXED
		if (!destroyedOnce_)
#endif
		std::atexit(pDtorFunc);
	}
	static void OnDeadReference()
	{
#ifndef ATEXIT_FIXED
		destroyedOnce_ = true;
#endif
	}
        
private:
#ifndef ATEXIT_FIXED
	static bool destroyedOnce_;
#endif
};
    
#ifndef ATEXIT_FIXED
template <class T> bool PhoenixSingleton<T>::destroyedOnce_ = false;
#endif

template <class T> struct Adapter
{
    void operator()(T*) { return pFun_(); }
    void (*pFun_)();
};
template <class T> class SingletonWithLongevity
{
public:
    static void ScheduleDestruction(T* pObj, void (*pFun)())
    {
        Adapter<T> adapter = { pFun };
        SetLongevity(pObj, GetLongevity(pObj), adapter);
    }
    
    static void OnDeadReference()
	{
		throw std::logic_error("Dead Reference Detected");
	}
};



template <class T> struct NoDestroy
{
    static void ScheduleDestruction(volatile T*, void (*)()) {}
    static void OnDeadReference() {}
};


//////////////////////////////////////////////////
//
// Singleton Pattern
// Implement with mix-based class
//
// T has must have protected default ctors & dtors 
// and have no public ctors & dtors. As a result, user
// neither constructs nor destructs T. To do that, we
// must use CreateUsingNew, CreateUsingMalloc, 
// CreateStatic. So, just only CSingleHolder::Instance
// can construct or destruct on singleton object.
//
//////////////////////////////////////////////////
template<class T> class CSingleton : public T
{
public:
	T* operator->() volatile
	{	return const_cast<T*>(this);	}
	T& operator*() volatile
	{	return *operator->();	}
	BOOL InitSingleton()
	{
		return Init();
	}
private:
	CSingleton(){}
	virtual ~CSingleton(){}

	friend struct CreateUsingNew< CSingleton<T> >;
	friend struct CreateUsingMalloc< CSingleton<T> >;
	friend struct CreateStatic< CSingleton<T> >;
	//friend template<class U> struct CreateUsingNew;
	//friend template<class U> struct CreateUsingMalloc;
	//friend template<class U> struct CreateStatic;
};




//////////////////////////////////////////////////
//
// If VC++ support "friend template member fuction",
// CSingletonHolder can not derived from CreationPolicy
// for accessing its protected member fuction: Create
// and Destroy.
//
// Now we must derived from CreationPolicy for inhibiting
// user create singleton using other than CSingletonHolder
// methods.
//
//////////////////////////////////////////////////
template
<	class T,
	class CreationPolicy = CreateUsingNew< CSingleton<T> >,
	class LifetimePolicy = DefaultLifetime< CSingleton<T> >,
	class ThreadingModel = ClassLevelLockable< CSingleton<T> >
>
class CSingletonHolder : public CreationPolicy
{
public:
	static T& Instance();

protected:
	CSingletonHolder() {};
	~CSingletonHolder() {};
private:
	static void MakeInstance();
	static void DestroySingleton();

	CSingletonHolder(CSingletonHolder& rhs);
	CSingletonHolder& operator=(CSingletonHolder& rhs);

	typedef typename ThreadingModel::VolatileType  VolatileType;
	typedef CSingleton<T>  NonVolatileType;
	static VolatileType* pInstance__;
	static bool destroyed__;
};


template
<	class T,
	class C,
	class L,
	class M
>
typename CSingletonHolder<T, C, L, M>::VolatileType*
    CSingletonHolder<T, C, L, M>::pInstance__;

template
<	class T,
	class C,
	class L,
	class M
>
bool CSingletonHolder<T, C, L, M>::destroyed__;


template
<	class T,
	class CreationPolicy,
	class LifetimePolicy,
	class ThreadingModel
>
inline T& CSingletonHolder<T, CreationPolicy, 
    LifetimePolicy, ThreadingModel>::Instance()
{
    if (!pInstance__)
        MakeInstance();

    return *reinterpret_cast< CSingleton<T>* >(
		const_cast<NonVolatileType*>(pInstance__));
}

template
<	class T,
	class CreationPolicy,
	class LifetimePolicy,
	class ThreadingModel
>
void CSingletonHolder<T, CreationPolicy, 
    LifetimePolicy, ThreadingModel>::MakeInstance()
{
	_Lockit lk;
	//typename ThreadingModel::Lock  guard;
    //(void)guard;

	if( !pInstance__ )
	{
		if( destroyed__ )
		{
			LifetimePolicy::OnDeadReference();
			destroyed__ = false;
		}
		//pInstance__ = CreationPolicy::Create();
		pInstance__ = Create();
		LifetimePolicy::ScheduleDestruction(
			const_cast<NonVolatileType*>(pInstance__),
			&DestroySingleton);
		ASSERT(((CSingleton<T>&)Instance()).InitSingleton());
	}
}

template
<	class T,
	class CreationPolicy,
	class LifetimePolicy,
	class ThreadingModel
>
void CSingletonHolder<T, CreationPolicy, 
    LifetimePolicy, ThreadingModel>::DestroySingleton()
{
	assert(!destroyed__);
	//CreationPolicy::Destroy(const_cast<NonVolatileType*>(pInstance__));
	Destroy(const_cast<NonVolatileType*>(pInstance__));
	pInstance__ = 0;
	destroyed__ = true;
}


/*template
<	class T,
	class CreationPolicy = CreateStatic< CSingleton<T> >,
	class LifetimePolicy = DefaultLifetime< CSingleton<T> >,
	class ThreadingModel = ClassLevelLockable< CSingleton<T> >
>
class CSingletonCreateHelper
{
public:
	static T*  GetObjectPtr()
	{
		return &CSingletonHolder< T, CreationPolicy,
			LifetimePolicy, ThreadingModel>::Instance();
	}
	static T&  GetObjectRef()
	{
		return *GetObjectPtr();
	}
	virtual bool Init() = 0;
protected:
	CSingletonCreateHelper(){}
	virtual ~CSingletonCreateHelper(){}
private:
	CSingletonCreateHelper(CSingletonCreateHelper& rhs){}
	CSingletonCreateHelper& operator=(CSingletonCreateHelper&)
		{	return *this;	}
};*/


/////////////////////////////////////////////////////
//
// We afford the macro GET_SINGLETON_OBJECT to get
// singleton instance easily. 
//
// Note that the singleton instantiating in one dll
// can be also getted in other dll.
//
/////////////////////////////////////////////////////
//#define  SINGLE_DLL
#ifndef  SINGLE_DLL
	#define DECLARE_SINGLETON(CLASS) \
		struct AFX_EXT_CLASS __csc##CLASS { CLASS* operator()(); };
	#define IMPLEMENT_SINGLETON(CLASS)\
		CLASS* __csc##CLASS::operator()()\
		{	return &CSingletonHolder< CLASS >::Instance();}
	#define GET_SINGLETON_OBJECT(CLASS) \
		__csc##CLASS()()
#else
	#define DECLARE_SINGLETON(CLASS)
	#define IMPLEMENT_SINGLETON(CLASS)
	#define GET_SINGLETON_OBJECT(CLASS) \
		(&CSingletonHolder< CLASS >::Instance())
#endif







// Operation System Memory Addressing traits
// OS2 and MS Windows is LowByte in LowAddress
//     such as  (0x01234)=0xABCD, (0x1234)=0xCD, (0x1235)=0xAB
// AIX is LowByte in HighAddress
//     such as  (0x01234)=0xABCD, (0x1234)=0xAB, (0x1235)=0xCD
struct exchange_traits
{
	typedef struct {} exchange_byte;
	typedef struct {} exchange_bit;

	exchange_traits()
		: bExchangeByte(false), bExchangeBit(false)
	{}
	exchange_traits(exchange_byte&)
		: bExchangeByte(true), bExchangeBit(false)
	{}
	exchange_traits(exchange_bit&)
		: bExchangeByte(false), bExchangeBit(true)
	{}
	exchange_traits(exchange_byte&, exchange_bit&)
		: bExchangeByte(true), bExchangeBit(true)
	{}

	// 字节交换
	template<typename T> void operator()(T *v_addr)
	{
		if( !bExchangeByte )	return;

		UINT  size = sizeof(T);
		register char  *p = (char*)v_addr;
		register char  *q = p + size -1;

		while( p<q ) {
			*p = *p ^ *q;	*q = *p ^ *q;	*p = *q ^ *p;
			p++, q--;
		}

		p = (char*)v_addr;
		q = p + size;
		while(p<q)	(*this)(p++);
	}

	// 位交换
	template<> void operator()(char *v_addr)
	{
		if( !bExchangeBit ) return;

		register char  p = *v_addr;
		register char  q = 0;
		register int i=0;
		do{
			q <<= 1;
			q |= p & 0x01;
			p >>= 1;
		} while(++i<8);
		*v_addr = q;
	}
	template<> void operator()(unsigned char *v_addr)
	{
		(*this)((char *)v_addr);
	}

private:
	bool  bExchangeByte;	// high byte first for IBM OS2
	bool  bExchangeBit;	// define exchanging bit for myself
};



class  IstreamFile
{
public:
	IstreamFile() : pos(0), file_size(0), pFile(0), ref_count(0){}
	IstreamFile(LPCSTR  lpszFileName)
	{
		OpenFile(lpszFileName);
	}
	virtual ~IstreamFile()
	{
		if(::InterlockedDecrement(ref_count)==0)
		{
			try {
				pFile->Close();
			}catch(CFileException *e)
			{
				ExceptionReport(*e);
				e->Delete();
			}
			delete pFile;
			delete ref_count;
			delete pos;
		}
	}

	void  OpenFile(LPCSTR  lpszFileName)
	{
		CFileException e;
		pFile = new CFile();

		if( pFile->Open(lpszFileName, /*CFile::modeCreate |*/
				CFile::modeReadWrite /*| CFile::shareExclusive*/, &e)
			)
		{
			file_size = pFile->GetLength();
			ref_count = new LONG(1);
			pos = new LONG(0);
		}
		else {
			ExceptionReport(e);
		}
	}

	void  ExceptionReport(CFileException& e)
	{
		if (e.m_cause == CFileException::none)
			AfxMessageBox("No error occurred");
		else
		{
			if (e.m_cause == CFileException::fileNotFound)
				AfxMessageBox("The file could not be located");
			else if (e.m_cause == CFileException::tooManyOpenFiles)
				AfxMessageBox("The permitted number of open files was exceeded, File handles not enough");
			else if (e.m_cause == CFileException::hardIO)
				AfxMessageBox("There was a hardware error");
			else if (e.m_cause == CFileException::diskFull)
				AfxMessageBox("The disk is full");
			else if (e.m_cause == CFileException::badPath)
				AfxMessageBox("All or part of the path is invalid");
			else if (e.m_cause == CFileException::generic)
				AfxMessageBox("An unspecified error occurred");
			else if (e.m_cause == CFileException::accessDenied)
				AfxMessageBox("The file could not be accessed");
			else if (e.m_cause == CFileException::invalidFile)
				AfxMessageBox("There was an attempt to use an invalid file handle");
			else if (e.m_cause == CFileException::removeCurrentDir)
				AfxMessageBox("The current working directory cannot be removed");
			else if (e.m_cause == CFileException::directoryFull)
				AfxMessageBox("There are no more directory entries");
			else if (e.m_cause == CFileException::badSeek)
				AfxMessageBox("There was an error trying to set the file pointer");

			else if (e.m_cause == CFileException::sharingViolation)
				AfxMessageBox("SHARE.EXE was not loaded, or a shared region was locked");
			else if (e.m_cause == CFileException::lockViolation)
				AfxMessageBox("There was an attempt to lock a region that was already locked");

			else if (e.m_cause == CFileException::endOfFile)
				AfxMessageBox("he end of file was reached");
			else
				AfxMessageBox("Unknown File error");
	
			ASSERT(false);
		}
		e.Delete();
	}

	
	IstreamFile& operator=(IstreamFile& ar)
	{
		pFile = ar.pFile;
		file_size = ar.file_size;
		pos = ar.pos;
		ref_count = ar.ref_count;
		::InterlockedIncrement(ref_count);
		return *this;
	}

	template<typename T> IstreamFile&  operator >> (T& t)
	{
		Read(&t, 1);
		//UINT size = sizeof(T);
		//UINT cnt = pFile->Read(&t, size);
		//if(cnt != size)
		//	memset(&t, 0xFF, size);
		//*pos += size;
		return *this;
	}
	
	template<typename T> IstreamFile&  operator << (T  t)
	{
		Write(&t, 1);
		return *this;
	}

	template<typename T> void Read(T *t, UINT size)
	{
		size *= sizeof(T);
		UINT  cnt = pFile->Read(t, size);
		if(cnt != size)
			memset(t, 0xFF, size);
		*pos += size;
	}

	template<typename T> void Write(T *t, UINT size)
	{
		size *= sizeof(T);
		try {
			pFile->Write(t, size);
		}catch(CFileException& e)
		{
			ExceptionReport(e);
		}
		*pos += size;
	}

	BOOL  Seek(LONG lOff, UINT from = CFile::current)
	{
		switch(from)
		{
		case CFile::begin:
			*pos = lOff;
			break;
		case CFile::current:
			*pos += lOff;
			break;
		case CFile::end:
			*pos = file_size + lOff;
			break;
		}
		ASSERT((ULONG)*pos <= file_size);//ASSERT(0 <= *pos && *pos <= file_size);
		
		try	{ pFile->Seek(lOff, from); }
		catch(CFileException* e)
		{
			e->Delete();
			return FALSE;
		}
		return TRUE;
	}
	ULONG  GetCurrentOffset(){ return *pos; }
	BOOL  Eos() {  return GetCurrentOffset() == file_size;  }


private:
	CFile  *pFile;
	ULONG  file_size;
	LPLONG pos;
	LPLONG ref_count;
};


class IstreamMem
{
	//constructor & destructor
public:
	IstreamMem() : __BlockSize(512)
	{
		pRef_count = new LONG(1);
		start = new (BYTE *)(0);
		cur_pos = new (BYTE *)(0);
		finish = new (BYTE *)(0);
		end_of_storage = new (BYTE*)(0);

		*start = *cur_pos = *finish = new BYTE[__BlockSize];
		*end_of_storage = *start + __BlockSize;
		memset(*start, 0, __BlockSize);
	}
	IstreamMem(BYTE* buf, ULONG size) : __BlockSize(512)
	{
		pRef_count = new LONG(1);
		start = new (BYTE *)(0);
		cur_pos = new (BYTE *)(0);
		finish = new (BYTE *)(0);
		end_of_storage = new (BYTE*)(0);

		ULONG new_size = (size + __BlockSize - 1) & ~(__BlockSize - 1);
		*start = *cur_pos = *finish = new BYTE[new_size];
		*end_of_storage = *start + new_size;
		memset(*cur_pos, 0, new_size);
		memcpy(*start, buf, size);
		*finish += size;
	}
	IstreamMem(IstreamMem& other) : __BlockSize(512)
	{
		pRef_count = other.pRef_count;
		start = other.start;
		cur_pos = other.cur_pos;
		finish = other.finish;
		end_of_storage = other.end_of_storage;

		::InterlockedIncrement(pRef_count);
	}
	IstreamMem& operator=(IstreamMem& other)
	{
		if( ::InterlockedDecrement(pRef_count) == 0 )
			delete[] *start;

		pRef_count = other.pRef_count;
		start = other.start;
		cur_pos = other.cur_pos;
		finish = other.finish;
		end_of_storage = other.end_of_storage;

		::InterlockedIncrement(pRef_count);
		return *this;
	}

	virtual ~IstreamMem()
	{
		if(::InterlockedDecrement(pRef_count)==0)
		{
			if(*start)
				delete[] *start;
			delete start;
			delete cur_pos;
			delete finish;
			delete end_of_storage;
			delete pRef_count;
		}
	}

	// operators
public:
	// buffer operators
	void    SetBuffer(BYTE *buf, ULONG size)
	{
		ULONG new_size = (size + __BlockSize - 1) & ~(__BlockSize - 1);
		*cur_pos = *finish = new BYTE[new_size];
		*end_of_storage = *cur_pos + new_size;
		memset(*cur_pos, 0, new_size);
		memcpy(*cur_pos, buf, size);
		*finish += size;
		if( ::InterlockedDecrement(pRef_count) == 0 )
		{
			if(*start)
				delete[] *start;
		}
		*start = *cur_pos;
		*pRef_count = 1;
	}
	BYTE*	GetBuffer(){	return *start;	}

	BOOL   Seek(LONG lOff, UINT from)
	{
		switch(from)
		{
		case CFile::begin:
			ASSERT(lOff <= *finish - *start);
			*cur_pos = *start + lOff;
			break;
		case CFile::current:
			ASSERT(*start - *cur_pos <= lOff
				&& lOff < *finish - *cur_pos);
			*cur_pos += lOff;
			break;
		case CFile::end:
			ASSERT(lOff >= (*start - *finish));
			*cur_pos = *finish + lOff;
			break;
		}
		return TRUE;
	}

	// for debug
	template<typename T> void Read(T *t, UINT size)
	{
		ASSERT(*cur_pos + size <= *finish);
		memcpy(t, *cur_pos, size);
		*cur_pos += size;
	}
	template<typename T> void Write(T *t, UINT size)
	{
		for(int i=0; i<size; i++)
			(*this) << *t;
		*cur_pos += size;
	}

	ULONG	GetCurrentOffset() { return *cur_pos - *start; }
	BOOL  Eos() {  return  *cur_pos == *finish;  }
	
private:
	BYTE	**start;
	BYTE	**cur_pos;
	BYTE	**finish;
	BYTE	**end_of_storage;
	LPLONG  pRef_count;
	USHORT  __BlockSize;// = 512;

private:
	// Serialize operators
	template<typename T> IstreamMem& operator <<(T val)
	{
		if(*cur_pos + sizeof(T) > *end_of_storage)
		{
			ULONG  new_size = ( (*end_of_storage - *start)*5/4 + sizeof(T)*20 + __BlockSize - 1) & ~(__BlockSize - 1);

			BYTE *new_buffer = new BYTE[new_size];
			ASSERT(new_buffer != NULL);
			memset(new_buffer, 0, new_size);
			memcpy(new_buffer, *start, *finish - *start);
			delete[] (*start);
			
			*cur_pos = new_buffer + (*cur_pos - *start);
			*finish = new_buffer + (*finish - *start);
			*start = new_buffer;
			*end_of_storage = new_buffer + new_size;
		}
		**(T **)cur_pos = val;
		*cur_pos += sizeof(T);
		if(*cur_pos > *finish)
			*finish = *cur_pos;
		return *this;
	}
		
	template<typename T> IstreamMem& operator >>(T& val)
	{
		ASSERT(*cur_pos + sizeof(T) <= *finish);
		val = **(T **)cur_pos;
		*cur_pos += sizeof(T);
		return *this;
	}
};


class Istream
{
public:
	enum  __BUF_FLAG {
		FILE_BUFFER	= 0,
		MEM_BUFFER	= 1
	};
	Istream(CString filename = CString("")) : bUseExchangeTraits(FALSE)
	{
		if(filename.IsEmpty())
		{
			buffer.p = new IstreamMem;
			buffer.flag = MEM_BUFFER;
		}
		else
		{
			buffer.p = new IstreamFile(filename);
			buffer.flag = FILE_BUFFER;
		}
	}
	Istream(CString filename, exchange_traits &ex)
		: bUseExchangeTraits(TRUE), __exchange(ex)
	{
		if(filename.IsEmpty())
		{
			buffer.p = new IstreamMem;
			buffer.flag = MEM_BUFFER;
		}
		else
		{
			buffer.p = new IstreamFile(filename);
			buffer.flag = FILE_BUFFER;
		}
	}
	Istream(exchange_traits &ex)
		: bUseExchangeTraits(TRUE), __exchange(ex)
	{
		buffer.p = new IstreamMem;
		buffer.flag = MEM_BUFFER;
	}
	virtual ~Istream()
	{
		if(buffer.p == NULL)	return;

		switch(buffer.flag)
		{
		case MEM_BUFFER:
			delete buffer.pMEM;
			break;
		case FILE_BUFFER:
			delete buffer.pFILE;
			break;
		}
	}
private:
	struct {
		enum __BUF_FLAG  flag;
		union {
			void *p;
			IstreamFile    *pFILE;
			IstreamMem  *pMEM;
		};
	} buffer;
	BOOL    bUseExchangeTraits;
	exchange_traits  __exchange;

public:
	void    SetExchangeTraits(exchange_traits &ex)
	{
		__exchange = ex;
		bUseExchangeTraits = TRUE;
	}

	enum  SEEK_FLAG {
		beg = CFile::begin,
		cur = CFile::current,
		end = CFile::end
	};
	BOOL   Seek(LONG lOff, UINT from = cur)
	{
		switch(buffer.flag)
		{
		case MEM_BUFFER:
			return buffer.pMEM->Seek(lOff, from);
		case FILE_BUFFER:
			return buffer.pFILE->Seek(lOff, from);
		}
		return FALSE;
	}
	// for debug
	template<typename T> void Read(T *t, UINT size)
	{
		switch(buffer.flag)
		{
		case MEM_BUFFER:
			buffer.pMEM->Read(t, size);
			break;
		case FILE_BUFFER:
			buffer.pFILE->Read(t, size);
			break;
		}
	}
	template<typename T> void Write(T *t, UINT size)
	{
		switch(buffer.flag)
		{
		case MEM_BUFFER:
				buffer.pMEM->Write(t, size);
			break;
		case FILE_BUFFER:
				buffer.pFILE->Write(t, size);
			break;
		}
		/*int i;
		T	*tmp = t;
		switch(buffer.flag)
		{
		case MEM_BUFFER:
			for(i=0; i<size; i++)
				*(buffer.pMEM) << *tmp ++;
			break;
		case FILE_BUFFER:
			for(i=0; i<size; i++)
				*(buffer.pFILE) << *tmp ++;
			break;
		}*/
	}

	ULONG	GetCurrentOffset()
	{
		switch(buffer.flag)
		{
		case MEM_BUFFER:
			return buffer.pMEM->GetCurrentOffset();
		case FILE_BUFFER:
			return buffer.pFILE->GetCurrentOffset();
		}
		return 0;
	}
	BOOL  Eos()
	{
		switch(buffer.flag)
		{
		case MEM_BUFFER:
			return buffer.pMEM->Eos();
		case FILE_BUFFER:
			return buffer.pFILE->Eos();
		}
		return TRUE;
	}
	
	// Serialize operators
	template<typename T> Istream& operator << (T t)
	{
		if( bUseExchangeTraits )	__exchange(&t);
		switch(buffer.flag)
		{
		case MEM_BUFFER:
			*(buffer.pMEM) << t;
			break;
		case FILE_BUFFER:
			*(buffer.pFILE) << t;
			break;
		}
		return *this;
	}
	
	template<typename T> Istream& operator >> (T& t)
	{
		switch(buffer.flag)
		{
		case MEM_BUFFER:
			*(buffer.pMEM) >> t;
			break;
		case FILE_BUFFER:
			*(buffer.pFILE) >> t;
			break;
		}
		if( bUseExchangeTraits )	__exchange(&t);
		return *this;
	}
};




// for auto dynamic object from stream
interface ISerializable
{
public:
	ISerializable(){}
	virtual ~ISerializable(){}

	virtual BOOL Load(Istream& ar) = 0;
	virtual void Save(Istream&) = 0;
	
	typedef ISerializable* (*CreateFun)();
};


template<class T> class io : public T
{
public:
	virtual  ~io(){}
	static  io*  createObject(){	return new io();	}

private:
	io(){}
};



/*
 * For creating those class object identified by an intenger;
 */
/*struct IO_MAP_INTENGER
{
	static CMap<ULONG, ULONG, ISerializable::LoadFrom, 
				ISerializable::LoadFrom> *io_map;
	IO_MAP_INTENGER(ULONG type_id, ISerializable::LoadFrom f)
	{
		if(io_map == 0)
			io_map = new CMap<ULONG, ULONG, ISerializable::LoadFrom, 
								ISerializable::LoadFrom>;
		ASSERT((*io_map)[type_id] == NULL);
		(*io_map)[type_id] = f;
	}
};
#define IMPLEMENT_SERIALIZE_FOR_INTENGER(classname, sn) \
static IO_MAP_INTENGER _io_map_intenger_##classname##sn \
		( classname::TypeID, \
		  (ISerializable::LoadFrom)&io<classname>::createObject \
		);

#define IMPLEMENT_SERIALIZE_FOR_ICON(classname, type, sn) \
static IO_MAP_INTENGER _io_map_intenger_##classname##type##sn \
		( type, (ISerializable::LoadFrom)&io<classname>::createObject \
		);
*/


/*
 * For creating those class object identified by a string;
 */
/*struct IO_MAP_STRING
{
	static CMap<CString, LPCSTR, ISerializable::LoadFrom, ISerializable::LoadFrom> *io_map_string_buffer;
	IO_MAP_STRING(CString  name_str, ISerializable::LoadFrom f)
	{
		if(io_map_string_buffer == 0)
			io_map_string_buffer = new CMap<CString, LPCSTR, ISerializable::LoadFrom, ISerializable::LoadFrom>;
		(*io_map_string_buffer)[name_str] = f;
	}
};
#define IMPLEMENT_SERIALIZE_FOR_STRING(classname) \
static IO_MAP_STRING _io_map_string_buffer_##classname(#classname, (ISerializable::LoadFrom)&io<classname>::createObject); \
*/




/*
template<typename  T> struct IO_MAP_FOR_CASCO
{
	IO_MAP_FOR_CASCO(long type_id, ISerializable::LoadFrom f)
	{
		if(io_map==NULL)
			io_map = new std::map<long, ISerializable::LoadFrom>();
		ASSERT((*io_map)[type_id] == NULL);
		(*io_map)[type_id] = f;
	}
	static ISerializable::LoadFrom&  GetCreator(long type)
		{	return (*io_map)[type];	}
	
	static std::map<long, ISerializable::LoadFrom> *io_map;
	//static CMap<long, long, ISerializable::LoadFrom, ISerializable::LoadFrom> *mfc_io_map;
};
template<typename T>
std::map<long, ISerializable::LoadFrom>  *IO_MAP_FOR_CASCO<T>::io_map;


#define IMPLEMENT_SERIALIZE_FOR_CASCO(baseclass, classname, type, sn) \
static IO_MAP_FOR_CASCO<baseclass> _io_map_intenger_##classname##type##sn \
		( type, (ISerializable::LoadFrom)&io<classname>::createObject \
		);
*/




///////////////////////////////////////////////////////
//
// DefaultFactoryError::OnUnknownType throws exception
// when trying to create one product with an unknown
// product id
//
// IdentifierType is the type of product id
// ProductType is the product class
//
///////////////////////////////////////////////////////
template<typename ProductType>
class DefaultFactoryError
{
public:
	typedef typename ProductType::IdentifierType IdentifierType;

public:
	class Exception : public std::exception
	{
	public:
		Exception(const IdentifierType& unknownId)
			: __unknownId(unknownId)
		{}
		virtual const char* what()
		{
			return "Unknown object type passed to Factory.";
		}
		const IdentifierType GetId()
		{
			return __unknownId;
		}
	private:
		IdentifierType  __unknownId;
	};
public:
	static ProductType* OnUnknownType(const IdentifierType& id)
	{
		ASSERT(FALSE);
		//throw Exception(id);
		return NULL;
	}
};


////////////////////////////////////////////////////////////////
//
// CProductFactory can create those products derived from ProductType,
// ProductType is the base class of product cluster. Different
// ProductType represents different product cluster. 
//
// We take ProductType as the template parameter just for instantiate 
// AssocMap each time. Because ProductType::IdentifierType can be the
// same, and different product can be the same id value. Which product
// can we create? So We instantiate different map for different product
// cluster.
//
// Derived from CSingletonHolder template just for getting its
// member function "Instance", etc. To create a CProductFactory
// instance. We can also make CProductFactory not derived from 
// CSingletonHolder, and then we must create instance through 
// CSingletonHolder<...>::Instance.
// The above two method is identical.
//
////////////////////////////////////////////////////////////////
template
	<class ProductType, 
	 class FactoryErrorPolicy = DefaultFactoryError<ProductType>
	>
class CProductFactory
	: public CSingletonHolder
		< CProductFactory< ProductType, FactoryErrorPolicy> >
{
public:
	typedef CProductFactory<ProductType, FactoryErrorPolicy> _Myt;
	typedef typename ProductType::IdentifierType IdentifierType;
	typedef typename ProductType::ProductCreator ProductCreator;

public:
	ProductType* createObject(Istream& ar)
	{
		if(ar.Eos())
			return NULL;

		IdentifierType  id = ProductType::GetTypeID(ar);
		if(ProductCreator  pf = GetCreator(id))
		{
			ProductType* p = (ProductType *)pf();
			return p;
		}
		else
			return FactoryErrorPolicy::OnUnknownType(id);
	}

	bool Init() { return true; }
protected:
	CProductFactory() {}

private:
	bool Register(const IdentifierType& id, ProductCreator creator)
	{
		return GetAssocMap().insert(AssocMap::value_type(id, creator)).second;
	}
	bool Unregister(const IdentifierType& id)
	{
		return GetAssocMap().erase(id) == 1;
	}
	ProductCreator&  GetCreator(IdentifierType type)
		{	return GetAssocMap()[type];	}

	typedef std::map<IdentifierType, ProductCreator> AssocMap;
	AssocMap& GetAssocMap()
		{ static AssocMap io_map;	return io_map; }

public:
	struct IO_MAP {
		IO_MAP(const IdentifierType& id, ProductCreator creator)
		{
			// The following two is identical
			ASSERT( true == Instance().Register(id, creator) );
			//ASSERT( true == CSingletonHolder<_Myt>::Instance().Register(id, creator) );
		}
	};
	friend struct IO_MAP;
};

// Only use in the declaration of those classes that can represent a 
// cluster of product
// for example: in SdLayout.dll, CFigure represents a product cluster.
#define DECLARE_SERIALIZE_FOR_PRODUCT_CLUSTER(productType, identifierType) \
	public: \
		typedef productType     ProductType; \
		typedef identifierType  IdentifierType; \
		typedef productType::CreateFun    ProductCreator;

// Must use in the definition of each product(ie, in *.cpp)
#define IMPLEMENT_SERIALIZE_WSG(classname, id) \
static CProductFactory<classname::ProductType>::IO_MAP __wsgserialize_##classname##id \
	( classname::id, (classname::ProductCreator)&io<classname>::createObject );






/////////////////////////////////////////////////////////////////////
struct StaticBuffer{
	StaticBuffer(short size)
	{
		size = (size + sizeof(short) + 32 - 1) & ~(32 - 1);
		buffer = new char[size];
		ASSERT(buffer);
		memset(buffer, 0, size);

		*(short*)buffer = size;
		buffer += sizeof(short);
	}
	~StaticBuffer()
	{
		if(buffer)
		{
			delete [](buffer-sizeof(short)); buffer = NULL;
		}
	}
	void SetZero()
	{
		memset(buffer+2, 0, *(USHORT*)buffer);
	}
	bool Expand(short size)
	{
		if(size+sizeof(short) > *(short*)(buffer-sizeof(short)))
		{
			size = (size + sizeof(short) + 32 - 1) & ~(32 - 1);
			char *pNew = new char[size];
			if(NULL == pNew)
			{
				ASSERT(FALSE);
				return false;
			}
			memset(pNew, 0, size);
			buffer -= sizeof(short);
			memcpy(pNew, buffer, *(short*)buffer);
			*(short*)pNew = size;

			delete []buffer;
			buffer = pNew + sizeof(short);
		}
		return true;
	}
	char *buffer;
};





/////////////////////////////////////////////////////////////////////
template<class T> class AFX_EXT_CLASS SmartPtr
{
private:
	struct DATA {
		void *ptr;
		long nRef;
		DATA(void *p, long ref) : ptr(p), nRef(ref) {}
	} *pData;
	//T *ptr;
	//int *nRef;
public:
	SmartPtr(T *p = NULL, long n = 1) : pData(new DATA(p, n)) {}
	SmartPtr(const SmartPtr& rhs)
	//template<class T1> SmartPtr(const SmartPtr<T1>& rhs)
		{
			//T1 t1;
			//T t = t1;
			memcpy(this, &rhs, sizeof(SmartPtr));
			//T1 *pT1 = *(T1**)(void*)&rhs;
			//T *p = dynamic_cast<T*>(pT1);
			//ASSERT(NULL != p);
			//ptr = p;
			//nRef = (int*)((char*)&rhs + sizeof(T*));
			inc_ref();
		}
	SmartPtr& operator=(const SmartPtr& rhs)
		{
			if(this == &rhs || *this == rhs)
				return *this;

			dec_ref();
			memcpy(this, &rhs, sizeof(SmartPtr));
			inc_ref();
			return *this;
		}
	~SmartPtr() { dec_ref(); }

	BOOL operator==(const SmartPtr& rhs)
		{
			return (this == &rhs || pData == rhs.pData)
		}

	template<class T1> operator SmartPtr<T1>()
	{
		return SmartPtr<T1>(ptr, nRef + 1);
	}
private:
	SmartPtr& operator=(T* ptr);	// inhibit
	void inc_ref()  { ASSERT(pData); ::InterlockedIncrement(&pData->nRef); }
	void dec_ref()
		{ ASSERT(pData != NULL); if(::InterlockedDecrement(&pData->nRef) == 0)
			{	delete direct(); delete pData;	pData = NULL; }
		}

public:
	T* operator->() const { return direct(); }
	T* direct() const {	ASSERT(pData); return (T*)pData->ptr;	}
	BOOL IsNullPtr() const { return pData->ptr == NULL; }
};







//};	// end of namespace dawnov

#endif	//#ifndef  __DAWNOV_INL_