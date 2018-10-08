
#if !defined(__BASEMSG_H__)
#define __BASEMSG_H__

#include "clbth_config.h"
#include "clbth_utility.h"
#include "msgid.h"


CLBTH_NAMESPACE_BEGIN


struct HRCODE {
	DWORD user_code : 16;
	DWORD sys_code : 15;
	DWORD bSuccess : 1;

	enum SF { HC_SUCCESS = 0, HC_FAIL = 1 };
	enum {
		E_WRITE_OUTOFBUFFER = 1,
		E_READ_OUTOFBUFFER,
		E_MSGLEN_NEQ_DATALEN,
		E_MSGLEN_LT_MSGHEADSIZE,	// 必须 >=4
		E_MSGID_RESERVE,			// != 0和1
		E_OBJECT_NOTCREATED_BY_MSGID,	// 建议BaseMsgFactory::createObject
		E_UNREGISTER_DEFAULT_PROC,
		E_UNREGISTER_UNIFIEDMSGPRO_OR_LISTENER,
		E_NOENOUGH_MEMORY,
		E_SEEK_OUTOFBUFFER,
		S_MSGLEN_INCORRECT,
		S_ARCHIVE_NOMORE_DATA
	};

	enum MODE {USER, SYS};
	HRCODE(SF sf = HC_FAIL, WORD w = 1, MODE m=USER)
	{
		if(sf == HC_SUCCESS && w == 0)
			w = 1;
		bSuccess = sf;
		if(m == USER)
			user_code = w;
		else
			sys_code = w;
	}
	HRCODE(const HRCODE& rhs) : bSuccess(rhs.bSuccess), 
		user_code(rhs.user_code), sys_code(rhs.sys_code) {}
	HRCODE& operator=(const HRCODE& rhs)
	{
		bSuccess = rhs.bSuccess;
		sys_code = rhs.sys_code;
		user_code = rhs.user_code;
		return *this;
	}
	operator BOOL() { return succeed(); }
	//operator DWORD() { return *(DWORD*)this; }
	BOOL succeed() { return bSuccess == HC_SUCCESS; }
	BOOL failed()  { return bSuccess == HC_FAIL; }
};



class Archive;

struct MSGHEAD;

class BaseMsg;	// BaseMsg
class BaseMsgFactory;
class BaseMsgVisitor;
class EventSink;

struct Plex;
class MapWordToPtr;
class Archive;



/////////////////////////////////////////////////////////////////////
struct MSGHEAD
{
	typedef MapWordToPtr  lsn_container_t;
	typedef lsn_container_t::BASE_KEY  msgid_t;
	typedef USHORT len_t;

	len_t  length;
	msgid_t  id;
	enum { SIZEOF = sizeof(len_t) + sizeof(msgid_t) };
	MSGHEAD() { memset(this, 0, sizeof(MSGHEAD)); }
};

EXTERN Archive& operator<<(Archive& ar, const MSGHEAD& head);
EXTERN Archive& operator>>(Archive& ar, MSGHEAD& head);




#define DECLARE_DYNCREATE_BASEMSG(classname)                              \
	public:                                                               \
		classname(const classname& rhs);                                  \
		classname& operator=(const classname& rhs);                       \
		virtual BOOL isKindOf(MSGHEAD::msgid_t nTypeId);                  \
		virtual MSGHEAD::msgid_t getTypeIDVirtual() const;                \
		static  MSGHEAD::msgid_t m_nTypeID;                               \
		static  MSGHEAD::msgid_t getTypeIDStatic();                       \
		static  BaseMsg* createObject();                                  \
		static  HRCODE processObject(Archive& ar, BASEMSGPROC proc, VPPARAM& vpParam);     \
		virtual size_t getObjectLength();                                 \
	friend class Archive; 

#define IMPLEMENT_DYNCREATE_BASEMSG(classname, nTypeID)                   \
	MSGHEAD::msgid_t classname::m_nTypeID = nTypeID;                      \
	MSGHEAD::msgid_t classname::getTypeIDStatic()                         \
	{                                                                     \
		static const MSGHEAD::msgid_t m_nTypeID = nTypeID;                \
		return m_nTypeID;                                                 \
	}                                                                     \
	MSGHEAD::msgid_t classname::getTypeIDVirtual() const                  \
	{                                                                     \
		return getTypeIDStatic();                                         \
	}                                                                     \
	BOOL classname::isKindOf(MSGHEAD::msgid_t nTypeId)                    \
	{                                                                     \
		return getTypeIDVirtual() == nTypeId;                             \
	}                                                                     \
	BaseMsg* classname::createObject()                                    \
	{                                                                     \
		return new classname;                                             \
	}                                                                     \
	HRCODE classname::processObject(Archive& ar, BASEMSGPROC proc, VPPARAM& vpParam)  \
	{                                                                     \
		classname obj;                                                    \
		HRCODE hr = obj.serialize(ar);                                    \
		if(hr.failed())                                                   \
			return hr;                                                    \
		vpParam.pBaseMsg = &obj;                                          \
		return proc(vpParam);                                             \
	}                                                                     \
	size_t classname::getObjectLength()                                   \
	{                                                                     \
		size_t size = countObjectLength();                                \
		if(size >= 0)	return size + MSGHEAD::SIZEOF;                    \
		return sizeof(classname) - sizeof(BaseMsg) + MSGHEAD::SIZEOF;     \
	}                                                                     \
	static char szClassName_##classname[] = #classname;                   \
	static char szTypeID_##classname[] = #nTypeID;                        \
	static struct BaseMsgInit __BaseMsgInit##classname(                   \
		classname::createObject, classname::processObject,                \
		szClassName_##classname, szTypeID_##classname);                   \
	static struct MSGHEAD::lsn_container_t::AutoInsertItem                \
		aii_static_creator##classname##nTypeID(                           \
				classname::getTypeIDStatic(),                             \
				(void*)&__BaseMsgInit##classname,                         \
				BaseMsgFactory::getCreatorMap());


#define DECLARE_SERIALIZE_OPERATOR(classname)                             \
	//EXTERN Archive& operator>>(Archive& ar, classname** p);               \
	//EXTERN Archive& operator>>(Archive& ar, classname& m); 

#define IMPLEMENT_SERIALIZE_OPERATOR(classname)                           \
	/*EXTERN Archive& operator>>(Archive& ar, classname** p)                \
		{ ASSERT(NULL!=p && NULL==*p);                                    \
		  MSGHEAD msghead = BaseMsg::getMsgHead(ar);                      \
		  if(msghead.id == classname::getTypeIDStatic())                  \
			*p =(classname*) ar.readObject(0);                            \
		  else ASSERT(FALSE);                                             \
		  return ar; }                                                    \
	EXTERN Archive& operator>>(Archive& ar, classname& m)                   \
		{ MSGHEAD msghead = BaseMsg::getMsgHead(ar);                      \
		  if(msghead.id == m.getTypeIDStatic())                           \
			{ m.~classname(); new (&m) classname(); m.serialize(ar); }    \
		  else ASSERT(FALSE);                                             \
		  return ar; }*/

// [Begin] add this macro for msg of WintgTerm and TGOcx
#define DECLARE_DYNCREATE_BASEMSG_OCX(classname)                          \
	public:                                                               \
		virtual BOOL isKindOf(MSGHEAD::msgid_t nTypeId);                  \
		virtual MSGHEAD::msgid_t getTypeIDVirtual() const;                \
		static  MSGHEAD::msgid_t m_nTypeID;                               \
		static  MSGHEAD::msgid_t getTypeIDStatic();                       \
		static  BaseMsg* createObject();                                  \
		static  HRCODE processObject(Archive& ar, BASEMSGPROC proc, VPPARAM& vpParam);     \
		virtual size_t getObjectLength();                                 \
	friend class Archive; 

#define DECLARE_CLBTH_OCX(classname)                                          \
	DECLARE_DYNCREATE_BASEMSG_OCX(classname)                                  \
	public:                                                               \
		virtual void accept(BaseMsgVisitor *pVisitor);                    \
	friend EXTERN_FRIEND Archive& operator>>(Archive& ar, classname** p); \
	friend EXTERN_FRIEND Archive& operator>>(Archive& ar, classname& m);
// [End]add this macro for msg of WintgTerm and TGOcx

// the following two macro only use in BaseMsg's subclass
#define DECLARE_CLBTH(classname)                                          \
	DECLARE_DYNCREATE_BASEMSG(classname)                                  \
	public:                                                               \
		virtual void accept(BaseMsgVisitor *pVisitor);                    \
	friend EXTERN_FRIEND Archive& operator>>(Archive& ar, classname** p); \
	friend EXTERN_FRIEND Archive& operator>>(Archive& ar, classname& m);

#define IMPLEMENT_CLBTH(classname, nTypeID)                               \
	IMPLEMENT_DYNCREATE_BASEMSG(classname, nTypeID)                       \
	void classname::accept(BaseMsgVisitor *pVisitor)                      \
		{	pVisitor->visit(this);	}

// zero fill everything after the vtbl pointer
#define CLBTH_ZERO_INIT_OBJECT(base_class) \
	memset(((base_class*)this)+1, 0, sizeof(*this) - sizeof(class base_class));

/////////////////////////////////////////////////////////////////////
// declare and define process function responsing each msg class
////////////////////////////////////////////////////////////////////
struct VPPARAM // Visitor Proc Parameters
{
	VPPARAM() : inParam(0), msgid(0), buf_ptr(0), buf_len(0) {}
	const void *inParam;
	BaseMsg  *pBaseMsg;
	MSGHEAD::msgid_t msgid;
	void *buf_ptr;
	MSGHEAD::len_t buf_len;
};

//	typedef HRCODE (*DEFPROC)(const VPPARAM& vpParam);
typedef HRCODE (*BASEMSGPROC)(const VPPARAM& vpParam);
typedef BASEMSGPROC UINIFIEDMSGPROC;


//#define IMPORT_C_VISITOR(visitor_name)                                    \
//	extern CLBTH::MSGHEAD::lsn_container_t visitor_##visitor_name
//#define DECLARE_C_VISITOR(visitor_name)                                   \
//	CLBTH::MSGHEAD::lsn_container_t visitor_##visitor_name
//#define GET_C_VISITOR(visitor_name)  visitor_##visitor_name

#define IMPORT_C_VISITOR(visitor_name)                                    \
	MODULE_IMPORT CLBTH::MSGHEAD::lsn_container_t& getVisitor_##visitor_name();

#define DECLARE_C_VISITOR(visitor_name)                                   \
	MODULE_EXPORT CLBTH::MSGHEAD::lsn_container_t& getVisitor_##visitor_name()   \
	{                                                                     \
		static CLBTH::MSGHEAD::lsn_container_t visitor;                   \
		return visitor;                                                   \
	}

#define GET_C_VISITOR(visitor_name)  getVisitor_##visitor_name()

#define DEFINE_BASEMSG_DEFPROC(outparam, visitor_name)                    \
	static CLBTH::HRCODE process_default##visitor_name(const CLBTH::VPPARAM& vpParam);  \
	static struct CLBTH::MSGHEAD::lsn_container_t::AutoInsertItem         \
			__aii_default##visitor_name(                                  \
				(CLBTH::MSGHEAD::msgid_t)0,                               \
				(void*)process_default##visitor_name,                     \
				GET_C_VISITOR(visitor_name));                             \
	static CLBTH::HRCODE process_default##visitor_name(const CLBTH::VPPARAM& outparam)

#define DEFINE_BASEMSG_PROC(class, ptr, outparam, visitor_name)           \
	static CLBTH::HRCODE PASCAL process(CLBTH::class *p, const CLBTH::VPPARAM& vpParam);\
	CLBTH::HRCODE process##class##visitor_name(const CLBTH::VPPARAM& param)\
	{                                                                     \
		ASSERT(NULL != param.pBaseMsg);                                   \
		ASSERT(param.pBaseMsg->getTypeIDVirtual()==CLBTH::class::getTypeIDStatic());\
		return process((CLBTH::class*)param.pBaseMsg, param);             \
	}                                                                     \
	static struct CLBTH::MSGHEAD::lsn_container_t::AutoInsertItem         \
			__aii_##class##visitor_name(                                  \
				CLBTH::class::getTypeIDStatic(),                          \
				(void*)process##class##visitor_name,                      \
				GET_C_VISITOR(visitor_name));                             \
	static CLBTH::HRCODE PASCAL process(CLBTH::class *ptr, const CLBTH::VPPARAM& outparam) 
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// Compatible to casco old msg proc
////////////////////////////////////////////////////////////////////
typedef int (*CASCOOLDMSG_PROC)(void *ptr);
struct AFX_EXT_CLASS CASCOOLDMSG_ENTRY {
	MSGHEAD::msgid_t id;
	CASCOOLDMSG_PROC proc;
};
#define BEGIN_CASCOOLDMSG_MAP(listener) \
	static CLBTH::CASCOOLDMSG_ENTRY Cascooldmsglist_##listener[] = {
#define ON_CASCOOLDMSG(id, proc) {id, (CLBTH::CASCOOLDMSG_PROC)proc},
#define END_CASCOOLDMESG_MAP(listener) \
	{0, 0}}; \
	MODULE_EXPORT CLBTH::CASCOOLDMSG_ENTRY* getCascoOldmsgList_##listener() \
	{ \
		return &Cascooldmsglist_##listener[0]; \
	}
#define IMPORT_CASCOOLDMSG_MAP(listener) \
	MODULE_IMPORT CLBTH::CASCOOLDMSG_ENTRY* getCascoOldmsgList_##listener();
#define GET_CASCOOLDMSG_MAP(listener) getCascoOldmsgList_##listener()
inline CLBTH::CASCOOLDMSG_PROC GetCascoOldmsgProc(
	CLBTH::MSGHEAD::msgid_t id,
	CLBTH::CASCOOLDMSG_ENTRY *entries)
{
	for(; 0 != entries->id && NULL != entries->proc; entries++)
	{
		if(id == entries->id)
			return entries->proc;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////
//  class BaseMsgVisitor
/////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS BaseMsgVisitor
{
protected:
	BaseMsgVisitor() {}
public:
	virtual ~BaseMsgVisitor() {}

	virtual void visit(BaseMsg *pBaseMsg) = 0;
};
/////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////
//  class BaseMsg    BaseMsgProtocol
/////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS BaseMsg
{
	DECLARE_DYNCREATE_BASEMSG(BaseMsg)

	// ctor & dtor
public:
	typedef BaseMsg* (*CREATOR)();
	typedef HRCODE (*PROCESSOR)(Archive& ar, BASEMSGPROC, VPPARAM& vpParam);

	virtual ~BaseMsg();
	BaseMsg* clone();

//protected:
	BaseMsg();	// DYNAMIC CREATE
	void* operator new(size_t nSize);
	void* operator new(size_t nSize, void *p);
	void  operator delete(void *p);
#ifndef __AIXC__
	void  operator delete(void *p, void*pPlace);
#endif
//private:
//	BaseMsg(const BaseMsg& rhs);	// INHIBIT copy constructor
//	BaseMsg& operator=(const BaseMsg& rhs);	//INHIBIT assign operator

public:
	// read object's identifier from archive
	static MSGHEAD getMsgHead(Archive& ar);
	HRCODE serialize(Archive& ar);
	virtual INT toString(void *buf, INT buflen);
#ifdef _MSC_VER
	virtual string toString();
#endif
private:
	virtual HRCODE doSerialize(Archive& ar);
	//MSGHEAD msghead;
protected:
	virtual size_t countObjectLength();

public:
	void deferDelete();
	void endDeferDelete();
	bool isDeferDelete();
private:
	BOOL m_bDeferDelete;
	int m_refDeferDelete;
	friend class BaseMsgFactory;

	//friend Archive& operator>>(Archive& ar, BaseMsg**); 
	// Object I/O is pointer based to avoid added construction overhead.
	// Use the Serialize member function directly for embedded objects.
	friend EXTERN_FRIEND Archive& operator<<(Archive& ar, const BaseMsg* pBaseMsg);
	friend EXTERN_FRIEND Archive& operator<<(Archive& ar, const BaseMsg& rBaseMsg);

	friend EXTERN_FRIEND Archive& operator>>(Archive& ar, BaseMsg** pBaseMsg);
	friend EXTERN_FRIEND Archive& operator>>(Archive& ar, const BaseMsg** pBaseMsg);
	friend EXTERN_FRIEND Archive& operator>>(Archive& ar, BaseMsg& mBaseMsg);

	// special use for LiRC, only
public:
	// special use too
	static MSGHEAD getMsgHead(void *buf, int buflen);
	static void moveToNextMsg(Archive& ar);
	HRCODE serialize(INT mode, void** buf, size_t len);
	HRCODE doSerialize(INT mode, void** buf, size_t len);
};

EXTERN Archive& operator<<(Archive& ar, const BaseMsg* pBaseMsg);
EXTERN Archive& operator<<(Archive& ar, const BaseMsg& rBaseMsg);
EXTERN Archive& operator>>(Archive& ar, BaseMsg** pBaseMsg);
EXTERN Archive& operator>>(Archive& ar, const BaseMsg** pBaseMsg);
EXTERN Archive& operator>>(Archive& ar, BaseMsg& mBaseMsg);
/////////////////////////////////////////////////////////////////////




struct BaseMsgInit {
	BaseMsgInit(BaseMsg::CREATOR creator, 
		BaseMsg::PROCESSOR processor,
		const char *pszName, const char *pszID)
		: pCreator(creator), pProcessor(processor), pszClassName(pszName), pszTypeID(pszID) {}
	BaseMsg::CREATOR pCreator;
	BaseMsg::PROCESSOR pProcessor;
	const char * pszTypeID;
	const char * pszClassName;
};


/////////////////////////////////////////////////////////////////////
//  class BaseMsgFactory
/////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS BaseMsgFactory
{
public:
	// malloc memory and construct object
	static BaseMsg* readObject(Archive& ar);
	static BaseMsg* createObject(const MSGHEAD::msgid_t& id);

	static BOOL hasClassID(const WORD& szClassIDRequested);

	typedef MapWordToPtr init_container_t;
	static init_container_t& getCreatorMap();

	//struct ONODE {
	//	BaseMsg *pBaseMsg;
	//	ONODE *next;
	//};
	//static ONODE *object_list_head;
	static void deleteObject(BaseMsg *pBaseMsg);

	static const char * stringFromTypeID(const MSGHEAD::msgid_t& id);
	static const char * classNameFromTypeID(const MSGHEAD::msgid_t& id);
};



class AFX_EXT_CLASS EventSink
{
public:
	EventSink() : fUnifiedMsgProc(NULL), pListener(NULL) {}
	virtual ~EventSink() {}

	// 对象方式处理
public:
	UINIFIEDMSGPROC  RegisterUnifiedMsgProc(UINIFIEDMSGPROC f);
	MSGHEAD::lsn_container_t* RegisterListener(
		MSGHEAD::lsn_container_t& listener);
	HRCODE PostEvent(Archive& ar, const void* inParam=NULL);
	HRCODE PostOneEvent(Archive& ar, const void* inParam=NULL);
private:
	UINIFIEDMSGPROC  fUnifiedMsgProc;
	MSGHEAD::lsn_container_t *pListener;

	// 静态方式处理
public:
	static HRCODE PostOneEvent(Archive& ar, 
					UINIFIEDMSGPROC fUnifiedMsgProc,
					const void* inParam);

	// 客户端提供消息处理函数表，消息建构在heap上
	static HRCODE PostOneEvent(Archive& ar,
					MSGHEAD::lsn_container_t& listener, 
					const void* inParam);

	// 客户端提供消息处理函数表，消息建构在stack上
	static HRCODE PostOneEventEx(Archive& ar,
					MSGHEAD::lsn_container_t& listener, 
					const void* inParam);
	
	static HRCODE PostEvent(Archive& ar,
					MSGHEAD::lsn_container_t& listener,
					const void* inParam=NULL);
	static HRCODE PostEvent(Archive& ar,
		UINIFIEDMSGPROC fUnifiedMsgProc, const void* inParam);
private:
	static HRCODE DefaultMsgProc(const VPPARAM& vpParam);
};
/////////////////////////////////////////////////////////////////////




CLBTH_NAMESPACE_END

#endif //#if !defined(__BASEMSG_H__)
