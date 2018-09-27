/*
*    swecapi.h
*
*    Copyright (c) 2000 Secure Computing Corp.
*
*    defines and data structures for:
*
*    SafeWord EASSP Client (SWEC) Application Interface (API)
*
*/
#ifndef SWECAPI_H_INCLUDED
#define SWECAPI_H_INCLUDED

#include <time.h>
#include "swxml.h"
#include "swlist.h"
#ifndef THREADS
#define THREADS 1
#endif
#include "swecsrv.h"

#if ACCNT_DB
#include "accnt_rec.h"
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

#include "sccerrno.h"
#include "swthread.h"

/* boolean values */
#define SWEC_FALSE   0
#define SWEC_TRUE    1

/* bit values for passUserFlag */
#define SWEC_PASS_USER (1<<0)
#define SWEC_NO_LOG (1<<1)

/* Authen types */
#define SWEC_AUTHEN_OTHER   0
#define SWEC_AUTHEN_FIXED   1

/* dialog types */
#define SWEC_DIALOG_INPUT_ECHO           1
#define SWEC_DIALOG_INPUT_NO_ECHO        2
#define SWEC_DIALOG_INFO                 3

/* input data description codes (used for inputCode) */
#define SWEC_INPUT_NONE                  0
#define SWEC_INPUT_USER_ID               1
#define SWEC_INPUT_PWD                   2
#define SWEC_INPUT_NEW_PWD               3
#define SWEC_INPUT_VERIFY_NEW_PWD        4
#define SWEC_INPUT_AUTHEN_COMBO          5

/* wait callback codes */
#define SWEC_WAIT_CONNECT           1   /* waiting for a server connection */

/* authentication result codes */
#define SWEC_RESULT_INCOMPLETE      0   /* authen process not completed */
#define SWEC_RESULT_PASSED_CHECK    1   /* passed authen,locks,priv,tamper */
#define SWEC_RESULT_BAD_ID          2   /* unknown id */
#define SWEC_RESULT_FAILED_AUTHEN   3   /* failed authentication */
#define SWEC_RESULT_QUOTA_LOCK_ON   4   /* used up usage quota */
#define SWEC_RESULT_TIME_LOCK_ON    5   /* wrong time of day */
#define SWEC_RESULT_DATE_LOCK_ON    6   /* wrong date */
#define SWEC_RESULT_WEEK_LOCK_ON    7   /* wrong day of week */
#define SWEC_RESULT_ATTACK_LOCK_ON  8   /* attack lock triggered */
#define SWEC_RESULT_CLOCK_LOCK_ON   9   /* incorrect system clock setting */
#define SWEC_RESULT_LOW_PRIVILEGE   10  /* insufficent privilege to access */
#define SWEC_RESULT_TAMPER_LOCK_ON  11  /* failed tamper testing */
#define SWEC_RESULT_INPUT_NEEDED    12  /* ok to here, needs more input */
#define SWEC_RESULT_REEVALUATE      13  /* reevaluate */
#define SWEC_RESULT_PASS_DURESS     14  /* passed but used a duress PIN */
#define SWEC_RESULT_PASS_BAD_PIN    15  /* passed but used a bad PIN */
#define SWEC_RESULT_RECORD_LOCKED   16  /* someone else has the record */
#define SWEC_RESULT_INPUT_TIMEOUT   17
#define SWEC_RESULT_LINE_HANGUP     18
#define SWEC_RESULT_BAD_NEW_FIXED   19  /* invalid new fixed password */
#define SWEC_RESULT_PASS_NEED_NEW   20  /* passed but must set new pwd */
#define SWEC_RESULT_SERVER_FAILURE  21  /* server failed during authenticaton */
#define SWEC_RESULT_NO_SERVICE      26  /* no service allowed */
#define SWEC_RESULT_NO_AUTHEN       27  /* user has no authenticators */
#define SWEC_RESULT_WEAK_AUTHEN     28  /* user's authen strength is too weak */
#define SWEC_RESULT_BAD_SERIAL_NUM  31  /* serial# must be unique for proxy */
#define SWEC_RESULT_TTY_PASSED      32
#define SWEC_RESULT_RSH_PASSED      33
#define SWEC_RESULT_NET_PASSED      34

#define SWEC_REPLY_CODE_SUCCESS             0 /* Operation completed with out any problems */
#define	SWEC_REPLY_CODE_NO_SUCCESS          1 /* Operation failed, cause implied or unknown */
#define	SWEC_REPLY_CODE_SESSION_ID_UNKNOWN  2 /* The session ID could not be found */
#define	SWEC_REPLY_CODE_SERVER_ERROR        3 /* Request not completed due to server error */
#define	SWEC_REPLY_CODE_DATA_LOCKED         4 /* Data required for the result is currently locked */
#define	SWEC_REPLY_CODE_NO_PERMISSION       5 /* No permission to obtain the requested data */
#define	SWEC_REPLY_CODE_INVALID_REQUEST     6 /* Bad msg format, missing or invalid data */

/* Global values available to the swecSetGlobalValue and swecGetGlobalValue functions */
/* SWEC_GLOBAL_VALUES is the number of global values available. The first six
 * are reserved by swec as described below */
#define SWEC_GLOBAL_VALUES    12 

/* SWEC_GLOBAL_SHUT_DOWN is reserved for shuting down applications. When this value
*  is set true, the authentication server listener will be halted, and any
*  swThreadWait calls will be terminated. Any application code can use this value
*  to trigger or monitor a shutdown event.
*/
#define SWEC_GLOBAL_SHUTDOWN 0  

/* SWEC_GLOBAL_DEBUG is reserved for run time specifed debuging
*  some internal code will display debug information when this value is set true
*/
#define SWEC_GLOBAL_DEBUG     1

#define SWEC_GLOBAL_RESERVED_1 2
#define SWEC_GLOBAL_RESERVED_2 3
#define SWEC_GLOBAL_RESERVED_3 4
#define SWEC_GLOBAL_RESERVED_4 5

/* data passed to the dialog callback routine */
typedef struct
{
    int             dialogType;
    char            *dialogMessage;
    char            *dialogInputPrompt;
    int             authenNumber;
    char            *authenName;
    char            *challengeText;
    int             inputCode;
    char            *inputBuffer;
    int             inputBufferLength;
    int             abortFlag;
    int             authenType;
} SwecDialogRec;

/* Data and control record for swecOpen() */
typedef struct
{
    int             waitTime;
    int             openAllFlag;
    SwecServerRec   *server;
    int             serverNumber;
    int             numberOfServersOpened;
    char            statusText[SWEC_STATUS_TEXT_LENGTH];
} SwecOpenRec;

/* Data and control record for swecAuthen() */
typedef struct
{
    int             waitTime;
    char            *userId;
    char            *password;
    int             passUserFlag;
    int             resultCode;
    long            actionDataLength;
    char            *actionData;
#if NO_SYSENV
    void            (*dialogCallback)PROTO((SwecHdl dialogContext,
                      SwecDialogRec *dialogRecPtr));
#else
    void            (*dialogCallback)PROTO((Obj sysObj, SwecHdl dialogContext,
                      SwecDialogRec *dialogRecPtr));
#endif
    SwecHdl         dialogContext;
    char            *peerName;
    char            *peerAddr;
    char            *comment;
    char            statusText[SWEC_STATUS_TEXT_LENGTH];
} SwecAuthenRec;

/* Data and control record for swecClose() */
typedef struct
{
    int             closeAllFlag;
    char            statusText[SWEC_STATUS_TEXT_LENGTH];
} SwecCloseRec;

/* Data for swecVersion() */
typedef struct
{
    int     majorVersion;
    int     minorVersion;
    int     revision;
    char    *transport;
    char    *description;
} SwecVersionRec;

/* functions exported by the SWCAPI module */

#if NO_SYSENV
/* SYSTEM ENVIRONMENT WITHHELD
 * PROTOTYPES
 */
SW_EXTERN int  swecRegister    PROTO((SwecHdl       *appHandlePtr, 
                                      SwecRegisterRec *ioReg));
SW_EXTERN int swecRegisterEx1  PROTO((SwecHdl       *appHandlePtr, 
                                      SwecHdl       iConfigHdl,
                                      char          *statusText));
SW_EXTERN int swecRegisterEx2  PROTO((SwecHdl       *appHandlePtr, 
                                      SwecHdl       iConfigHdl,
                                      char          *iAgentType,
                                      char          *statusText));
SW_EXTERN int  swecOpen        PROTO((SwecHdl       appHandle,
                                      SwecHdl       *connectionHandlePtr,
                                      SwecOpenRec   *openRecPtr));
SW_EXTERN int  swecClose       PROTO((SwecHdl       appHandle,
                                      SwecHdl       connectionHandle,
                                      SwecCloseRec  *closeRecPtr));
SW_EXTERN int  swecDeregister  PROTO((SwecHdl       appHandle, 
                                      SwecDeregisterRec *deregRecPtr));
SW_EXTERN int  swecUninit      PROTO((int           forceDeregisterFlag));
SW_EXTERN int  swecAuthen      PROTO((SwecHdl       appHandle, 
                                      SwecHdl       connectionHandle,
                                      SwecAuthenRec *authenRecPtr));
SW_EXTERN int  swecAuthenEx1   PROTO((SwecHdl       appHandle, 
                                      SwecHdl       connectionHandle,
                                      SwecAuthenRec *authenRecPtr,
                                      char          *clientType));
SW_EXTERN int swecCreateConfig PROTO((SwecHdl       *configHdlPtr,
                                      int           source,
                                      char          *identifier,
                                      char          *statusText));
SW_EXTERN int swecDestroyConfig PROTO((SwecHdl      configHdl));
SW_EXTERN int swecSetConfigField PROTO((SwecHdl     configHdl,
                                        int         fieldCode,
                                        void        *dataPtr,
                                        char        *statusText));
SW_EXTERN int swecGetConfigField PROTO((SwecHdl     configHdl,
                                        int         fieldCode,
                                        void        **dataPtrPtr,
                                        char        *statusText));
SW_EXTERN int  swecMsgSend     PROTO((SwecHdl       appHdl,
                                      SwecHdl       connectionHdl,
                                      char          *msg, 
                                      char          **respMsg));

#if ACCNT_DB
SW_EXTERN int swecAccnt        PROTO((SwecHdl       iAppHandle,
                                      SwecHdl       iConnectionHandle,
                                      AccntRec      *accnt_rec)); 
#endif
SW_EXTERN int swecGetSwasHostId PROTO((SwecHdl      iAppHandle,
                                      SwecHdl       iConnectionHandle,
                                      Byte          *hostId));
#ifdef USER_INQURY
SW_EXTERN int  swecCheckUser   PROTO((SwecHdl       iAppHandle,
                                      SwecHdl       iConnectionHandle,
                                      Byte          *UserID));
#endif

#else
/* SYSTEM ENVIRONMENT PASSED
 * PROTOTYPES
 */
SW_EXTERN int  swecRegister    PROTO((Obj           sysObj,
                                      SwecHdl       *appHandlePtr, 
                                      SwecRegisterRec *ioReg));
SW_EXTERN int swecRegisterEx1  PROTO((Obj           sysObj,
                                      SwecHdl       *appHandlePtr, 
                                      SwecHdl       iConfigHdl,
                                      char          *statusText));
SW_EXTERN int swecRegisterEx2  PROTO((Obj           sysObj,
                                      SwecHdl       *appHandlePtr, 
                                      SwecHdl       iConfigHdl,
                                      char          *iAgentType,
                                      char          *statusText));
SW_EXTERN int  swecOpen        PROTO((Obj           sysObj,
                                      SwecHdl       appHandle,
                                      SwecHdl       *connectionHandlePtr,
                                      SwecOpenRec   *openRecPtr));
SW_EXTERN int  swecClose       PROTO((Obj           sysObj,
                                      SwecHdl       appHandle,
                                      SwecHdl       connectionHandle,
                                      SwecCloseRec  *closeRecPtr));
SW_EXTERN int  swecDeregister  PROTO((Obj           sysObj,
                                      SwecHdl       appHandle, 
                                      SwecDeregisterRec *deregRecPtr));
SW_EXTERN int  swecUninit      PROTO((Obj           sysObj,
                                      int           forceDeregisterFlag));
SW_EXTERN int  swecAuthen      PROTO((Obj           sysObj,
                                      SwecHdl       appHandle, 
                                      SwecHdl       connectionHandle,
                                      SwecAuthenRec *authenRecPtr));
SW_EXTERN int  swecAuthenEx1   PROTO((Obj           sysObj,
                                      SwecHdl       appHandle, 
                                      SwecHdl       connectionHandle,
                                      SwecAuthenRec *authenRecPtr,
                                      char          *clientType));
W_EXTERN int swecCreateConfig PROTO((Obj           sysObj,
                                      SwecHdl       *configHdlPtr,
                                      int           source,
                                      char          *identifier,
                                      char          *statusText));
SW_EXTERN void swecDestroyConfig PROTO((Obj         sysObj,
                                        SwecHdl     configHdl));
SW_EXTERN int swecSetConfigField PROTO((Obj         sysObj, 
                                        SwecHdl     configHdl,
                                        int         fieldCode,
                                        void        *dataPtr,
                                        char        *statusText));
SW_EXTERN int swecGetConfigField PROTO((Obj         sysObj, 
                                        SwecHdl     configHdl,
                                        int         fieldCode,
                                        void        **dataPtrPtr,
                                        char        *statusText));
SW_EXTERN int  swecMsgSend     PROTO((Obj           sysObj,
                                      SwecHdl       appHdl,
                                      SwecHdl       connectionHdl,
                                      char          *msg, 
                                      char          **respMsg));

#if ACCNT_DB
SW_EXTERN int  swecAccnt       PROTO((Obj           sysObj,
                                      SwecHdl       iAppHandle,
                                      SwecHdl       iConnectionHandle,
                                      AccntRec      *accnt_rec)); 
#endif
SW_EXTERN int swecGetSwasHostId PROTO((Obj          sysObj,
                                       SwecHdl      iAppHandle, 
                                       SwecHdl      iConnectionHandle,
                                       Byte         *hostId));
#ifdef USER_INQURY
SW_EXTERN int  swecCheckUser   PROTO((Obj           sysObj,
                                      SwecHdl       iAppHandle, 
                                      SwecHdl       iConnectionHandle,
                                      Byte          *UserID));
#endif
#endif

/* SYSTEM ENVIRONMENT INDEPENDANT
 * PROTOTYPES
 */
SW_EXTERN void swecVersion       PROTO((SwecVersionRec *versionRecPtr));
SW_EXTERN int  swecInit          PROTO((void));
SW_EXTERN char *swecSysFilePath  PROTO((void));
SW_EXTERN int  swecLogStatus     PROTO((SwecHdl        appHandle,
                                        int            messageType, 
                                        char           *message));
SW_EXTERN Word swecSetAPIMember  PROTO((SwecHdl        appHandle,
                                        void           *value,
                                        Word           member));
SW_EXTERN void *swecGetAPIMember PROTO((SwecHdl        appHandle,
                                        Word           member));
SW_EXTERN void swecAttachThread  PROTO((SwecHdl        ioAppHandle));
SW_EXTERN void swecDetachThread  PROTO((SwecHdl        ioAppHandle));

SW_EXTERN char *swecConfigFieldName     PROTO((int fieldCode));

SW_EXTERN int swecMsgCreateRequest      PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               char    *idData,
                                               char    *idType));
SW_EXTERN int swecMsgCreateRequest2     PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               char    *idData,
                                               char    *idType,
                                               char    *idEntity));
SW_EXTERN int swecMsgCreateResponse     PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               char    *idData,
                                               char    *idType));
SW_EXTERN int swecMsgCreateResponse2    PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               char    *idData,
                                               char    *idType,
                                               char    *idEntity));
SW_EXTERN int swecMsgCreateSessionDataRequest PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               char    *sessionID,
                                               char    *agentIpAddr,
                                               int     agentPort));
SW_EXTERN int swecMsgCreateSessionStatusRequest PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               char    *sessionID));
SW_EXTERN int swecMsgCreateSessionActivityRequest PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               char    *sessionID,
                                               time_t  touchTimeRequired));
SW_EXTERN int swecMsgCreateSessionActivityResult PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               int     replyStatusCode,
                                               char    *replyStatusMsg,
                                               int     foundFlag));
SW_EXTERN int swecMsgCreateSessionCancelRequest PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               char    *sessionID,
                                               char    *source,
                                               char    *reason));
SW_EXTERN int swecMsgCreateAccessPolicyRequest PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               char    *accessPolicyName,
                                               char    *agentType));
SW_EXTERN int swecMsgCreateUpdateRequest PROTO((SwecHdl appHdl,
                                               char    **msgPtr));
SW_EXTERN int swecMsgCreateUpdateRequest2 PROTO((SwecHdl appHdl,
                                               char    **msgPtr,
                                               char    *accessPolicyName,
                                               char    *agentType,
                                               char    *agentIpAddr,
                                               int     agentPort));
SW_EXTERN int swecMsgSetSessionUpdate   PROTO((SwecHdl appHdl,
                                               char    **msg,
                                               char    *agentIpAddr,
                                               int     agentPort));
SW_EXTERN int swecMsgSetAccessPolicyUpdate PROTO((SwecHdl appHdl,
                                               char    **msg,
                                               char    *accessPolicyName,
                                               char    *agentType));
SW_EXTERN int swecMsgSetRequirements    PROTO((char    **msg,
                                               char    *session,
                                               char    *strength,
                                               char    *service));
SW_EXTERN int swecMsgSetRequirements2   PROTO((char    **msg,
                                               char    *session,
                                               char    *strength,
                                               char    *service,
                                               char    *roles));
SW_EXTERN int swecMsgAddPersonalizationData PROTO((char **msgPtr,
                                               char    *name,
                                               char    *value));
SW_EXTERN int swecMsgAddReqAuthenticator PROTO((char **msgPtr,
                                               char    *deviceID,
                                               char    *deviceData));
SW_EXTERN int swecMsgSetClientLoc       PROTO((char    **msgPtr,
                                               char    *ipAddr,
                                               char    *hostName));
SW_EXTERN int swecMsgSetClientType      PROTO((char    **msgPtr,
                                               char    *name,
                                               char    *version));
SW_EXTERN int swecMsgSetAgent           PROTO((char    **msgPtr,
                                               char    *name,
                                               char    *type));
SW_EXTERN int swecMsgSetClientComment   PROTO((char    **msgPtr,
                                               char    *data));
SW_EXTERN int swecMsgSetAgentComment    PROTO((char    **msgPtr,
                                               char    *data));
SW_EXTERN int swecMsgSetUserLoc         PROTO((char    **msgPtr,
                                               char    *ipAddr,
                                               char    *hostName));
SW_EXTERN int swecMsgAddAppData         PROTO((char    **msgPtr,
                                               char    *type,
                                               char    *data));
SW_EXTERN int swecMsgSetAuthen          PROTO((char    **msgPtr,
                                               int     authenNum,
                                               char    *type));
SW_EXTERN int swecMsgSetAuthenAttr      PROTO((char    **msgPtr,
                                               int     authenNum,
                                               char    *attribute,
                                               char    *value));

SW_EXTERN char *swecMsgGetType          PROTO((char    *msg));
SW_EXTERN char *swecMsgGetIdData        PROTO((char    *msg));
SW_EXTERN char *swecMsgGetIdType        PROTO((char    *msg));
SW_EXTERN char *swecMsgGetIdEntity      PROTO((char    *msg));
SW_EXTERN char *swecMsgGetAuthenType    PROTO((char    *msg,
                                               int     authenNum));
SW_EXTERN char *swecMsgGetAuthenAttr    PROTO((char    *msg,
                                               int     authenNum,
                                               char    *attribute));
SW_EXTERN char *swecMsgGetAppData       PROTO((char    *msg,
                                               char    *type));
SW_EXTERN char *swecMsgGetStatusMsg     PROTO((char    *msg));
SW_EXTERN char *swecMsgGetResultCode    PROTO((char    *msg));
SW_EXTERN char *swecMsgGetResult        PROTO((char    *msg));
SW_EXTERN char *swecMsgGetAuthorization PROTO((char    *msg,
                                               char    *name));
SW_EXTERN int  swecMsgGetReplyStatusCode PROTO((char    *msg));
SW_EXTERN char *swecMsgGetReplyStatusMsg PROTO((char    *msg));
SW_EXTERN void *swecAllocMemory         PROTO((unsigned int size));
SW_EXTERN void swecFreeMemory           PROTO((void    *memory));
SW_EXTERN Word swecAction               PROTO((char    *actionStr, 
                                               char    *userId,
                                               char    *groupName,
                                               char    *term,
                                               int     rc,
                                               int     argc,
                                               char    **argv,
                                               char    **envp));
SW_EXTERN int  swecAuthenTTY            PROTO((int     argc, 
                                               char    *argv[], 
                                               char    **envp,
                                               char    *userId));
SW_EXTERN int  swecConnPoolCreate       PROTO((SwecHdl appHandle,
                                               SwecHdl *connPoolHandlePtr,
                                               int     poolSize,
                                               int     socketTimeout,
                                               int     persistenceTime,
                                               long    retryDelay));
SW_EXTERN void swecConnPoolDestroy      PROTO((SwecHdl connPoolHandle));
SW_EXTERN int  swecConnPoolGetConn      PROTO((SwecHdl connPoolHandle,
                                               SwecHdl *connectionHandlePtr,
                                               char    *statustText));
SW_EXTERN int  swecConnPoolReturnConn   PROTO((SwecHdl connPoolHandle,
                                               SwecHdl connectionHandle));
SW_EXTERN int  swecGetGlobalValue        PROTO((int     indexIntoValues));

SW_EXTERN void swecSetGlobalValue       PROTO((int     indexIntoValues,
                                               int     value));
SW_EXTERN Word swecConvertErrorCode     PROTO((Word    errorCode));

SW_EXTERN int  swecGetRndChallStr       PROTO((char    *outBuf,
                                               int     outBufSize));

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _MSC_VER */

#ifdef WIN32
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

#endif /* #ifndef SWECAPI_H_INCLUDED */
