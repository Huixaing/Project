
/*
 *  Copyright (c)  Casco Signal Ltd. co.   Sep 19, 2002
 *  Author : wsg
 *  The template is written just for wrap accessing data read from gpc.def
 */


#if !defined(__GPCDATATMPL_H__)
#define	__GPCDATATMPL_H__


#include "GpcDataStruct.h"
#include <afxtempl.h>
#include <vector>

template<class T>
class GpcDataTmpl
{
// Constructor and Destructor
public:
	GpcDataTmpl() { }
	~GpcDataTmpl(){	}

// Operations
public:
	T*	getContentFromID( int id )
	{
		for(int i=0; i<size(); i++)
			if(data[i].getID() == id)	return &data[i];
		return NULL;
	}

	T&	operator [] (int idx)
	{
		ASSERT(	idx >= 0 && idx < size() );
		return data[idx];
	}

	int size()
	{
		return data.size();
		//return data.GetSize();
	}

	BOOL saveContent(T&  unit)
	{
		data.push_back(unit);
		//data.Add( unit );
		return TRUE;
	}
	
	BOOL loadContent( char * ptr )
	{
		T	tmp;
		if( ! tmp.loadContent(ptr) )
			return FALSE;
		data.push_back(tmp);
		//data.Add( tmp );
		return TRUE;
	}

public:
	std::vector<T> data;
	//CArray<T, T> data;	2010下不能使用该方法
};






template<class T> struct _type_traits;
template<>
struct _type_traits<int>
{
	static int convert(char *ptr){ return atoi(ptr); }
	typedef int value_type;
};
template<>
struct _type_traits<char>
{
	static CString convert(char *ptr){ return ptr; }
	typedef char value_type;
};

//Specialization
template<>
class GpcDataTmpl<int>
{
// Constructor and Destructor
public:
	GpcDataTmpl() : data(0) { }
	virtual ~GpcDataTmpl(){	}

// Operations
public:
	int	 getContent();
	BOOL loadContent(char *ptr);
	BOOL operator()()
	{
		return  data;
	}
	GpcDataTmpl<int>& operator=(const int& rhs)
	{
		data = rhs;
		return *this;
	}

private:
	int data;
};
typedef GpcDataTmpl<int>	gpcINT;


//Specialization
template<>
class GpcDataTmpl<CString>
{
	//ctor & dtor
public:
	GpcDataTmpl() {}
	virtual ~GpcDataTmpl() {}

	//Operations
public:
	CString  getContent();
	BOOL  loadContent(char *ptr);
private:
	CString  data;
};

#endif