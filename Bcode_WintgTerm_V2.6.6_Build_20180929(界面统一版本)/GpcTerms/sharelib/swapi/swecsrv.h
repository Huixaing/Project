/*
*   swecsrv.h
*
*   server transport defines for BSD sockets in Unix and Win32 environments
*
*/
#ifndef swecsrv_INCLUDE
#define swecsrv_INCLUDE

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

#define SWEC_SRV_CLIENT_NAME_LEN 80
#define SWEC_SRV_IP_ADDR_LEN 16

typedef struct
{
    char    *msg; /* pointer to whole message */
    char    *content; /* pointer to content */
} SwecSrvEassp2MsgRec;

typedef struct
{
    int     socketNum;
    int     *abortFlagPtr;
    SwecHdl appHdl;
} SwecSrvClientsRec;


typedef struct
{
    int            socketNum;
    char           name[SWEC_SRV_CLIENT_NAME_LEN];
    char           ipaddr[SWEC_SRV_IP_ADDR_LEN];
    unsigned short port;
    int            *abortFlagPtr;
    Ptr            textSet;
    Word           textMode;
} SwecSrvClientRec;

/*  swecSrvOpenClients()
*
*   Open a connection for receiving requests from clients and stores a
*   ptr to a SwecSrvClientsRec at clients. Call swecSrvCloseClients to dispose
*   of this object.
*
*   Creates a socket and assigns an address based on the service data
*   specified.  The servicePort parameter is used if passed in (not 0), else
*   serviceName is used. If the name fails, or is NULL the servicePortDefault
*   number is used. If a statusText pointer is not NULL, text will be stored
*   indicating connection and error status.
*
*   Inputs:
*       serviceName - name used if servicePort == 0 (null terminated
*                      string) or set to NULL if servicePort is specified
*       servicePort - port number used (set to 0 to use serviceName)
*       servicePortDefault - port number used if name and port are both null
*                            or service can not be found.
*       abortFlagPtr - pointer to boolean, can be NULL. If not NULL, this boolean
*                      is checked periodically in this function as well as
*                      swecSrvAcceptClient and swecSrvRecClientEassp2. If true, the task
*                      will be aborted. (Note that the global value
*                      SWGFLAG_SHUTDOWN is also checked. In the libswecapi context
*                      this variable is accessed with swecSetGlobalValue() and
*                      swecGetGlobalValue().
*   Output:
*       statusText - if not NULL, status text is stored here
*                    (at least SWEC_STATUS_TEXT_LENGTH)
*       clients - stores ptr to SwecSrvClientsRec struct if SWEC_STATUS_SUCCESS returned
*   Return:
*       SWEC_STATUS_SUCCESS, SWEC_STATUS_ALLOC_ERR or SWEC_STATUS_NO_SUCCESS
*/
SW_EXTERN int swecSrvOpenClients   PROTO((SwecSrvClientsRec       **clients,
                                          char                    *serviceName,
                                          unsigned short          servicePort,
                                          unsigned short          servicePortDefault,
                                          int                     *abortFlagPtr,
                                          char                    *statusText));

/*  swecSrvCloseClients()
*
*   Close the connection for receiving requests from clients, releasing all
*   resources. Call this after calling swecSrvOpenClients.
*
*   Input:
*       clients - ptr to SwecSrvClientsRec returned from the openClients function
*/
SW_EXTERN void swecSrvCloseClients PROTO((SwecSrvClientsRec       *clients));

/*  swecSrvCloseClient()
*
*   Close a connection for a client, releasing all resources. Call this after
*   calling swecSrvAcceptClient.
*
*   Input:
*       client - ptr to SwecSrvClientRec returned from the swecSrvAcceptClient function
*/
SW_EXTERN void swecSrvCloseClient  PROTO((SwecSrvClientRec        *client));

/* -------------------------------- swecSrvAcceptClient() --------------------------
*
*   Wait for a client to initiate a request. Create a new client object
*   and set a pointer to it @ theClient. The new client object must be
*   disposed by a call to swecSrvCloseClient by the code that calls this function.
*
*   If an unload signal comes in or an error occurs, the client object
*   is not created and theClient is set to NULL.
*
*   Inputs:
*       clients - is the SwecSrvClientsRec ptr returned by swecSrvOpenClients
*   Outputs:
*       theClient - ptr to ptr to SwecSrvClientRec
*       statusText - ptr to string storage for any error or status messages
*                    (at least SWEC_STATUS_TEXT_LENGTH)
*   Returns:
*       SWEC_STATUS_SUCCESS
*       SWEC_STATUS_ABORTED on shutdown
*       SWEC_STATUS_ALLOC_ERR or SWEC_STATUS_NO_SUCCESS for error conditions
*/
SW_EXTERN int swecSrvAcceptClient  PROTO((SwecSrvClientsRec       *clients,
                                          SwecSrvClientRec        **theClient,
                                          char                    *statusText));

/*  swecSrvRecClientEassp2()
*
*   Receive a msg from the client. A pointer to the msg is returned @
*   msgPtr. Code that called this function is responsible for deallocating
*   the msg. Error and status info is returned to the storage indicated by
*   statusText.
*   The message format is essentially HTTP.
*   A Content-length header must be present.
*   Each headers is delimited by \n, content by \n\n
*
*   Inputs:
*       client - ptr to SwecSrvClientRec returned from swecSrvAcceptClient()
*       maxContent - maximum content size to accept
*   Outputs:
*       msgPtr - ptr to location to store ptr to msg received
*       statusText - location to store err msg text (at least SWEC_STATUS_TEXT_LENGTH)
*   Returns:
*       SWEC_STATUS_SUCCESS for msg received
*       SWEC_STATUS_ABORTED   for no msg received or interrupted
*       SWEC_STATUS_INVALID_CLIENT_REQUEST or SWEC_STATUS_ALLOC_ERR for error condition or invalid msg
*/
SW_EXTERN int swecSrvRecClientEassp2  PROTO((SwecSrvClientRec        *client,
                                             SwecSrvEassp2MsgRec     **msgPtr,
                                             long                    maxContent,
                                             char                    *statusText));

/* --------------------------- swecSrvSendClientEassp2() --------------------------------
*
*   Send a message to a client. A content length header is created and prepended to the
*   message.
*
*   Inputs:
*       client - ptr to SwecSrvClientRec returned from swecSrvAcceptClient()
*       msgBuf - ptr to msg to send
*   Outputs:
*       statusText - location to store err msg text (at least SWEC_STATUS_TEXT_LENGTH)
*   Returns:
*       SWEC_STATUS_SUCCESS for msg sent
*       SWEC_STATUS_CLIENT_SEND_ERR or SWEC_STATUS_ALLOC_ERR for error condition
*/
SW_EXTERN int swecSrvSendClientEassp2 PROTO((SwecSrvClientRec        *client,
                                             char                    *msg,
                                             long                    msgLen,
                                             char                    *statusText));

/* --------------------------- swecSrvSendRawClient() --------------------------------
*
*   Send a message to a client.
*   This is a raw write, all headers must be included in the message
*   Minimum headers for EASSP 2xx: Content-Length
*   Inputs:
*       client - ptr to SwecSrvClientRec returned from swecSrvAcceptClient()
*       msgBuf - ptr to msg to send
*   Outputs:
*       statusText - location to store err msg text (at least SWEC_STATUS_TEXT_LENGTH)
*   Returns:
*       SWEC_STATUS_SUCCESS for msg sent
*       SWEC_STATUS_CLIENT_SEND_ERR or SWEC_STATUS_ALLOC_ERR for error condition
*/
SW_EXTERN int swecSrvSendRawClient PROTO((SwecSrvClientRec        *client,
                                          char                    *msg,
                                          long                    msgLen,
                                          char                    *statusText));

/* ----------------------------- swecSrvFreeEassp2Msg() ---------------------------------
*
*   Free memory allocated for the message.
*/
SW_EXTERN void swecSrvFreeEassp2Msg   PROTO((SwecSrvEassp2MsgRec  *msg));

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _MSC_VER */

#ifdef WIN32
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

#endif /* swecsrv_INCLUDE */
