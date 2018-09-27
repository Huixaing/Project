/*
*    swconfig.h
*
*    Copyright (c) 2000 Secure Computing Corp.
*
*    defines and data structures for:
*
*    SafeWord EASSP Client (SWEC) Application Interface (API)
*
*/

#ifndef SWCONFIG_H
#define SWCONFIG_H 1

#ifdef  WIN32
#ifdef __cplusplus
#define PROTOTYPING 1
extern "C" {
#endif /* __cplusplus */
#endif /* WIN32 */

#ifdef _MSC_VER
#pragma pack(push)
#pragma pack(8)
#endif /* _MSC_VER */

#ifdef SW_EXTERN
#undef SW_EXTERN
#endif

/* define one of the following as true */
#if WIN32
#define SW_EXTERN __declspec(dllexport)
#else
#define SW_EXTERN extern
#endif

/* SafeWord Data types 
 * SafeWord production applications use this header
 * as do SafeWord API based applications.  This
 * header (and some API headers) require some
 * SafeWord defined datatypes be set.
 * The production applications already have their
 * datatypes defined and do not need to define the
 * below datatypes.  API based applications do
 * need to define the below datatypes.  All 
 * production applications should have SW_VERSION
 * defined before this header is included.  If 
 * SW_VERSION is not defined, it is assumed that
 * the application being compiled is an API based
 * application being compiled by the customer.
 */
#ifndef SW_VERSION
typedef char Byte;
typedef unsigned char uByte;
typedef short Word;
typedef unsigned short uWord;
typedef long LongWord;
typedef unsigned long uLongWord;
typedef void *Ptr;
#ifdef WIN32
typedef void *HANDLE;
#else /* WIN32 */
typedef int HANDLE;
#endif /* WIN32 */
#endif /* SW_VERSION */

#ifndef NO_SYSENV
# define NO_SYSENV 1
#endif

#ifndef PROTOTYPING
# define PROTOTYPING 0
#endif

#if PROTOTYPING
#   define PROTO(x)     x
#else
#   define PROTO(x)     ()
#endif

#include "swlist.h"

/* SafeWord registry keys */
#define SW_PRODUCT_NAME      "SafeWord"
#define SW_COMPANY_NAME      "Secure Computing"

/* --------------  XML message defines ----------------  */

/* Message Header Names */
#define SWXML_HTTP_HDR_TASK_ID        "Task-id:"
#define SWXML_HTTP_HDR_CONTENT_TYPE   "Content-type:"
#define SWXML_HTTP_HDR_CONTENT_LENGTH "Content-length:"

/* Message header values */
#define SWXML_CONTENT_TYPE_AUTH_MSG "AUTH_MSG"

/* Message Element Tags */
#define SWXML_ELTAG_AUTHEN_REQUEST_MSG    "AuthenRequestMsg"
#define SWXML_ELTAG_AUTHEN_CHAL_MSG       "AuthenChallengeMsg"
#define SWXML_ELTAG_AUTHEN_RESPONSE_MSG   "AuthenResponseMsg"
#define SWXML_ELTAG_AUTHEN_RESULT_MSG     "AuthenResultMsg"

/* Misc Element Tags */
#define SWXML_ELTAG_PROTOCOL          "Protocol"
#define SWXML_ELTAG_SAFEWORD_SYSTEM   "SafeWordSystem"
#define SWXML_ELTAG_ID                "ID"
#define SWXML_ELTAG_CHALLENGES        "Challenges"
#define SWXML_ELTAG_RESPONSES         "Responses"
#define SWXML_ELTAG_FIXED_PWD_CHAL    "FixedPwdChallenge"
#define SWXML_ELTAG_FIXED_PWD_RESP    "FixedPwdResponse"
#define SWXML_ELTAG_DYNAMIC_PWD_CHAL  "DynamicPwdChallenge"
#define SWXML_ELTAG_DYNAMIC_PWD_RESP  "DynamicPwdResponse"
#define SWXML_ELTAG_AUTHEN_RESULT     "AuthenResult"
#define SWXML_ELTAG_AUTHORIZATIONS    "Authorizations"
#define SWXML_ELTAG_ACTION_DATA       "ActionData"
#define SWXML_ELTAG_STATUS_MSG        "StatusMsg"
#define SWXML_ELTAG_CLIENT_TYPE       "ClientType"
#define SWXML_ELTAG_CLIENT_LOCATION   "ClientLocation"
#define SWXML_ELTAG_CLIENT_COMMENT    "ClientComment"
#define SWXML_ELTAG_USER_LOCATION     "UserLocation"
#define SWXML_ELTAG_APPLICATION_DATA  "ApplicationData"
#define SWXML_ELTAG_AGENT             "Agent"
#define SWXML_ELTAG_AGENT_COMMENT     "AgentComment"

/* Misc Attribute Tags */
#define SWXML_ATTAG_TYPE              "type"
#define SWXML_ATTAG_NAME              "name"
#define SWXML_ATTAG_VERSION           "version"
#define SWXML_ATTAG_AUTHEN_NUM        "authenNumber"
#define SWXML_ATTAG_AUTHEN_NAME       "authenName"
#define SWXML_ATTAG_AUTHEN_ATTRS      "authenAttributes"
#define SWXML_ATTAG_PWD               "pwd"
#define SWXML_ATTAG_NEW_PWD           "newPwd"
#define SWXML_ATTAG_MIN_PWD_SIZE      "minPwdSize"
#define SWXML_ATTAG_NEW_PWD_REQUIRED  "newPwdRequired"
#define SWXML_ATTAG_CHALLENGE         "challenge"
#define SWXML_ATTAG_CHAP_CHALLENGE    "chapChallenge"
#define SWXML_ATTAG_CHAP_ID           "chapID"
#define SWXML_ATTAG_ECHO              "echo"
#define SWXML_ATTAG_RESULT            "result"
#define SWXML_ATTAG_RESULT_CODE       "resultCode"
#define SWXML_ATTAG_STATUS_MSG        "statusMsg"
#define SWXML_ATTAG_IP_ADDRESS        "ipAddress"
#define SWXML_ATTAG_HOST_NAME         "hostName"

/* Misc Attribute values */
#define SWXML_ATVAL_PASSED            "passed"
#define SWXML_ATVAL_FAILED            "failed"
#define SWXML_ATVAL_TRUE              "true"
#define SWXML_ATVAL_FALSE             "false"
#define SWXML_ATVAL_NAME              "name"
#define SWXML_ATVAL_CERT              "certificate"

/* Authenticator stuff */
/* names */
#define SWXML_AUTHEN_TYPE_FIXED_PWD   "FixedPwd"
#define SWXML_AUTHEN_TYPE_DYNAMIC_PWD "DynamicPwd"

/* end of XML message defines */

/* SafeWord registry keys */
#define SW_PRODUCT_NAME      "SafeWord"
#define SW_COMPANY_NAME      "Secure Computing"

/* Registry values */
#define SW_DATABASEPATH_KEY "DataBase Path"
#define SW_COMPANY_KEY      "Company Name"
#define SW_PRODUCT_KEY      "Product Name"
#define SW_VERSION_KEY      "Version"
#if WIN32
#define SW_ROOT_REGISTRY_KEY "SOFTWARE\\"SW_COMPANY_NAME"\\"SW_PRODUCT_NAME
#else
#define SW_ROOT_REGISTRY_KEY SW_COMPANY_NAME"\\"SW_PRODUCT_NAME
#endif

/* Error messages for swconfig.c 
 * We can't use the text set messages because the
 * text set hasn't been loaded yet, so we must use
 * these definitions
 *
 */
/*Default error value*/
#define SWCONFIG_NO_ERROR                 "No errors encountered"
/*Boundary errors*/
#define SWCONFIG_EXEC_NAME_LEN_INVALID       "Executable name length invalid"
#define SWCONFIG_HOST_NAME_LEN_INVALID       "Host Name length invalid"
#define SWCONFIG_SITE_NAME_LEN_INVALID       "Site Name length invalid"
#define SWCONFIG_SYSTEM_NAME_LEN_INVALID     "Authentication System Name length invalid"
#define SWCONFIG_DATA_PATH_LEN_INVALID       "Data Files Path length invalid" 
#define SWCONFIG_LOG_NAME_LEN_INVALID        "Status Log Filename length invalid" 
#define SWCONFIG_TEXT_NAME_LEN_INVALID       "Text Set Name length invalid"
#define SWCONFIG_ID_SOURCE_LEN_INVALID       "User ID Source length invalid" 
#define SWCONFIG_APP_CONFIG_LEN_INVALID      "Application Configuration field length invalid" 
#define SWCONFIG_LOG_LABEL_LEN_INVALID       "Status Log Label length invalid" 
#define SWCONFIG_CLIENT_TYPE_LEN_INVALID     "Client Type string length invalid" 
#define SWCONFIG_DATABASE_CIPHER_LEN_INVALID "Database cipher length invalid"
#define SWCONFIG_SERVER_KEY_LEN_INVALID      "Server Key length invalid"
#define SWCONFIG_FILE_PATH_LEN_INVALID       "File Path length invalid"
#define SWCONFIG_REGFILE_VALUE_LEN_INVALID   "Registry/ConfigFile value length invalid"
#define SWCONFIG_SAS_NAME_LEN_INVALID        "SAS Name length invalid" 
/*Invalid data errors*/
#define SWCONFIG_CONFIG_INVALID           "Error in configuration"
#define SWCONFIG_LOG_MODE_INVALID         "Invalid Log Status Mode" 
#define SWCONFIG_FIELD_CODE_INVALID       "Invalid configuration field code"
#define SWCONFIG_REG_KEY_INVALID          "Registry key is empty or invalid"
#define SWCONFIG_EASSP_VERSION_INVALID    "Eassp Version is invalid"
#define SWCONFIG_SSL_VERSION_INVALID      "SSL version is invalid"
#define SWCONFIG_SWEC_DATA_LOCATION_INVALID "SWEC Data Location is invalid"
#define SWCONFIG_SWEC_DATA_MD5_LOCATION_INVALID "SWEC Data MD5 Location is invalid"
#define SWCONFIG_MAX_LOG_LEN_INVALID      "Maximum Log File Length must be greater than zero"
#define SWCONFIG_SWEC_DATA_MD5_INVALID    "SWEC Data MD5 is invalid"
#define SWCONFIG_HOST_CERTIFICATE_INVALID "AAA Server certificate is invalid"

/*Missing data errors*/
#define SWCONFIG_SERVER_SPEC_MISSING      "Server Spec missing"
#define SWCONFIG_SERVER_KEY_MISSING       "Server Key missing"
#define SWCONFIG_SERVER_PORT_MISSING      "Server Port missing"
#define SWCONFIG_HOST_NAME_MISSING        "Host Name missing"
#define SWCONFIG_SYSTEM_NAME_MISSING      "Authentication System Name missing"
#define SWCONFIG_REGFILE_VALUE_MISSING    "Required value from config source missing" 
#define SWCONFIG_REG_KEY_MISSING          "Key missing from registry"
#define SWCONFIG_FIELD_DATA_MISSING       "Config data missing for field"
#define SWCONFIG_REG_VALUE_MISSING        "Registry value not found"
#define SWCONFIG_APPLICATION_NAME_MISSING "Application name must be set first"
/*Retrieval errors*/
#define SWCONFIG_CIPHER_LOADING           "Error retrieving cipher code"
#define SWCONFIG_SYSENV_LOADING           "Error loading system environment"
#define SWCONFIG_REG_LOADING              "Error loading registry config"
#define SWCONFIG_FILE_LOADING             "Error loading file config"
#define SWCONFIG_LIBRARY_LOADING          "Error loading library file"
/*Allocation errors*/
#define SWCONFIG_OUT_OF_MEMORY            "Out of memory"
#define SWCONFIG_HOST_NAME_ALLOC          "Error allocating host name in Server Spec"
#define SWCONFIG_SERVER_SPEC_ALLOC        "Error allocating Server Spec structure"
#define SWCONFIG_PROXY_SITE_ALLOC         "Error allocating Proxy Site record"
#define SWCONFIG_EXEC_FILE_ALLOC          "Error allocating Executable File rec"
/*Misc errors*/
#define SWCONFIG_NOT_AVAILABLE_OS         "Configuration field not available for this OS"
#define SWCONFIG_NOT_INITIALIZED          "Config struct not initialized"
#define SWCONFIG_REGISTRY_NT_ONLY         "The registry only exists on WIN32 platforms"
#define SWCONFIG_CALLBACK_ERROR           "Config callback func returned error"
#define SWCONFIG_CHOOSE_SYSENV_ERROR      "Error choosing system environment"
#define SWCONFIG_NOT_API_FIELD            "Field not available through API"
#define SWCONFIG_RECORD_NOT_FOUND         "Extended config record not found"
#define SWCONFIG_MAXIMUM_RECORDS          "Extended config records limit reached"
#define SWCONFIG_MISSING_CONFIG_FILENAME  "Missing configuration filename"
#define SWCONFIG_CANT_USE_TEXT_INPUT      "Configuration field can not be set from a text source, use the swecSetConfigField function instead"
#define SWCONFIG_CANT_SET_NUM_SERVERS     "Can not set number of servers with SWCFG_NUMBER_OF_SERVERS, use SWCFG_SERVER_SPEC1 or SWCFG_SERVER_SPEC2 instead."
#define SWCONFIG_REGISTRY_READ_ERROR      "Error reading registry"
#define SWCONFIG_EVENT_LOG_REG_ERROR      "Error registering application with event log"
#define SWCONFIG_CONFIG_FILE_OPEN_ERROR   "Can not open configuration file"

/* field lengths */
#define SWEC_HOST_LENGTH         MAX_HOST_NAME_LEN
#define SWEC_STATUS_TEXT_LENGTH  256
#define SWEC_ID_SOURCE_LENGTH    32
#define SWEC_APP_CONFIG_LENGTH   80
#define SWEC_SWEC_DATA_LENGTH    64
#define STLOG_LABEL_SIZE         32
#define SITE_LENGTH              80   /* For PROXY */
#define DB_CIPHER_SIZE           32
#define MAX_USER_RESP_SIZE       33
#define CS_USER_ID_SIZE          64
#define SERVER_NAME_SIZE         SWEC_HOST_LENGTH
#define SYSTEM_NAME_SIZE         32
#define SERVER_CHALL_SIZE        32
#define SERVER_RESPONSE_SIZE     32
#define DEFAULT_MAX_LOG_SIZE     64 /* max KBytes for status log file */
#define FILE_PATH_SIZE           255
#define FILE_NAME_SIZE           8
#define TEXT_NAME_SIZE           FILE_NAME_SIZE
#define MAX_HOST_NAME_LEN        256
#define SERVER_KEY_SIZE          32
#define CHAP_CHALLENGE_SIZE      16
/* status error codes (these end up in errorCode 
 * member of swecapi standard structure
 */
#define SWEC_STATUS_SUCCESS              0
#define SWEC_STATUS_NOT_REGISTERED       27001
#define SWEC_STATUS_INVALID_CONNECT_HDL  27002
#define SWEC_STATUS_ALLOC_ERR            27003
#define SWEC_STATUS_FILE_ERR             27004
#define SWEC_STATUS_NO_SERVER            27005
#define SWEC_STATUS_TIME_OUT             27006
#define SWEC_STATUS_INTERNAL_ERR         27007
#define SWEC_STATUS_INVALID_CONFIG_DATA  27008
#define SWEC_STATUS_CONNECT_FAIL         27009
#define SWEC_STATUS_UNLOADING            27010
#define SWEC_STATUS_DEREGISTERING        27011
#define SWEC_STATUS_USER_ABORT           27012
#define SWEC_STATUS_NO_CALLBACK          27013
#define SWEC_STATUS_NOT_INITIALIZED      27014
#define SWEC_STATUS_APP_REGISTERED       27015
#define SWEC_STATUS_SERVER_BUSY          27016
#define SWEC_SOCK_ACCES_ERR              27017
#define SWEC_SOCK_ADDRINUSE_ERR          27018
#define SWEC_SOCK_ADDRNOTAVAIL_ERR       27019
#define SWEC_SOCK_AFNOSUPPORT_ERR        27020
#define SWEC_SOCK_ALREADY_ERR            27021
#define SWEC_SOCK_CONNABORTED_ERR        27022
#define SWEC_SOCK_CONREFUSED_ERR         27023
#define SWEC_SOCK_CONNRESET_ERR          27024
#define SWEC_SOCK_DESTADDRREQ_ERR        27025
#define SWEC_SOCK_FAULT_ERR              27026
#define SWEC_SOCK_HOSTDOWN_ERR           27027
#define SWEC_SOCK_HOSTUNREACH_ERR        27028
#define SWEC_SOCK_INPROGRESS_ERR         27029
#define SWEC_SOCK_INTR_ERR               27030
#define SWEC_SOCK_INVAL_ERR              27031
#define SWEC_SOCK_ISCONN_ERR             27032
#define SWEC_SOCK_MFILE_ERR              27033
#define SWEC_SOCK_MSGSIZE_ERR            27034
#define SWEC_SOCK_NETDOWN_ERR            27035
#define SWEC_SOCK_NETRESET_ERR           27036
#define SWEC_SOCK_NETUNREACH_ERR         27037
#define SWEC_SOCK_NOBUFS_ERR             27038
#define SWEC_SOCK_NOPROTOOPT_ERR         27039
#define SWEC_SOCK_NOTCONN_ERR            27040
#define SWEC_SOCK_NOTSOCK_ERR            27041
#define SWEC_SOCK_OPNOTSUPP_ERR          27042
#define SWEC_SOCK_PFNOSUPPORT_ERR        27043
#define SWEC_SOCK_PROCLIM_ERR            27044
#define SWEC_SOCK_PROTONOSUPPORT_ERR     27045
#define SWEC_SOCK_PROTOTYPE_ERR          27046
#define SWEC_SOCK_SHUTDOWN_ERR           27047
#define SWEC_SOCK_SOCKTNOSUPPORT_ERR     27048
#define SWEC_SOCK_TIMEDOUT_ERR           27049
#define SWEC_SOCK_WOULDBLOCK_ERR         27050
#define SWEC_SOCK_HOST_NOT_FOUND_ERR     27051
#define SWEC_SOCK_NOTINITIALIZED_ERR     27052
#define SWEC_SOCK_NO_RECOVERY_ERR        27053
#define SWEC_SOCK_SYSNOTREADY_ERR        27054
#define SWEC_STATUS_INVALID_CONFIG_SOURCE 27055
#define SWEC_STATUS_REGISTRY_ERR         27056
#define SWEC_STATUS_EVENT_LOG_ERR        27057
#define SWEC_STATUS_INVALID_PARAM        27058
#define SWEC_STATUS_SERVER_UNAVAILABLE   27059
#define SWEC_STATUS_SSL_ERROR            27060
#define SWEC_STATUS_INVALID_RESPONSE     27061
#define SWEC_STATUS_INVALID_SSL_CERT     27062
#define SWEC_STATUS_LIBRARY_LOAD_ERR     27063
#define SWEC_STATUS_LOAD_SSL_ERR         27064
#define SWEC_STATUS_LOAD_CRYPTO_ERR      27065
#define SWEC_STATUS_INVALID_CONN_POOL_HDL 27066
#define SWEC_STATUS_NO_SUCCESS           27067
#define SWEC_STATUS_ABORTED              27068
#define SWEC_STATUS_INVALID_CLIENT_REQUEST 27069
#define SWEC_STATUS_CLIENT_SEND_ERR      27070
#define SWEC_STATUS_LOGICAL_LOCK_FAILED  27071

/* Defines for structure type designation for
 * each member in the registryConfigFieldValues
 * array.  Members are either from the original
 * swecapi structure, or from the the extended
 * swecapi structure.  Each member of the
 * registryConfigFieldValues array must have one
 * of the below defines associated with it.
 */
#define SWCFG_ORIGINAL (Byte*)0
#define SWCFG_EXTENDED (Byte*)1

/* Defines for accessing fields within
 * the registryConfigFieldValues[]
 * array
 */
#define MemberName(member) member*2
#define MemberType(member) member*2+1

/* shorten status message types for ease */
#define SLI  SWEC_LOG_INFO
#define SLE  SWEC_LOG_ERROR
#define SLD  SWEC_LOG_DEBUG
#define SLW  SWEC_LOG_WARNING
#define SLS  SWEC_LOG_AUDSUCC
#define SLF  SWEC_LOG_AUDFAIL

/* Configuration Sources */
#define SWEC_CONFIG_DEFAULT     0
#define SWEC_CONFIG_FILE        1
#define SWEC_CONFIG_REGISTRY    2

/* SWEC data location strings */
#define SWEC_DATA_IN_APPLICATION "APPLICATION"
#define SWEC_DATA_IN_FILE "FILE"
#define SWEC_DATA_IN_REGISTRY "REGISTRY"

/* SWEC default ssl ciphers */
#define  SWEC_DEFAULT_SSL_CIPHERS "DEFAULT"

/*
 * Configuration field codes
 */
#ifndef SWCONFIG_C
extern Byte *registryConfigFieldValues[];
#else
Byte *registryConfigFieldValues[]={"N/A",SWCFG_ORIGINAL,
#endif
#define SWCFG_SESSION_INFO_FILE_PATH 1    /* path to session info file */
#ifdef SWCONFIG_C
    "Session Info File Path",SWCFG_ORIGINAL,
#endif
#define SWCFG_SERVER_SPEC1           2    /* server spec */
#ifdef SWCONFIG_C
    "Server Spec",SWCFG_ORIGINAL,
#endif
#define SWCFG_SAS_BACKUP_NAMES       3    /* SAS Backup names obsolete */
#ifdef SWCONFIG_C
    "SAS Backup Names",SWCFG_ORIGINAL,
#endif
#define SWCFG_UNKNOWN_1              4    /* ? */
#ifdef SWCONFIG_C
    "?1",SWCFG_ORIGINAL,
#endif
#define SWCFG_UNKNOWN_2              5    /* ? */
#ifdef SWCONFIG_C
    "?2",SWCFG_ORIGINAL,
#endif
#define SWCFG_SEC_NAME               6    /* SEC name */
#ifdef SWCONFIG_C
    "SEC Name",SWCFG_ORIGINAL,
#endif
#define SWCFG_SAR_NAME               7    /* SAR name */
#ifdef SWCONFIG_C
    "SAR Name",SWCFG_ORIGINAL,
#endif
#define SWCFG_WORKSTATION_NAME       8    /* Workstation name */
#ifdef SWCONFIG_C
    "Workstation Name",SWCFG_ORIGINAL,
#endif
#define SWCFG_USER_ID_SOURCE         9    /* User ID Source - goes to app */
#ifdef SWCONFIG_C
    "User Id Source",SWCFG_ORIGINAL,
#endif
#define SWCFG_SYSTEM_NAME            10   /* System Name */
#ifdef SWCONFIG_C
    "System Name",SWCFG_ORIGINAL,
#endif
#define SWCFG_SERVER_KEY             11   /* Server Key */
#ifdef SWCONFIG_C
    "Server Key",SWCFG_EXTENDED,
#endif
#define SWCFG_SERVER_PORT            12   /* Server Port # */
#ifdef SWCONFIG_C
    "Server Port",SWCFG_EXTENDED,
#endif
#define SWCFG_DATABASE_FILES_PATH    13   /* Database Files Path */
#ifdef SWCONFIG_C
    "Database Files Path",SWCFG_ORIGINAL,
#endif
#define SWCFG_DATA_FILES_CIPHER      14   /* Data Files Cipher Phrase */
#ifdef SWCONFIG_C
    "Data Files Cipher",SWCFG_EXTENDED,
#endif
#define SWCFG_USER_STATUS_LOG        15   /* user status log  */
#ifdef SWCONFIG_C
    "User Status Log",SWCFG_ORIGINAL,
#endif
#define SWCFG_CONSOLE_STATUS_LOG     16   /* console status log */
#ifdef SWCONFIG_C
    "Console Status Log",SWCFG_ORIGINAL,
#endif
#define SWCFG_FILE_STATUS_LOG        17   /* file status log */
#ifdef SWCONFIG_C
    "File Status Log",SWCFG_ORIGINAL,
#endif
#define SWCFG_STATUS_LOG_FILE_PATH   18   /* status log filepath */
#ifdef SWCONFIG_C
    "Status Log File Path",SWCFG_ORIGINAL,
#endif
#define SWCFG_MAX_USERS              19   /* max users */
#ifdef SWCONFIG_C
    "Maximum Users",SWCFG_ORIGINAL,
#endif
#define SWCFG_MAX_LOG_FILE_LENGTH    20   /* Max length for log file */
#ifdef SWCONFIG_C
    "Maximum Log File Length",SWCFG_ORIGINAL,
#endif
#define SWCFG_TEXT_SET_NAME          21   /* Text set name */
#ifdef SWCONFIG_C
    "Text Set Name",SWCFG_ORIGINAL,
#endif
#define SWCFG_SHOW_TEXT_CODE_NUMBERS 22   /* Show Text Code Numbers */
#ifdef SWCONFIG_C
    "Show Text Code Numbers",SWCFG_ORIGINAL,
#endif
#define SWCFG_STATUS_LOG_LABEL       23   /* Status log label */
#ifdef SWCONFIG_C
    "Status Log Label",SWCFG_ORIGINAL,
#endif
#define SWCFG_SERVER_SPEC2           24   /* server spec (identical to SPEC1)*/
#ifdef SWCONFIG_C
    "Server Spec2",SWCFG_ORIGINAL,
#endif
#define SWCFG_PROXY_SITES            25   /* Proxy Sites */
#ifdef SWCONFIG_C
    "Proxy Site",SWCFG_EXTENDED,
#endif
#define SWCFG_SITE_CHARACTER         26   /* Site Character */
#ifdef SWCONFIG_C
    "Site Character",SWCFG_EXTENDED,
#endif
#define SWCFG_CLIENT_TYPE            27   /* Client Type */
#ifdef SWCONFIG_C
    "Client Type",SWCFG_ORIGINAL,
#endif
#define SWCFG_EVENT_STATUS_LOG       28   /* event status log */
#ifdef SWCONFIG_C
    "Event Status Log",SWCFG_EXTENDED,
#endif
#define SWCFG_CONFIG_REGISTRY_KEY    29   /* Key for registry config info */
#ifdef SWCONFIG_C
    "Registry Key",SWCFG_EXTENDED,
#endif
#define SWCFG_APPLICATION_NAME       30   /* Application Name */
#ifdef SWCONFIG_C 
    "Application Name",SWCFG_EXTENDED,
#endif
#define SWCFG_EVENT_HANDLE           31   /* Event Handle */
#ifdef SWCONFIG_C
    "Event Handle",SWCFG_EXTENDED,
#endif
#define SWCFG_FILE_POINTER           32   /* File Pointer */
#ifdef SWCONFIG_C
    "File Pointer",SWCFG_EXTENDED,
#endif
#define SWCFG_TEXT_SET               33   /* Text Set */
#ifdef SWCONFIG_C
    "Text Set",SWCFG_ORIGINAL,
#endif
#define SWCFG_TEXT_MODE              34   /* Text Mode */
#ifdef SWCONFIG_C
    "Text Mode",SWCFG_EXTENDED,
#endif
#define SWCFG_STATUS_LOG_CALLBACK    35   /* Status Log Callback */
#ifdef SWCONFIG_C
    "Status Log Callback",SWCFG_ORIGINAL,
#endif
#define SWCFG_USER_DATA              36   /* User Data */
#ifdef SWCONFIG_C
    "User Data",SWCFG_EXTENDED,
#endif
#define SWCFG_REBUILD_DATABASE_FLAG  37   /* Rebuild Database Flag */
#ifdef SWCONFIG_C
    "Rebuild Database Flag",SWCFG_EXTENDED,
#endif
#define SWCFG_CIPHER                 38   /* Cipher */
#ifdef SWCONFIG_C
    "Cipher",SWCFG_EXTENDED,
#endif
#define SWCFG_HOST_NAME              39   /* Host Name */
#ifdef SWCONFIG_C
    "Host Name",SWCFG_EXTENDED,
#endif
#define SWCFG_CONFIG_FILE_PATH       40   /* Config File Path */
#ifdef SWCONFIG_C
    "Config File Path",SWCFG_ORIGINAL,
#endif
#define SWCFG_USE_CONFIG_FILE_FLAG   41   /* Use Config File Flag */
#ifdef SWCONFIG_C
    "Use Config File Flag",SWCFG_ORIGINAL,
#endif
#define SWCFG_MAX_CLIENTS            42   /* Max Clients */
#ifdef SWCONFIG_C
    "Max Clients",SWCFG_EXTENDED,
#endif
#define SWCFG_SAS_NAME               43   /* SAS Name */
#ifdef SWCONFIG_C
    "SAS Name",SWCFG_EXTENDED,
#endif
#define SWCFG_NUM_PROXYS             44   /* Num Proxys */
#ifdef SWCONFIG_C
    "Num Proxys",SWCFG_EXTENDED,
#endif
#define SWCFG_SWEC_DATA              45
#ifdef SWCONFIG_C
    "SWEC Data",SWCFG_ORIGINAL,
#endif
#define SWCFG_NUMBER_OF_SERVERS      46
#ifdef SWCONFIG_C
    "Number Of Servers",SWCFG_ORIGINAL,
#endif
#define SWCFG_STATUS_LOG_CONTEXT     47
#ifdef SWCONFIG_C
    "Status Log Context",SWCFG_ORIGINAL,
#endif
#define SWCFG_WAIT_CALLBACK          48
#ifdef SWCONFIG_C
    "Wait Callback",SWCFG_ORIGINAL,
#endif
#define SWCFG_STATUS_TEXT            49
#ifdef SWCONFIG_C
    "Status Text",SWCFG_ORIGINAL,
#endif
#define SWCFG_APP_CONFIG_1000        50   /* Also defined as 1000 */
#ifdef SWCONFIG_C
    "App Config 1000",SWCFG_ORIGINAL,
#endif
#define SWCFG_APP_CONFIG_1001        51   /* Also defined as 1001 */
#ifdef SWCONFIG_C
    "App Config 1001",SWCFG_ORIGINAL,
#endif
#define SWCFG_APP_CONFIG_1002        52   /* Also defined as 1002 */
#ifdef SWCONFIG_C
    "App Config 1002",SWCFG_ORIGINAL,
#endif
#define SWCFG_APP_CONFIG_1003        53   /* Also defined as 1003 */
#ifdef SWCONFIG_C
    "App Config 1003",SWCFG_ORIGINAL,
#endif
#define SWCFG_SWEC_DATA_LOCATION     54   /* Swec data location */
#ifdef SWCONFIG_C
    "SWEC Data Location",SWCFG_EXTENDED,
#endif
#define SWCFG_EASSP_VERSION          55   /* Eassp version number */
#ifdef SWCONFIG_C
    "Eassp Version",SWCFG_EXTENDED,
#endif
#define SWCFG_SSL_VERSION            56   /* SSL version number */
#ifdef SWCONFIG_C
    "SSL Version",SWCFG_EXTENDED,
#endif
#define SWCFG_SSL_ENABLE             57   /* SSL enable */
#ifdef SWCONFIG_C
    "SSL Enable",SWCFG_EXTENDED,
#endif
#define SWCFG_SSL_CIPHERS            58   /* SSL prefered ciphers */
#ifdef SWCONFIG_C
    "SSL Ciphers",SWCFG_EXTENDED,
#endif
#define SWCFG_SOCKET_TIMEOUT         59   /* Socket Timeout */
#ifdef SWCONFIG_C
    "Socket Timeout",SWCFG_EXTENDED,
#endif
#define SWCFG_SWEC_DATA_MD5          60   /* Swec Data MD5 (AAA cert hash) */
#ifdef SWCONFIG_C
    "SWEC Data MD5",SWCFG_ORIGINAL,
#endif
#define SWCFG_SWEC_DATA_MD5_LOCATION 61   /* Swec data MD5 location */
#ifdef SWCONFIG_C
    "SWEC Data MD5 Location",SWCFG_EXTENDED,
#endif
#define SWCFG_LOAD_LIBRARY_PATH      62   /* path for dynamically loaded libraries */
#ifdef SWCONFIG_C
    "Load Library Path",SWCFG_EXTENDED,
#endif
#define SWCFG_AUTO_PROXY_FLAG        63
#ifdef SWCONFIG_C
    "Auto proxy flag",SWCFG_EXTENDED,
#endif
#define SWCFG_EXECUTABLE_FILE        64
#ifdef SWCONFIG_C
    "Executable File",SWCFG_EXTENDED,
#endif
#define SWCFG_NUM_EXECUTABLES        65
#ifdef SWCONFIG_C
    "Num Executable Files",SWCFG_EXTENDED,
#endif
#define SWCFG_CONN_POOL_SIZE         66
#ifdef SWCONFIG_C
    "Connection Pool Size",SWCFG_EXTENDED,
#endif
#define SWCFG_CONN_POOL_PERSISTENCE  67
#ifdef SWCONFIG_C
    "Connection Pool Persistence",SWCFG_EXTENDED,
#endif
#define SWCFG_CONN_POOL_RETRY_DELAY  68
#ifdef SWCONFIG_C
    "Connection Pool Retry Delay",SWCFG_EXTENDED,
#endif
#define SWCFG_WACA_ACCESS_VERIFICATION_LIBRARY  69
#ifdef SWCONFIG_C
    "WACA Access Verification Library",SWCFG_EXTENDED,
#endif
#define SWCFG_WACA_ACCESS_VERIFICATION_FUNCTION  70
#ifdef SWCONFIG_C
    "WACA Access Verification Function",SWCFG_EXTENDED,
#endif
#define SWCFG_WACA_SESSION_DATA_CACHE_MAX_SIZE  71
#ifdef SWCONFIG_C
    "WACA Session Data Cache Max Size",SWCFG_EXTENDED,
#endif
#define SWCFG_AGENT_NAME             72   /* name of agent, same as client type */
#ifdef SWCONFIG_C
    "Agent Name",SWCFG_EXTENDED,
#endif
#define SWCFG_ACCESS_POLICY_NAME     73   /* name of access policy */
#ifdef SWCONFIG_C
    "Access Policy Name",SWCFG_EXTENDED,
#endif
#define SWCFG_AGENT_IP_ADDRESS       74   /* ip addr of agent */
#ifdef SWCONFIG_C
    "Agent IP Address",SWCFG_EXTENDED,
#endif
#define SWCFG_AGENT_PORT             75   /* port of agent */
#ifdef SWCONFIG_C
    "Agent Port",SWCFG_EXTENDED,
#endif
#define SWCFG_AGENT_INTERNET_DOMAIN  76   /* internet domain of agent */
#ifdef SWCONFIG_C
    "Agent Internet Domain",SWCFG_EXTENDED,
#endif
#define SWCFG_COOKIE_SETTERS         77   /* URLs for cookie setting agents */
#ifdef SWCONFIG_C
    "Cookie Setters",SWCFG_EXTENDED,
#endif
#define SWCFG_AUTHENTICATION_SERVICE 78   /* name of authentictation service to use */
#ifdef SWCONFIG_C
    "Authentication Service",SWCFG_EXTENDED,
#endif
#define SWCFG_PWD_CHANGE_ENABLE      79   /* if true, user may change fixed pwd */
#ifdef SWCONFIG_C
    "Password Change Enable",SWCFG_EXTENDED,
#endif
#define SWCFG_CROSS_DOMAIN_SSO_ENABLE 80   /* if true, cross domain single signon is enabled */
#ifdef SWCONFIG_C
    "Cross Domain SSO Enable",SWCFG_EXTENDED,
#endif
#define SWCFG_LOGIN_SERVER           81   /* typically the URL of a login server */
#ifdef SWCONFIG_C
    "Login Server",SWCFG_EXTENDED,
#endif
#define SWCFG_CLIENT_IP_CHECK_ENABLE 82   /* if true, the client IP must match the authentication IP */
#ifdef SWCONFIG_C
    "Client IP Check Enable",SWCFG_EXTENDED,
#endif
#define SWCFG_WLS_FORM_SET           83   /* Web Login server form set */
#ifdef SWCONFIG_C
    "WLS Form Set",SWCFG_EXTENDED,

	0,SWCFG_ORIGINAL};                      /* End Marker */
#endif
/*** IMPORTANT - when adding parameters, adjust the define below */
#define SWCFG_END_OF_PARAMS          84  /* set this value to 1 greater than the last 
                                           parameter id number. This allows us to loop
                                           through the numbers and know when to quit. */

/* delimiters for multiple parameters in config data file */
#define CONFIG_DELIMITERS " ,;:{}[]()|\\/\t<>"

/* Bit codes for message types 
 * If you want to add a message type,
 * add only to the SWEC_LOG_* list as
 * it is the list that is documented.
 * The STLOG_MASK_* list is not.
 */ 
#define SWEC_LOG_NONE      0x0000
#define SWEC_LOG_INFO      0x0001
#define SWEC_LOG_ERROR     0x0002
#define SWEC_LOG_DEBUG     0x0004
#define SWEC_LOG_ALL       0xFFFF
#define SWEC_LOG_WARNING   0x000F
#define SWEC_LOG_AUDSUCC   0x0020
#define SWEC_LOG_AUDFAIL   0x0040
#define STLOG_MASK_ALL     SWEC_LOG_ALL
#define STLOG_MASK_NONE    SWEC_LOG_NONE
#define STLOG_MASK_INFO    SWEC_LOG_INFO
#define STLOG_MASK_ERROR   SWEC_LOG_ERROR
#define STLOG_MASK_DEBUG   SWEC_LOG_DEBUG
#define STLOG_MASK_WARNING SWEC_LOG_WARNING
#define STLOG_MASK_AUDSUCC SWEC_LOG_AUDSUCC /* Successful audit */
#define STLOG_MASK_AUDFAIL SWEC_LOG_AUDFAIL /* Failed audit */

/* shorten status message types for ease */
#define SLI  SWEC_LOG_INFO
#define SLE  SWEC_LOG_ERROR
#define SLD  SWEC_LOG_DEBUG
#define SLW  SWEC_LOG_WARNING
#define SLS  SWEC_LOG_AUDSUCC
#define SLF  SWEC_LOG_AUDFAIL

/* Default config values (assigned during init) */
#define SWCFG_DEFAULT_SYSTEM_NAME  "STANDARD"
#define SWCFG_DEFAULT_TEXTSET_NAME "STANDARD"
#define SWCFG_DEFAULT_SERVICE_PORT 7482
#define SWCFG_DEFAULT_APP_NAME     "SafeWord app"
#define SWCFG_DEFAULT_SOCKET_TIMEOUT 15
#define SWCFG_DEFAULT_CONN_POOL_SIZE 0
#define SWCFG_DEFAULT_CONN_POOL_PERSISTENCE 300
#define SWCFG_DEFAULT_CONN_POOL_RETRY_DELAY 10
#define SWCFG_DEFAULT_WACA_ACCESS_VERIFICATION_FUNCTION "accessVerificationPlugin"
#define SWCFG_DEFAULT_WACA_SESSION_DATA_CACHE_MAX_SIZE 20

typedef unsigned long SwecHdl; /* Typedef for handles */

typedef struct 
{
    Byte    *execFile;
    Byte    *execPath;
    Byte    *execCmdOpt;
} ExecutableRec;

typedef struct
{
    Byte    site[SITE_LENGTH];
    Byte    host[MAX_HOST_NAME_LEN];
    int     port;
    Byte    key[ SERVER_KEY_SIZE + 1 ];
} ProxySiteRec;

/* Record for user data */
typedef struct
{
    Byte    id[CS_USER_ID_SIZE+1];             /* user's id */
    Byte    response[3][MAX_USER_RESP_SIZE];   /* user responses for 3 authens */
    Byte    *actionPtr;
    uWord   actionLength;
} swecUserRec;

/* Authentication server specification */
#define SWEC_CERT_MD5_LENGTH     48
typedef struct
{
    Byte    *host;
    Word    servicePort;
    Word    weight;
    Word    maxConnections;
	char    certificateMD5[SWEC_CERT_MD5_LENGTH]; /* hex string with colon separators */
} SwecServerRec;

/* Data and control record for swecDeregister() */
typedef struct
{
    Byte    swecData[SWEC_SWEC_DATA_LENGTH];
    Byte    statusText[SWEC_STATUS_TEXT_LENGTH];
} SwecDeregisterRec;

typedef struct
{
	Byte	*host;
	Word	servicePort;
	Byte	*MD5;
} SwecDataMD5Rec;

/* This is the structure used by the swecRegister() API call
 * to pass config data for an application. Do not add or
 * change the contents this structure, it is part of the published
 * interface. Use the swecRegisterEx1() function if not all
 * config fields are available here.
 *
 * BASIC FIELD SET
 *
 * useSwecDataFileFlag : Uses swec data file if TRUE
 * swecData            : Pointer to swec data
 * useConfigFileFlag   : Force config source priority to be from file
 * configFilePath      : Path and filename for config data
 * numberOfServers     : SWAS servers
 * servers             : SWAS servers, alloc's array
 * systemName          : System DB to use
 * dataFilesDir        : Optional,path to data files
 * consoleLogMask      : Status log to console mask
 * userLogMask         : Status log to user mask
 * fileLogMask         : Status log to file mask
 * statusFilePath      : Log file path and name
 * maxStatusFileLength : Max size of log file in kb
 * textSetName         : Optional test set
 * textCodeFlag        : True SHOW_CODE,else HIDE_CODE
 * maxUsers            : Maximum number of users
 * userIdSource        : Where to grab user's id
 * appConfig1000       : User defined 
 * appConfig1001       : User defined
 * appConfig1002       : User defined
 * appConfig1003       : User defined
 * statusLogCallback() : Callback func from SWEC module for status log
 * statusLogContext    : Application context, passed as arg in statusLogCallback
 * statusLogLabel      : Log ident label
 * waitCallback()      : Callback to allow application to switch threads during wait
 * clientType          : Optional client type (agent name) string
 * statusText          : Error msg returned by getSafeWordConfig()
 */
typedef struct
{
    /* BASIC FIELD SET */
    Word          useSwecDataFileFlag;
    Byte          *swecData;
    Word          useConfigFileFlag;
    Byte          *configFilePath;
    Word          numberOfServers;
    SwecServerRec *servers;
    Byte          *systemName;
    Byte          *dataFilesDir;
    Word          consoleLogMask;
    Word          userLogMask;
    Word          fileLogMask;
    Byte          *statusFilePath;
    LongWord      maxStatusFileLength;
    Byte          *textSetName;
    Word          textCodeFlag;
    Word          maxUsers;
    Byte          userIdSource[SWEC_ID_SOURCE_LENGTH];
    Byte          appConfig1000[SWEC_APP_CONFIG_LENGTH];
    Byte          appConfig1001[SWEC_APP_CONFIG_LENGTH];
    Byte          appConfig1002[SWEC_APP_CONFIG_LENGTH];
    Byte          appConfig1003[SWEC_APP_CONFIG_LENGTH];
    void          (*statusLogCallback)PROTO((SwecHdl statusLogContext,
                      Byte *statusStr));
    SwecHdl       statusLogContext;
    Byte          *statusLogLabel;
    void          (*waitCallback)PROTO((int waitCode, int *abortFlag));
    Byte          *clientType; /* also called agent name */
    Byte          statusText[SWEC_STATUS_TEXT_LENGTH];
} SwecRegisterRec;

typedef struct
{
    /* BASIC FIELD SET */                   /* comments contain field codes and text */
    Word          useSwecDataFileFlag;      /* not accessible via function call (see swecDataLocation) */
    Byte          *swecData;                /* SWCFG_SWEC_DATA () "SWEC Data" */
    Word          useConfigFileFlag;        /* SWCFG_USE_CONFIG_FILE_FLAG () "Use Config File Flag" */
    Byte          *configFilePath;          /* SWCFG_CONFIG_FILE_PATH () "Config File Path" */
    Word          numberOfServers;          /* SWCFG_NUMBER_OF_SERVERS () "Number Of Servers" read only */
                                            /* this and next - SWCFG_SERVER_SPEC1 (2)"Server Spec" */
    SwecServerRec *servers;                 /* or SWCFG_SERVER_SPEC2 ()"Server Spec2" to set them */
											/* this member must follow numberOfServers */
    Byte          *systemName;              /* SWCFG_SYSTEM_NAME () "System Name" */
    Byte          *dataFilesDir;            /* SWCFG_DATABASE_FILES_PATH () "Database Files Path" */
    Word          consoleLogMask;           /* SWCFG_CONSOLE_STATUS_LOG () "Console Status Log" */
    Word          userLogMask;              /* SWCFG_USER_STATUS_LOG () "User Status Log" */
    Word          fileLogMask;              /* SWCFG_FILE_STATUS_LOG () "File Status Log" */
    Byte          *statusFilePath;          /* SWCFG_STATUS_LOG_FILE_PATH () "Status Log File Path" */
    LongWord      maxStatusFileLength;      /* SWCFG_MAX_LOG_FILE_LENGTH () "Maximum Log File Length" */
    Byte          *textSetName;             /* SWCFG_TEXT_SET_NAME () "Text Set Name" */
    Word          textCodeFlag;             /* SWCFG_SHOW_TEXT_CODE_NUMBERS () "Show Text Code Numbers" */
    Word          maxUsers;                 /* SWCFG_MAX_USERS () "Maximum Users" */
    Byte          userIdSource[SWEC_ID_SOURCE_LENGTH];   /* SWCFG_USER_ID_SOURCE () "User Id Source" */
    Byte          appConfig1000[SWEC_APP_CONFIG_LENGTH]; /* SWCFG_APP_CONFIG_1000 ( or 1000) "App Config 1000" */
    Byte          appConfig1001[SWEC_APP_CONFIG_LENGTH]; /* SWCFG_APP_CONFIG_1001 ( or 1001) "App Config 1001" */
    Byte          appConfig1002[SWEC_APP_CONFIG_LENGTH]; /* SWCFG_APP_CONFIG_1002 ( or 1002) "App Config 1002" */
    Byte          appConfig1003[SWEC_APP_CONFIG_LENGTH]; /* SWCFG_APP_CONFIG_1003 ( or 1003) "App Config 1003" */
    void          (*statusLogCallback)PROTO((SwecHdl statusLogContext,
                      Byte *statusStr));    /* SWCFG_STATUS_LOG_CALLBACK () "Status Log Callback" */
    SwecHdl       statusLogContext;         /* SWCFG_STATUS_LOG_CONTEXT () "Status Log Context" */
    Byte          *statusLogLabel;          /* SWCFG_STATUS_LOG_LABEL () "Status Log Label" */
    void          (*waitCallback)PROTO((int waitCode,
                      int *abortFlag));     /* SWCFG_WAIT_CALLBACK () "Wait Callback" */
    Byte          *clientType;              /* SWCFG_CLIENT_TYPE () "Client Type" or "Agent Name" */
    Byte          statusText[SWEC_STATUS_TEXT_LENGTH]; /* not accessible via function calls */
    /* EXTENDED FIELD SET */ 
/*jjt    void          (*userLogFunc)PROTO((uLongWord iUserData, Byte *iStatusStr)); */
    uLongWord     userData;                     /* SWCFG_USER_DATA () "User Data" */
    Byte          serverKey[SERVER_KEY_SIZE+1]; /* SWCFG_SERVER_KEY () "Server Key"*/
    Word          serverPort;                   /* SWCFG_SERVER_PORT () "Server Port" */
    Byte          cipher[DB_CIPHER_SIZE];       /* SWCFG_CIPHER () "Cipher" */
    Word          maxClients;                   /* SWCFG_MAX_CLIENTS () "Max Clients" */
    ProxySiteRec  *proxySites;                  /* SWCFG_PROXY_SITES () "Proxy Site" */
    Word          numProxys;                    /* SWCFG_NUM_PROXYS () "Num Proxys" */
    Byte          siteChar;                     /* SWCFG_SITE_CHARACTER () "Site Character" */
    Byte          sasName[SERVER_NAME_SIZE+1];  /* SWCFG_SAS_NAME () "SAS Name" */
    Ptr           textSet;                      /* SWCFG_TEXT_SET () "Text Set"*/
    Word          textMode;                     /* SWCFG_TEXT_MODE () "Text Mode" */
    Word          eventLogMask;                 /* SWCFG_EVENT_STATUS_LOG () "Event Status Log" */
    HANDLE        eventHandle;                  /* SWCFG_EVENT_HANDLE () "Event Handle" */
    FILE          *filePtr;                     /* SWCFG_FILE_POINTER () */
    Byte          configRegistryKey[FILE_PATH_SIZE+1]; /* SWCFG_CONFIG_REGISTRY_KEY () "Registry Key" */
    Byte          applicationName[SYSTEM_NAME_SIZE+1]; /* SWCFG_APPLICATION_NAME () "Application Name" */
    Word          rebuildFlag;                  /* SWCFG_REBUILD_DATABASE_FLAG () "Rebuild Database Flag" */
    Byte          *swecDataLocation;            /* SWCFG_SWEC_DATA_LOCATION () "SWEC Data Location" */
    Word          easspVersion;                 /* SWCFG_EASSP_VERSION () "Eassp Version" */
    Byte          *easspVersionStr;             /* string storage for above */
    Word          sslVersion;                   /* SWCFG_SSL_VERSION () "SSL Version" */
    Word          sslEnable;                    /* SWCFG_SSL_ENABLE () "SSL Enable" */
    Byte          *sslCiphers;                  /* SWCFG_SSL_CIPHERS () "SSL Ciphers" */
    Word          socketTimeout;                /* SWCFG_SOCKET_TIMEOUT () "Socket Timeout" */
	SwecDataMD5Rec **swecDataMD5Recs;			/* array of ptrs to records, last ptr must be null */
    Byte          *swecDataMD5Location;         /* SWCFG_SWEC_DATA_LOCATION () "SWEC Data Location" */
	Byte          *loadLibraryPath;             /* SWCFG_LOAD_LIBRARY_PATH () "Load Library Path" */
    Word          autoProxyFlag;
    /* Used by swsm */
    ExecutableRec *executableFiles;
    Word          numExecutables;
    /* connection pool stuff */
    Word          connPoolSize;
    LongWord      connPoolPersistence;
    LongWord      connPoolRetryDelay;
    Byte          *wacaAccessVerificationLib;
    Byte          *wacaAccessVerificationFunc;
    LongWord      wacaSessionDataCacheMaxSize;
	Byte          *accessPolicyName;             /* for access control agents */
	Byte		  *agentIpAddr;                  /* for AAA to contact agent */
	int           agentPort;                     /* for AAA to contact agent */
    Byte          *agentInternetDomain;          /* name of agent internet domain */
    Byte          *cookieSetters;                /* URLs for cookie setting agents */
    Byte          *authenticationService;        /* name of authentictation service to use */
    Word          pwdChangeEnable;               /* if true, user may change fixed pwd */
    Word          ssoEnable;                     /* if true, single signon is enabled */
    Byte          *loginServer;                  /* typically the URL of a login server */
    Word          clientIpCheckEnable;           /* if true, client IP must match authen IP for access */
    Byte          *wlsFormSet;                   /* name of web login server form set to use */
} SafeWordConfig;

/* application control block */
typedef struct
{
    int               errorCode;            /* one of SWEC_STATUS error codes */
    SwecHdl           appHandle;            /* unique identifier for an app */
    SafeWordConfig    *config;               /* configuration data */
    int               initStatusCode;       /* status of the init */
    SwListRec           clientList;           /* list w/ data for clients */
    int               lastConnectionHandle; /* counter to generate ids */
    int               *serverConnectCount;  /* alloc'd array of counters for
                                             * servers
                                             */
    int               lastServerConnected;  /* track servers for load
                                             *  balancing
                                             */
    int               useSwecDataFileFlag;
    LongWord          swecDataLock;         /* let 1 client at a time
                                             * access data
                                             */
    Byte              serverChall[SERVER_CHALL_SIZE+1];
    Byte              serverResp[SERVER_RESPONSE_SIZE+1];
    uLongWord         challRespTime;        /* time server chal/resp was
                                             * updated
                                             */ 
    void              (*statusLogCallback)PROTO((SwecHdl statusLogContext,
                          Byte *statusStr));
    SwecHdl           statusLogContext;
    SwecDeregisterRec *dereg;               /* deregistration data from
                                             * app, owned by app
                                             */
    int               destroyConfigOnDeregister; /* boolean */
    void              *sslContext;
	int				  numberOfServerMD5s;   /* number of items in array below */
	Byte			  **serverMD5s;			/* array of ptrs to MD5 cert hashes corresponding to
											 * the array of servers in config struct */
    SwecHdl           connPoolHandle;       /* connection pool */
    Byte              *agentType;
} AppRec;


#if SYSENV_TYPE==NO_SYSENV_TYPE
/*SW_EXTERN Word initSafeWordConfig    PROTO((Byte   useConfigFileFlag,
                                         AppRec *ioApp));
SW_EXTERN void uninitSafeWordConfig  PROTO((AppRec *ioApp));
SW_EXTERN Word getSafeWordConfig     PROTO((AppRec *ioApp,
                                         Word (*callback)PROTO((AppRec *app)),
                                         Word   *fieldList)); */
SW_EXTERN Word getConfigStruct       PROTO((AppRec *ioApp,
                                         Word (*callback)PROTO((AppRec *app)),
                                         Word   *fieldList));
SW_EXTERN Word createConfig          PROTO((SafeWordConfig **configPtrPtr,
                                         Word source, Byte *identifier,
                                         Byte *statusText));
SW_EXTERN void destroyConfig         PROTO((SafeWordConfig *configPtr));
SW_EXTERN Word setConfigField        PROTO((SafeWordConfig *configPtr,
                                           Word fieldCode, void *dataPtr,
                                           char *statusText));
SW_EXTERN Word getConfigField        PROTO((SafeWordConfig *configPtr,
                                           Word fieldCode, void **dataPtrPtr,
                                           char *statusText));
#else
/*SW_EXTERN Word initSafeWordConfig    PROTO((Obj sysObj, 
                                         Byte useConfigFileFlag, 
                                         AppRec *ioApp));
SW_EXTERN void uninitSafeWordConfig  PROTO((Obj sysObj, AppRec *ioApp));
SW_EXTERN Word getSafeWordConfig     PROTO((Obj sysObj, AppRec *ioApp,
                                         Word (*callback)PROTO((AppRec *app)),
                                         Word *fieldList)); */
SW_EXTERN Word createConfig          PROTO((Obj sysObj, SafeWordConfig **configPtrPtr,
                                         Word source, Byte *identifier,
                                         Byte *statusText));
SW_EXTERN void destroyConfig         PROTO((Obj sysObj, SafeWordConfig *configPtr));
SW_EXTERN Word setConfigField        PROTO((Obj sysObj, SafeWordConfig *configPtr,
                                           Word fieldCode, void *dataPtr,
                                           char *statusText));
SW_EXTERN Word getConfigField        PROTO((Obj sysObj, SafeWordConfig *configPtr,
                                           Word fieldCode, void **dataPtrPtr,
                                           char *statusText));
#endif
SW_EXTERN Word readValueFromRegistry PROTO((Byte *registryKey, Byte *value,
                                         Byte *buf, Byte *message));
SW_EXTERN SwecDataMD5Rec *createSwecDataMD5Rec PROTO((Byte *host, Word port, Byte *MD5));
SW_EXTERN void mergeSwecDataMD5      PROTO((SwecDataMD5Rec **data,
										    SwecDataMD5Rec *rec));
SW_EXTERN void freeSwecDataMD5       PROTO((SwecDataMD5Rec **data));
SW_EXTERN Byte *buildSwecDataMD5     PROTO((SwecDataMD5Rec **data,
										    SwecDataMD5Rec **newData,
											Byte *delimiter));
SW_EXTERN SwecDataMD5Rec **parseSwecDataMD5 PROTO((Byte *text));

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _MSC_VER */

#ifdef  WIN32
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* WIN32 */

#endif /* SWCONFIG_H */
