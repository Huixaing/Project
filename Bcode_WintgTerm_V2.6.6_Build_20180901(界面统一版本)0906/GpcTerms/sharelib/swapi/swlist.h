/*
*       swlist.h
*
*   This module contains routines for double linked lists:
*       swlistInsertAtTail()
*       swlistInsertAtHead()
*       swlistInsertAfterNode()
*       swlistGetHead()
*       swlistNodeRemove()
*       swlistNodeExists()
*       swlistIterate()
*       swlistRelease()
*
*   SwListRec is a struct with pointers to the head and tail of the list.
*   It is in essence the list.
*
*   Nodes in the list are data structures which must contain a SwListNodeRec
*   as the first part of the data structure. This is comprised of pointers
*   to the next and previous nodes. The data comes next.
*
*   Example code:
*
*   #include swlist.h
*   SwListRec     myList;
*   SwListNodeRec *aNodePtr;
*   SwListNodeRec *nPtr;
*   Byte          *someData = "This is some data.";
*
*   aNodePtr = allocate(sizeof(SwListNodeRec) + strlen(someData));
*   strcpy(aNodePtr->data, someData);
*   swlistInsertAtTail(aNodePtr, &myList);
*   if(swlistNodeExists(aNodePtr, &myList))
*                       ...
*   swlistInsertAtHead(aNodePtr, &myList);
*   swlistNodeRemove(aNodePtr, &myList);     // removing one node
*   deallocate(iNodePtr);                  // and releasing it
*
*   nPtr = NULL;                           // iterating thru list
*   while (swlistIterate(&nPtr, &myList))
*       printf(nPtr->data);
*
*   swlistRelease(&myList, nodeRelease, deallocate);     // releasing all nodes
*
*   void nodeRelease(void *nodePtr)
*   {
*           printf("Releasing node with data: %s", nodePtr->data);
*   }
*
*/

#ifndef SWLIST_H
#define SWLIST_H

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

typedef struct
{
    void           *head;
    void           *tail;
} SwListRec;

typedef struct SW_LIST_NODE_S
{
    struct SW_LIST_NODE_S  *next;
    struct SW_LIST_NODE_S  *prev;
    Byte                data[1];
} SwListNodeRec;

/* ----------------------------- swlistInsertAtTail() -------------------------
*
*   Insert a node at the tail of the list.
*
*   Input:
*       iNodePtr - pointer to node (must start with SwListNodeRec)
*       iListPtr - pointer to list (must start with SwListRec)
*/
SW_EXTERN void swlistInsertAtTail    PROTO((void *iNodePtr, void *iListPtr ));

/* ----------------------------- swlistInsertAtHead() -------------------------
*
*   Insert a node at the head of the list.
*
*   Input:
*       iNodePtr - pointer to node (must start with SwListNodeRec)
*       iListPtr - pointer to list (must start with SwListRec)
*/
SW_EXTERN void swlistInsertAtHead    PROTO((void *iNodePtr, void *iListPtr ));

/* ----------------------------- swlistInsertAfterNode() -------------------------
*
*   Insert new node into chain after iPreviousNodePtr
*
*   Input:
*       iNodePtr - pointer to node to insert (must start with SwListNodeRec)
*       iPreviousNodePtr - pointer to prev node (must start with SwListNodeRec)
*       iListPtr - pointer to list (must start with SwListRec)
*/
SW_EXTERN void swlistInsertAfterNode PROTO((void *iNodePtr, void *iPreviousNodePtr,
                                    void *iListPtr ));

/* ----------------------------- swlistNodeRemove() -------------------------
*
*   Remove the node specified from the list.
*
*   Input:
*       iNodePtr - pointer to node (must start with SwListNodeRec)
*       iListPtr - pointer to list (must start with SwListRec)
*/
SW_EXTERN void swlistNodeRemove      PROTO((void *iNodePtr, void *iListPtr ));

/* ----------------------------- swlistGetHead() -------------------------
*
*   Returns the head node in the list.
*
*   Input:
*       iListPtr - pointer to list (must start with SwListRec)
*   Return:
*       pointer to node (must start with SwListNodeRec) or NULL
*/
SW_EXTERN SwListNodeRec *swlistGetHead PROTO((void *iListPtr));

/* ----------------------------- swlistNodeExists() -------------------------
*
*   See if the node specified is in the list.
*
*   Input:
*       iNodePtr - pointer to node (must start with SwListNodeRec)
*       iListPtr - pointer to list (must start with SwListRec)
*   Return:
*       TRUE or FALSE
*/
SW_EXTERN Word swlistNodeExists      PROTO((void *iNodePtr, void *iListPtr ));

/* ----------------------------- swlistIterate() -------------------------
*
*   Call this routine to iterate thru a list. Pass in a ptr to a nodePtr.
*   To start at the head of the list, set the nodePtr to 0. The nodePtr
*   will be set ot the next node in the list upon each call. It will return
*   TRUE while there are nodes in the list.
*
*   Input:
*       ioNodePtrPtr - pointer to nodePtr (nodePtr must start with SwListNodeRec)
*       iListPtr - pointer to list (must start with SwListRec)
*   Return:
*       TRUE (got next node)
*       FALSE (no more nodes, nodePtr not changed)
*/
SW_EXTERN Word swlistIterate         PROTO((void **ioNodePtrPtr, void *iListPtr));

/* ----------------------------- swlistRelease() -------------------------
*
*   This function releases the entire list and optionally deallocates it. The
*   memoryFreeRoutine parameter is a pointer to a memory deallocation function.
*   That function will be called to free the list nodes as they are released.
*   If the pointer is NULL, nodes will not have their memory freed.
*
*   The entryFreeRoutine parameter may contain a pointer to a function that will be
*   called with the node ptr as it's argument, as the node is released from the list.
*   This call occurs just before the node is deallocated (if a memoryFree func was
*   passed in). If no callback is required, set to NULL.
*
*   Input:
*       iListPtr - pointer to SwListRec
*       entryFreeRoutine - function pointer for routine to call before
*                          deallocating a node.
*       memoryFreeRoutine - function pointer for routine to deallocate a node;
*                           pass NULL if you do not want the list automatically
*                           freed.
*   Return:
*       number of nodes released.
*/
#if SYSENV_TYPE==NO_SYSENV_TYPE
SW_EXTERN uLongWord swlistRelease    PROTO((void *iListPtr,
                                    void (*entryFreeRoutine)(void *iNodePtr),
                                    void (*memoryFreeRoutine)(void *data) ));
#else
SW_EXTERN uLongWord swlistRelease    PROTO((Obj sysObj, void *iListPtr,
                                    void (*entryFreeRoutine)(void *iNodePtr),
                                    void (*memoryFreeRoutine)(void *data) ));
#endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _MSC_VER */

#ifdef WIN32
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

#endif

