// Config.h: interface for the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_H__44C22E09_F22E_45F3_BD64_E613AEF940FF__INCLUDED_)
#define AFX_CONFIG_H__44C22E09_F22E_45F3_BD64_E613AEF940FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)

#include <map>
#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;


namespace casco { namespace clbth {


// 读配置文件的返回状态
enum {	// config file state
	S_XLY_OK	= 1,	// 正确
	S_XLY_FALSE	= 0,	// 错误, 但不影响
	E_FAILED  = -1	// 错误, 程序必须退出
};


struct LOAD_PARAM {
	ifstream& in;
	const string& filename;
	string szKeyword;
	void *pOwner;
	LOAD_PARAM(ifstream& _in, const string& _filename, void *_pOwner)
		: in(_in), filename(_filename), pOwner(_pOwner) {}
};

struct IOFUNC
{
	struct ITEM {
		DWORD  dwOffset;
		string szKeyword;
		string szNotation;
		ITEM() : dwOffset(0) {}
		ITEM(const string& keyword, DWORD nOff, const string& notation = "")
			: szKeyword(keyword), dwOffset(nOff), szNotation(notation) {}
	};
	virtual int  _load(LOAD_PARAM& param) = 0;
	virtual bool _save(ofstream& out, ITEM& item) = 0;

	typedef list< ITEM > container_t;
	static container_t::iterator find(container_t& c, const string& szKeyword)
	{
		container_t::iterator iter = c.begin();
		for(; iter != c.end(); iter ++)
		{
			if(szKeyword == (*iter).szKeyword)
				return iter;
		}
		return c.end();
	}
	struct INSERT {
		INSERT(container_t& c, const string& szKeyword, DWORD dwOffset, 
			const string& szNotation = "")
		{
			container_t::iterator iter = c.begin();
			for(; iter != c.end() && (*iter).dwOffset < dwOffset; iter ++)
				ASSERT(szKeyword != (*iter).szKeyword);

			if(iter != c.end())
				ASSERT(dwOffset != (*iter).dwOffset);
			c.insert(iter, ITEM(szKeyword, dwOffset, szNotation));
			
		}
	};
	static IOFUNC* getIOFUNC(void *p, container_t::iterator iter)
	{
		return (IOFUNC*)((DWORD)p + (*iter).dwOffset);
	}
	static IOFUNC* getIOFUNC(void *p, container_t& c, const string& szKeyword)
	{
		IOFUNC::container_t::iterator iter
			= find(c, szKeyword);
		if( iter != c.end())
			return (IOFUNC*)((DWORD)p + (*iter).dwOffset);
		else
			return NULL;
	}
};


// 除去前导无效字符集
// ; 定义为注释行
extern inline char trimleft(ifstream& in, const char * sufs)
{
	if(in.eof())	return -1;

	char buffer[256];
	char c = 0;
	do {
		if(in.eof())	break;

		in.get(c);
		if(c == ';')	// 注释行
		{
			in.getline(buffer, 256, '\n');
			continue;
		}
	} while(strchr(sufs, c) || c == ';' || c == '\n' || c == '\r');

	if(NULL == strchr(sufs, c) && c != ';' && c != '\n' && c != '\r')
	{
		//in.unget();
		in.seekg(-1, ios::cur);
		return c;
	}
	return -1;
}

extern inline void getCurLine(ifstream& in, int *nLineNo, string& szLineContent)
{
	*nLineNo = 0;
	ifstream::pos_type cur_pos = in.tellg();
	in.seekg(0, ios::beg);
	ifstream::pos_type pos;
	char buf[1024];
	while( (pos = in.tellg()) < cur_pos )
	{
		(*nLineNo) ++;
		in.getline(buf, 1024, '\n');
	}
	szLineContent = buf;
	in.seekg(cur_pos, ios::beg);
}



/////////////////////////////////////////////////////////////////////
// ComplexData
/////////////////////////////////////////////////////////////////////
template<class T> struct BaseComplexData
{
	typedef int Please_Declare_T_as_ComplexData_Type;
	typedef typename T myt;
	bool	canSort() { return false; }
};

struct UnknownKeyword : public BaseComplexData<UnknownKeyword>
{
	string szKeyword;
	string sz;

	UnknownKeyword() {}
	UnknownKeyword(const UnknownKeyword& rhs)
	{
		szKeyword = rhs.szKeyword;
		sz = rhs.sz;
	}
	UnknownKeyword& operator=(const UnknownKeyword& rhs)
	{
		szKeyword = rhs.szKeyword;
		sz = rhs.sz;
		return *this;
	}
};

template<typename T> struct ComplexDataContainer : public IOFUNC
{
	typedef typename T::Please_Declare_T_as_ComplexData_Type NotSupported;

	ComplexDataContainer() : m_bSkipAllError(FALSE) {}
	vector<T>  data;
	T* getContentFromID(int id);
	int getPosFromID(int id);
	T& operator[](int idx);
	int size();
	void add(T val);
	bool sort();
private:
	int  _load(LOAD_PARAM& param);
	bool _save(ofstream& out, ITEM& item);

	BOOL m_bSkipAllError;
};

template<typename T>
T* ComplexDataContainer<T>::getContentFromID(int id)
{
	vector<T>::iterator  iter = data.begin();
	for(; iter < data.end(); iter ++)
		if((*iter).getID() == id)	return &(*iter);
	return NULL;
}


template<typename T>
int ComplexDataContainer<T>::getPosFromID(int id)
{
	int pos=0;
	vector<T>::iterator  iter = data.begin();
	for(; iter < data.end(); iter ++)
	{
		if((*iter).getID() == id)	
			return pos;		
			pos++;
	}
	return -1;
}

template<typename T>
T& ComplexDataContainer<T>::operator[](int idx)
{
	ASSERT(idx >=0 && idx < size());	return data[idx];
}

template<typename T>
int ComplexDataContainer<T>::size()
{
	return data.size();
}

template<typename T>
int ComplexDataContainer<T>::_load(LOAD_PARAM& param)
{
	int nResult = S_XLY_OK;

	char c = trimleft(param.in, " \t");
	if(param.in.eof())
		return S_XLY_FALSE;

	while(!param.in.eof())
	{
		c = trimleft(param.in, " \t");
		if(param.in.eof() || c == '[')	break;

		T  tmp;
		int res = tmp.loadFrom(param);
		
		string errdesc;
		
		if( res == E_FAILED )
			return E_FAILED;
		
		else if(S_XLY_OK == res)
		{
			vector<T>::iterator  iter = data.begin();
			for(; iter < data.end(); iter ++)
			{
				errdesc.erase(errdesc.begin(), errdesc.end());
				if((*iter).getID() == tmp.getID())
				{	// ID 重复，忽略或退出系统
					errdesc = "唯一标识出现重复";
					break;
				}
				res = tmp.IsValidData(*iter, errdesc);
				if(S_XLY_OK != res)
					break;
			}
			if(iter == data.end())
				data.push_back(tmp);
			else
				res = S_XLY_FALSE;
		}
		else
		{
			res = S_XLY_FALSE;
			//continue;
		}
		
		if(S_XLY_OK != res)
		{
			nResult = S_XLY_FALSE;
			if(!m_bSkipAllError)
			{
				int nLineNo;
				string szLineContent;
				getCurLine(param.in, &nLineNo, szLineContent);
				CString prompt;
				prompt.Format("错误位置: 文件%s 第%d行\n",
					param.filename.c_str(), nLineNo);
				prompt = prompt
					+ "错误内容: " + szLineContent.c_str() + "\n"
					+ "错误简述: " + errdesc.c_str() + "\n"
					+ "解决方案: YES--Skip? NO--Skip "
							+ param.szKeyword.c_str()
							+ "'s all error? CANCEL--Quit?";

				int res = AfxMessageBox(prompt, MB_ICONQUESTION|MB_YESNOCANCEL);
				if(IDNO == res)
					m_bSkipAllError = TRUE;
				else if(IDCANCEL == res)
				{
					prompt.Format("系统退出, 请修正文件 %s 的错误 %s",
						param.filename.c_str(), szLineContent.c_str());
					AfxMessageBox(prompt);
					ExitProcess(1);
					return E_FAILED;
				}
			}
		}
	}
	return nResult;
}

template<typename T>
bool ComplexDataContainer<T>::_save(ofstream& out, ITEM& item)
{
	if( data.size() != 0 )
	{
		if(!item.szNotation.empty() && item.szNotation != "; ")
			out << item.szNotation.c_str() << "\n";

		string format = T::formatString();
		if(!format.empty())
			out << format << "\n";

		if(!item.szKeyword.empty())
		{
			out << item.szKeyword.c_str();
			if(data.size() == 1)
				out << " ";
			else
				out << "\n";
		}

		vector<T>::iterator iter = data.begin();
		for(; iter < data.end(); iter ++)
		{
			if( !(*iter).save(out) )
				return false;
			out << "\n";
		}

		out << "\n";
	}
	return true;
}

template<typename T>
void ComplexDataContainer<T>::add(T val)
{
	data.push_back(val);

}

template<typename T>
bool ComplexDataContainer<T>::sort()
{
	if( T::canSort() )		
	{
		
		std::sort(data.begin(), data.end(), less<T>() );
		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////////////////

template<> struct ComplexDataContainer<UnknownKeyword> : public IOFUNC
{
//	typedef typename UnknownKeyword::Please_Declare_T_as_ComplexData_Type NotSupported;

	ComplexDataContainer() : m_bSkipAllError(FALSE) {}
	vector<UnknownKeyword>  data;
	UnknownKeyword* getContentFromID(int id)
	{
		//vector<UnknownKeyword>::iterator  iter = data.begin();
		//for(; iter < data.end(); iter ++)
		//	if((*iter).getID() == id)	return &(*iter);
		return NULL;
	}
	UnknownKeyword& operator[](int idx)
	{
		ASSERT(idx >=0 && idx < size());	return data[idx];
	}
	int size()
	{
		return data.size();
	}

	int  _load(LOAD_PARAM& param)
	{
		UnknownKeyword  tmp;
		tmp.szKeyword = param.szKeyword;

		int nResult = S_XLY_OK;
		char buffer[1024];

		while(!param.in.eof())
		{
			char c = 0;
			param.in.get(c);
			if(c=='[')
			{
				param.in.unget();
				break;
			}

			tmp.sz += c;

			if(c == ';')	// 注释行
			{
				param.in.getline(buffer, 1024, '\n');
				tmp.sz += buffer;
			}
		}
		data.push_back(tmp);
		return nResult;
	}
	bool _save(ofstream& out, ITEM& item)
	{
		if( data.size() != 0 )
		{
			vector<UnknownKeyword>::iterator iter = data.begin();
			for(; iter < data.end(); iter ++)
			{
				const UnknownKeyword& u = *iter;
				if(!u.szKeyword.empty())
					out << u.szKeyword.c_str();

				out << (*iter).sz;
				out << "\n";
			}
		}
		return true;
	}

	BOOL m_bSkipAllError;
};

/////////////////////////////////////////////////////////////////////
// SimpleData
/////////////////////////////////////////////////////////////////////
struct SimpleDataContext
{
	typedef map<DWORD, void*> map_t;
	SimpleDataContext(map_t& iomap, DWORD key, void* p)
		{	iomap[key] = p;	}
};

template<typename T> int load(T& value, LOAD_PARAM& param)
{
	return value.loadFrom(param);
}
template<typename T> bool save(T& value, ofstream& out)
{
	return value.save(out);
}
template<typename T> string formatString(T& value)
{
	return T::formatString();
}

template<> inline int load(int& value, LOAD_PARAM& param)
{
		char c = trimleft(param.in, " \t");
		if(c == '[' || param.in.eof())
			return S_XLY_FALSE;

		char buffer[256];
		param.in.getline(buffer, 256, '\n');

		char *token;
		char seps[]=" ,\t\\/=\n";

		if( token = strtok(buffer, seps) )
			value = atoi(token);
		return S_XLY_OK;
}
template<> inline bool save(int& value, ofstream& out)
{
		out << value;
		return true;
};
template<> inline string formatString(int& value)
{
	return "; 格式: =数值";
}
template<> inline int load(string& value, LOAD_PARAM& param)
{
		char c = trimleft(param.in, " \t");
		if(c == '[' || param.in.eof())
			return S_XLY_FALSE;

		char buffer[256];
		param.in.getline(buffer, 256, '\n');

		char *token;
		char seps[]=" \t=\r\n";

		if( token = strtok(buffer, seps) )
		{
			value = token;
			string::size_type pos = value.find('\r');
			if(pos != string::npos)
				value = value.substr(0, pos);
		}
		return S_XLY_OK;
}
template<> inline bool save(string& value, ofstream& out)
{
		out << value.c_str();
		return true;
};
template<> inline string formatString(string& value)
{
	return "; 格式: =字符串";
}

template<typename T> struct SimpleData : public IOFUNC
{
	T value;
	int  _load(LOAD_PARAM& param)
	{
		return load(value, param);
	}
	bool _save(ofstream& out, ITEM& item)
	{
		if(!item.szNotation.empty())
			out << item.szNotation.c_str() << "\n";

		string format = formatString(value);
		if(!format.empty())
			out << format << "\n";

		if(!item.szKeyword.empty())
			out << item.szKeyword.c_str() << "=";

		if(!save(value, out))
			return false;

		out << "\n";

		return true;
	}
};
/////////////////////////////////////////////////////////////////////




#define BEGIN_DATA	 public:
#define END_DATA

///////////////////////////////////////////////////////////////////////////////
// Used to serailize ComplexData
///////////////////////////////////////////////////////////////////////////////
#define DECLARE_COMPLEXDATA_PART(type, var)                                   \
	casco::clbth::ComplexDataContainer<type>  var;
#define IMPLEMENT_COMPLEXDATA_PART(class, type, var, KEYWORD, NOTATION)       \
	static casco::clbth::IOFUNC::INSERT __INSERT##class##type##var(           \
		class::getIOContainer(), "[" #KEYWORD "]", (DWORD)&(((class*)0)->var), "; 说明:" NOTATION);
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Used to serailize SimpleData
///////////////////////////////////////////////////////////////////////////////
#define DECLARE_SIMPLEDATA_PART(type, var) casco::clbth::SimpleData<type>  var;
#define IMPLEMENT_SIMPLEDATA_PART(class,type,var,KEYWORD,DEFVALUE,NOTATION)   \
	static void __init##class##type##var(void *pOwner, DWORD dwOffset)        \
	{                                                                         \
		casco::clbth::SimpleData<type>* p =                                   \
			(casco::clbth::SimpleData<type>*)((BYTE*)pOwner + dwOffset);      \
			p->value = type(DEFVALUE);                                        \
	}                                                                         \
	static casco::clbth::SimpleDataContext __SDC##class##type##var(           \
		class::getSimpleDataContextMap(),                                     \
		(DWORD)&(((class*)0)->var),                                           \
		(void*)__init##class##type##var);                                     \
	static casco::clbth::IOFUNC::INSERT __INSERT##class##type##var(           \
		class::getIOContainer(), "[" #KEYWORD "]", (DWORD)&(((class*)0)->var), "; 说明:" NOTATION);
///////////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_SIMPLEDATA_PART2(class,type,var,KEYWORD,NOTATION)   \
	static casco::clbth::IOFUNC::INSERT __INSERT##class##type##var(   \
		class::getIOContainer(), "[" #KEYWORD "]", (DWORD)&(((class*)0)->var), "; 说明:" NOTATION);
///////////////////////////////////////////////////////////////////////////////


template<class T> class IOConfig
{
protected:
	IOConfig() : bInitialize_Simpledata(false) {}
public:
	virtual ~IOConfig() {}

//DECLARE_GPCDATA_SERIALIZE()
public:
	bool  load(string filename);
	bool  save(string filename);
	static IOFUNC::container_t& getIOContainer();
	static SimpleDataContext::map_t& getSimpleDataContextMap();
protected:
	void INITIALIZE_SIMPLEDATA(void *p);
private:
	//bool loadTAG(ifstream& in, string& tag);
	bool bInitialize_Simpledata;
	ComplexDataContainer<UnknownKeyword> __unknownKeyword__;
};

//#define INITIALIZE_SIMPLEDATA(this)
template<class T> void IOConfig<T>::INITIALIZE_SIMPLEDATA(void *p)
{
	SimpleDataContext::map_t::iterator iter
				= getSimpleDataContextMap().begin();
	for(; iter!=getSimpleDataContextMap().end(); iter++)
	{
		((void (*)(void*, DWORD))iter->second)(p, (*iter).first);
	}
	bInitialize_Simpledata = true;
}

//IMPLEMENT_GPCDATA_SERIALIZE(class)
template<class T>
IOFUNC::container_t& IOConfig<T>::getIOContainer()
{
	static IOFUNC::container_t iomap;
	return iomap;
}

template<class T>
SimpleDataContext::map_t& IOConfig<T>::getSimpleDataContextMap()
{
	static SimpleDataContext::map_t io_container;
	return io_container;
}

extern inline bool hasTAG(ifstream& in)
{
	char c = trimleft(in, " \t");
	if(in.eof())
		return false;
	return c == '[';
}

//template<class T>
//bool IOConfig<T>::loadTAG(ifstream& in, string& tag)
inline bool loadTAG(ifstream& in, string& tag)
{
	if(!hasTAG(in))
		return false;

	tag.erase(tag.begin(), tag.end());
	//char c = trimleft(in, " \t");
	//if(in.eof())
	//	return false;

	//if(c != '[')
	//	return false;
	char c;
	in.get(c);
	do {
		tag.append(1, c);
		in.get(c);
		if(c == '[' || c == ' ' && c == '\t' && c == '\n')
			break;
	} while(!in.eof() && c != ']');
	if(c == ']')
	{
		tag.append(1, c);
		return true;
	}
	else
	{
		tag.erase(tag.begin(), tag.end());
		return false;
	}
}

template<class T>
bool  IOConfig<T>::load(string filename)
{
	ASSERT(bInitialize_Simpledata);

	ifstream in(filename.c_str(), ios::in | ios::binary);
	if(in.fail())
	{
		CString prompt;
		prompt.Format("没有配置文件%s!", filename.c_str());
		AfxMessageBox(prompt);
		return false;
	}

	LOAD_PARAM param(in, filename, this);
	string szKeyword;
	bool bHasTag = loadTAG(in, szKeyword);
	bool bResult = true;
	BOOL bSkipAllError = FALSE;
	while(!in.eof() && bHasTag)
	{
		param.szKeyword = szKeyword;
		IOFUNC *pfn = IOFUNC::getIOFUNC(this, getIOContainer(), szKeyword);
		if(NULL == pfn)
			pfn = &__unknownKeyword__;//._load(param);
		if(NULL == pfn || E_FAILED == pfn->_load(param))
		{
			int nLineNo = 0;
			string szLineContent;
			getCurLine(in, &nLineNo, szLineContent);
			CString prompt;
			prompt.Format("错误位置: 文件%s 第%d行\n",
				filename.c_str(), nLineNo);
			prompt = prompt + "错误内容: " + szLineContent.c_str() + "\n";

			prompt = prompt + "错误简述: ";
			if(NULL == pfn)
				prompt = prompt + "不认识的关键字" + szKeyword.c_str();
			else
				prompt = prompt + "未知错误!";
			prompt = prompt + "\n解决方案: YES--Skip? NO--Skip all error? CANCEL--Quit?";

			if(!bSkipAllError)
			{
				int res = AfxMessageBox(prompt, MB_ICONQUESTION|MB_YESNOCANCEL);
				if(IDNO == res)
					bSkipAllError = TRUE;
				else if(IDCANCEL == res)
					ExitProcess(1);
			}

			bResult = false;	break;
		}
		bHasTag = loadTAG(in, szKeyword);
	}
	return bResult;
}

template<class T>
bool  IOConfig<T>::save(string filename)
{
	ASSERT(bInitialize_Simpledata);

	ofstream out(filename.c_str());
    if (out.fail())
		return false;

	out << ";用户配置文件\n\n";
	IOFUNC::container_t::iterator iter = getIOContainer().begin();
	for(; iter != getIOContainer().end(); iter ++)
	{
		if( !IOFUNC::getIOFUNC(this, iter)->_save(out, *iter) )
			return false;

		out << "\n";
	}

	__unknownKeyword__._save(out, IOFUNC::ITEM());//*iter

	return true;
}





} } // } namespace clbth } namespace casco


#endif // !defined(AFX_CONFIG_H__44C22E09_F22E_45F3_BD64_E613AEF940FF__INCLUDED_)
