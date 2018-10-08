/*
*   swxml.h
*/
#ifndef swxml_INCLUDED
#define swxml_INCLUDED

#include <stdio.h>

#ifdef SW_EXTERN
#undef SW_EXTERN
#endif

#if WIN32
#define SW_EXTERN __declspec(dllexport)
#else
#define SW_EXTERN extern
#endif

#ifdef  WIN32
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif

#ifdef _MSC_VER
#pragma pack(push)
#pragma pack(8)
#endif /* _MSC_VER */

#define SW_XML_DOC_HEAD "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"

#define SW_XML_CDATA_START "<![CDATA["
#define SW_XML_CDATA_START_LEN 9
#define SW_XML_CDATA_END "]]>"
#define SW_XML_CDATA_END_LEN 3

/* defines for the cData parameter for node creation functions */
#define SW_XML_PCDATA 0  /* parsed character data */
#define SW_XML_CDATA  1  /* unparsed character data */

/* SwXmlNode types */
#define SW_XML_ELEMENT_NODE    1
#define SW_XML_COMMENT_NODE    2

typedef struct _SwXmlAttr
{
    char                *name;
    char                *value;
    struct _SwXmlAttr   *next;
} SwXmlAttr;

typedef struct _SwXmlNode
{
    int                 type;       /* code that determines meaning of content */
    void                *context;   /* reference to some context for this node */
    struct _SwXmlNode   *parent;    /* child->parent link */
    struct _SwXmlNode   *next;      /* next sibling link  */
    struct _SwXmlNode   *previous;  /* previous sibling link  */
    struct _SwXmlNode   *children;  /* parent->children link */
    struct _SwXmlNode   *lastChild; /* last child link */
    char                *name;      /* the name of the node */
    char                *content;   /* content data */
    int                 cData;      /* if non-zero, content is CDATA, else PCDATA */
    SwXmlAttr           *attrs;     /* list of attributes for this element */
} SwXmlNode;

typedef struct
{
    char *content;          /* The buffer content UTF8 */
    unsigned int use;       /* The buffer size used */
    unsigned int size;      /* The buffer size */
} SwXmlBuf;

/* -------------------- summary of XML document functions ---------------------- */

/* creating nodes -------------------

    SwXmlNode *swxmlCreateNode(const char *name, const char *content, int cData,
    SwXmlNode *swxmlAddNewChild(SwXmlNode *parent, const char *name, const char *content,
                     int cData, const char **attrArray);
    SwXmlNode *swxmlInsertNewSiblingBefore(SwXmlNode *sibling, const char *name,
                     const char *content, int cData, const char **attrArray);

/* adding nodes ---------------------

    void    swxmlInsertSiblingBefore(SwXmlNode *sibling, SwXmlNode *newNode);
    void    swxmlAddChild(SwXmlNode *parent, SwXmlNode *child);

/* retrieving nodes -----------------

    SwXmlNode *swxmlGetLastChild(SwXmlNode *parent);
    SwXmlNode *swxmlFindNode(SwXmlNode *node, char *name);
    SwXmlNode *swxmlFindNextNode(SwXmlNode *node);

/* eliminating nodes ----------------

    void    swxmlDestroyNode(SwXmlNode *node);
    void    swxmlRemoveNode(SwXmlNode *node);
    void    swxmlDestroyNodeList(SwXmlNode *node);

/* contents of nodes ----------------

    char    *swxmlSetNodeContent(SwXmlNode *node, const char *content, int cData);
    char    *swxmlTrimContent(SwXmlNode *node);
    char    *swxmlAppendNodeContent(SwXmlNode *node, const char *content, int len);
    char    *swxmlGetNodeContent(SwXmlNode *node);

/* attributes of nodes --------------

    SwXmlAttr *swxmlAddNewAttr(SwXmlNode *node, const char *name, const char *value);
    SwXmlAttr *swxmlCreateAttrList(const char **attrs);
    SwXmlAttr *swxmlAddAttr(SwXmlNode *node, const char *name, const char *value);
    void    swxmlAddAttr(SwXmlNode *node, SwXmlAttr *attr);
    char    *swxmlGetAttrValue(SwXmlNode *node, const char *name);
    SwXmlAttr *swxmlCreateAttr(const char *name, const char *value);
    void    swxmlDestroyAttr(SwXmlAttr *attr);
    void    swxmlDestroyAttrList(SwXmlAttr *attr);

/* XML text document

    char    *swxmlWriteToBuf(SwXmlNode *node);
    int     swxmlCreateXmlFile(const char *filename, SwXmlNode *node);
    int     swxmlWriteToSwXmlBuf(SwXmlBuf *buf, SwXmlNode *node);
    int     swxmlWriteToFile(FILE *f, SwXmlNode *node);
    SwXmlBuf  *swxmlCreateSwXmlBuf(void);
    void    swxmlDestroySwXmlBuf(SwXmlBuf *buf);
    int     swxmlWriteSwXmlBufToFile(FILE *file, SwXmlBuf *buf);
    char    *swxmlGetSwXmlBufContents(SwXmlBuf *buf);
    char    *swxmlSwXmlBufToBuf(SwXmlBuf *buf);
    char    *swxmlTrim(char *str);


------------------------------------------------------------------------------------ */

/* ------------------------- xml document parsing function ------------------------- */

/*  swxmlDocToSwXmlNode
*   Parse an xml document, producing an xml tree.
*   returns the root node of the tree or NULL if error occured.
*/
SW_EXTERN SwXmlNode *swxmlDocToSwXmlNode(char *xmlDoc, long xmlDocLength);

/*  swxmlDocToSwXmlNode
*   Parse an xml document, producing an xml tree.
*   returns the root node of the tree or NULL if error occured.
*   also returns error message.
*/
SW_EXTERN SwXmlNode *swxmlDocToSwXmlNode2(char *xmlDoc, long xmlDocLength, char *errorBuf, int errorBufLength);


/* ------------------------------- xml tree functions ------------------------------ */

/*  swxmlCreateNode
*   creates new node that defaults to element type
*   name, content and attrArray can be NULL
*   cData should be either SW_XML_PCDATA or SW_XML_CDATA
*   attrArray is an array of x number of strings with attibute name followed by
*       attribute value, terminated with a NULL.
*   returns the new node or NULL if there is an allocation error
*/
SW_EXTERN SwXmlNode *swxmlCreateNode(const char *name, const char *content, int cData,
                      const char **attrArray);
/*  swxmlAddNewChild
*   creates new node that defaults to element type
*   name, content and attrArray can be NULL
*   cData should be either SW_XML_PCDATA or SW_XML_CDATA
*   attrArray is an array of x number of strings with attibute name followed by
*       attribute value, terminated with a NULL.
*   returns the new node or NULL if there is an allocation error
*/
SW_EXTERN SwXmlNode *swxmlAddNewChild(SwXmlNode *parent, const char *name, const char *content, int cData,
                     const char **attrArray);

/*  swxmlInsertNewSiblingBefore
*   creates a new node that defaults to element type
*   the node is inserted before a existing node as a sibling
*   name, content and attrArray can be NULL
*   cData should be either SW_XML_PCDATA or SW_XML_CDATA
*   attrArray is an array of x number of strings with attibute name followed by
*       attribute value, terminated with a NULL.
*   returns the new node or NULL if there is an allocation error
*/
SW_EXTERN SwXmlNode *swxmlInsertNewSiblingBefore(SwXmlNode *sibling, const char *name, const char *content, int cData,
                     const char **attrArray);

/*  swxmlInsertSiblingBefore
*   insert a node before a existing node as a sibling
*/
SW_EXTERN void    swxmlInsertSiblingBefore(SwXmlNode *sibling, SwXmlNode *newNode);

/*  swxmlAddChild
*   add a child to the end of the list of children
*/
SW_EXTERN void    swxmlAddChild(SwXmlNode *parent, SwXmlNode *child);

/*  swxmlGetLastChild
*   return the last child in the list of children
*/
SW_EXTERN SwXmlNode *swxmlGetLastChild(SwXmlNode *parent);

/*  swxmlDestroyNode
*   destroy a node and all its children, freeing their memory
*/
SW_EXTERN void    swxmlDestroyNode(SwXmlNode *node);

/*  swxmlFindNode
*   search for a node with the named element tag
*   the node and its children are searched
*   returns found node or NULL
*/
SW_EXTERN SwXmlNode *swxmlFindNode(SwXmlNode *node, char *name);

/*  swxmlFindNextNode
*   search for a node with the named element tag
*   the node's children are searched first
*   then the following siblings and their children are searhed
*   returns found node or NULL
*/
SW_EXTERN SwXmlNode *swxmlFindNextNode(SwXmlNode *node);

/*  swxmlRemoveNode
*   remove a node from the tree, memory for the node is NOT freed.
*/
SW_EXTERN void    swxmlRemoveNode(SwXmlNode *node);

/*  swxmlDestroyNodeList
*   destroy a node and all of the siblings that follow it, freeing their memory
*/
SW_EXTERN void    swxmlDestroyNodeList(SwXmlNode *node);

/*  swxmlSetNodeContent
*   cData should be either SW_XML_PCDATA or SW_XML_CDATA
*   returns pointer to content or NULL if allocation error
*/
SW_EXTERN char    *swxmlSetNodeContent(SwXmlNode *node, const char *content, int cData);

/*  swxmlTrimContent
*   trim leading and trailing whitespace from content
*   returns pointer to content or NULL if node or content is NULL
*/
SW_EXTERN char    *swxmlTrimContent(SwXmlNode *node);

/*  swxmlAppendNodeContent
*   append data to content
*   len specifies how many bytes to append
*   note that content must not have escaped text, 
*   this function automatically escapes the text
*   returns pointer to escaped text or
*   returns NULL if out of memory
*/
SW_EXTERN char    *swxmlAppendNodeContent(SwXmlNode *node, const char *content, int len);

/*  swxmlGetNodeContent
*   returns pointer to content or NULL if node or content is NULL
*/
SW_EXTERN char    *swxmlGetNodeContent(SwXmlNode *node);

/*  swxmlAddNewAttr
*   add a new attribute to a node
*   return a pointer to the attribute or NULL if node is NULL or allocation error
*/
SW_EXTERN SwXmlAttr *swxmlAddNewAttr(SwXmlNode *node, const char *name, const char *value);

/*  swxmlCreateAttrList
*   convert an array of x number of strings to an attribute list
*   the array contains name then value then name then value etc.
*   the array must be terminated with a NULL
*   return a pointer to the first attribute in the list
*   or NULL if allocation error
*/
SW_EXTERN SwXmlAttr *swxmlCreateAttrList(const char **attrs);

/*  swxmlSetAttr
*   set the value of an attribute
*   if the attribute does not exist, it will be created
*   return attribute or NULL if node or name is NULL or allocation error
*/
SW_EXTERN SwXmlAttr *swxmlSetAttr(SwXmlNode *node, const char *name, const char *value);

/*  swxmlAddAttr
*   add an attribute to a node, does not check if attribute exists
*/
SW_EXTERN void swxmlAddAttr(SwXmlNode *node, SwXmlAttr *attr);

/*  swxmlGetAttrValue
*   return value of named attribute or NULL if node is NULL or no attribute
*/
SW_EXTERN char    *swxmlGetAttrValue(SwXmlNode *node, const char *name);

/*  swxmlCreateAttr
*   create an attribute with the specified name and value
*   return NULL if allocation error
*/
SW_EXTERN SwXmlAttr *swxmlCreateAttr(const char *name, const char *value);

/*  swxmlDestroyAttr
*   destroy a single attribute, freeing its memory
*   if attribute is part of a list, use swxmlDestroyAttrList to free all in list
*/
SW_EXTERN void    swxmlDestroyAttr(SwXmlAttr *attr);

/*  swxmlDestroyAttrList
*   destroy a list of attributes, freeing their memory
*/
SW_EXTERN void    swxmlDestroyAttrList(SwXmlAttr *attr);



/* ------------------------xml document creation functions ------------------------- */

/*  swxmlWriteToBuf
*   write node tree to XML document and return it in allocated memory
*   return NULL if allocation error
*/
SW_EXTERN char    *swxmlWriteToBuf(SwXmlNode *node);

/*  swxmlCreateXmlFile
*   creates a file with the specifed name and writes the XML document to it
*   returns file length or 0 if allocation or file error
*/
SW_EXTERN int     swxmlCreateXmlFile(const char *filename, SwXmlNode *node);

/*  swxmlWriteToSwXmlBuf
*   writes the XML document into an SwXmlBuf
*   returns length of document
*   or 0 if allocation error or null input
*/
SW_EXTERN int     swxmlWriteToSwXmlBuf(SwXmlBuf *buf, SwXmlNode *node);

/*  swxmlWriteToFile
*   writes an XML document to the specified open file
*   returns file length or 0 if allocation or file error
*/
SW_EXTERN int     swxmlWriteToFile(FILE *f, SwXmlNode *node);

/*  swxmlCreateSwXmlBuf
*   allocate an SwXmlBuf, intial size is XML_BUF_BASE_SIZE
*   return NULL if allocation error
*/
SW_EXTERN SwXmlBuf  *swxmlCreateSwXmlBuf(void);

/*  swxmlDestroySwXmlBuf
*   dellocate the SwXmlBuf
*/
SW_EXTERN void    swxmlDestroySwXmlBuf(SwXmlBuf *buf);

/*  swxmlWriteSwXmlBufToFile
*   write the contents of the SwXmlBuf to the open file
*/
SW_EXTERN int     swxmlWriteSwXmlBufToFile(FILE *file, SwXmlBuf *buf);

/*  swxmlGetSwXmlBufContents
*   return the contents of the SwXmlBuf
*/
SW_EXTERN char    *swxmlGetSwXmlBufContents(SwXmlBuf *buf);

/*  swxmlSwXmlBufToBuf
*   convert an SwXmlBuf to plain memory buffer
*   the SwXmlBuf struct is deallocated
*   returns pointer to buffer contents
*/
SW_EXTERN char    *swxmlSwXmlBufToBuf(SwXmlBuf *buf);

/*  swxmlTrim
*   trims leading and trailing whitespace, 
*   reallocs string if necessary
*/
SW_EXTERN char    *swxmlTrim(char *str);

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _MSC_VER */

#ifdef WIN32
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

#endif /* included */
