// DynCertificate.cpp: implementation of the CDynCertificate class.
//
//////////////////////////////////////////////////////////////////////
#include "stdAfx.h"
#include "DynCertificate.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
** Definitions and prototypes for SWEC API.
*/

#include "swapi/swconfig.h"
#include "swapi/swecapi.h"

/*
** Path to text file that contains configuration data.  The format for
** the file and the available fields are described in the Authentication SDK
** documentation.
*/

#define CONFIG_FILE_PATH "..\\data\\auth.cfg"

//global variable for dynamic certificate

static char* g_dyn_user = NULL;
static char* g_dyn_pwd = NULL;

/* local functions */
int doRegister();
int doOpen (SwecHdl appHandle);
int doAuthenticate (SwecHdl appHandle, SwecHdl conectionHandle);
void dialogCallbackFunction (SwecHdl dialogContext, SwecDialogRec * dialogPtr);
void printSwecError (int rc, char *func);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDynCertificate::CDynCertificate()
{

}

CDynCertificate::~CDynCertificate()
{

}

int CDynCertificate::DynInit()
{

	int             rc;
    SwecVersionRec  swecVersionInfo;

    /*
    ** swecVersion() returns version information for the SWEC API.
    ** The format of the description string is:
    **
    **   SafeWord EASSP Client V<major>.<minor>.<revision>
    **
    ** The transport string is typically "TCPIP" or "SPX/IPX"
    */

    swecVersion(&swecVersionInfo);
    //printf("%s using %s transport.\n",
    //    swecVersionInfo.description, swecVersionInfo.transport);

    /*
    ** The application is responsible for initializing the SWEC
    ** module by calling swecInit.  This must be called before registering
    ** the application. It only needs to be called once even if multiple
    ** threads are registering separately. In a multi-threaded invironment,
    ** make the initialization in code that is mutually exclusive.
    */

    rc = swecInit();
    if (rc != SWEC_STATUS_SUCCESS)
    {
       // printSwecError (rc, "swecInit");
       // exit (0);
		return 0;
    }

	return 1;

}

int CDynCertificate::DynUninit()
{
	int rc = -1;
	 /*
    ** Call swecUninit when you are done.  This releases any resources
    ** acquired in the swecInit call.  If there are still registered
    ** applications(threads), this will fail with a return code of
    ** SWEC_STATUS_APP_REGISTERED.
    **
    ** If you need to force the deregistration of any registered applications
    ** (threads), you can set the parameter to swecUninit to SWEC_TRUE.
    ** This should only be done in a forced shutdown situation.
    */

    rc = swecUninit(SWEC_FALSE);
    if (rc != SWEC_STATUS_SUCCESS)
    {
       // printSwecError (rc, "swecUninit");
       // exit (0);
		return 0;
    }

	return 1;
}

int CDynCertificate::DynVerify(char *user, char *pwd)
{
	if(user == NULL || pwd == NULL)
		return AUTH_ERR_OTHER;

	if(strlen(user) <=0 || strlen(pwd)<=0)
		return AUTH_ERR_OTHER;

	g_dyn_user= user;
	g_dyn_pwd = pwd;

	return doRegister();  /* Register the application, and do authentication */

//	return 1;
}

/*******************************************************************************
**
**  void doRegister (void) - registers and unregister app with SWEC API
**
**  Before using any other API calls, the SWEC API must be configured with
**  the parameters for authentication.  This is accomplished through the
**  swecRegister call.  Information is thus passed to the API concerning the
**  authentication server (or servers, in the case of multiples), network
**  connection security, message logging, etc.
**
**  You can register several different applications through a single copy
**  of the SWEC API.  The API will maintain separate registration information
**  for each application.  This might be useful for a client that handles
**  more than one type of authentication service.
**
*******************************************************************************/

int doRegister ()
{
    SwecHdl          appHandle;
    SafeWordConfig   regRec;
    /* SwecRegisterRec  regRec; */
    int              rc;

	int rc_open;


    /*
    ** To keep the link to the authentication server secure, both sides
    ** need to maintain encryption keys.  On the client side, these are
    ** typically kept in the swec.dat file.  Setting the useSwecDataFileFlag
    ** to TRUE causes the SWEC API to use this file.  If set to FALSE, the
    ** application will have to maintain the data itself and pass it to
    ** the API.
    */

    regRec.useSwecDataFileFlag = SWEC_TRUE;


    /*
    ** Several of the configuration fields needed for registration can be
    ** read automatically from a configuration file.  Setting useConfigFileFlag
    ** and providing a config file name in configFilePath causes the API to
    ** do this at the time swecRegister is called.  The SwecRegisterRec fields
    ** read from the configuration file are:
    **
    ** Config file      SwecRegisterRec
    **   Field ID       Field
    ** ---------------  ---------------------------------------------------------
    ** (implied by 02)  numberOfServers
    **       02         servers  -- includes SwecServerRec fields:
    **                     host
    **                     servicePort
    **                     weight
    **       10         systemName
    **       13         dataFilesDir
    **       16         consoleLogMask
    **       15         userLogMask
    **       17         fileLogMask
    **       18         statusFilePath
    **       20         maxStatusFileLength
    **       21         textSetName
    **       22         textCodeFlag
    **       19         maxUsers
    **       09         userIdSource
    **       24         clientType
    **
    ** In addition, there are four user-defined fields available for use
    ** by your application:
    **
    ** Config file      SwecRegisterRec
    **   Field ID       Field
    ** ---------------  ---------------------------------------------------------
    **      1000        appConfig1000
    **      1001        appConfig1001
    **      1002        appConfig1002
    **      1003        appConfig1003
    **
    ** See sweccfg.doc for more details of configuration file format.
    */
    
    regRec.useConfigFileFlag = SWEC_TRUE;
    regRec.configFilePath = CONFIG_FILE_PATH;


    /*
    ** These fields can be used if you want your application to intercept
    ** status messages from SWEC API and process them.  Set them to NULL
    ** if you won't be doing this.
    */
    
    regRec.statusLogCallback = NULL;
    regRec.statusLogContext = (SwecHdl)NULL;


    /*
    ** The waitCallback field can be pointed to a function that will
    ** receive control if the SWEC API is blocked from authentication
    ** by unavailable resources.  It is intended for use in multi-
    ** threaded environments where the function can switch to another
    ** thread and possibly free up resources that thread has allocated.
    ** Set it to NULL if you don't want to handle that condition.
    */
    
    regRec.waitCallback = NULL;


    //rc = swecRegister(&appHandle, &regRec);        /* Register this app. */
	rc = swecRegister(&appHandle, (SwecRegisterRec *)&regRec);        /* Register this app. */
    if (rc != SWEC_STATUS_SUCCESS)
    {
        printSwecError (rc, "swecRegister");
        return AUTH_ERR_OTHER;
    }

    rc_open = doOpen (appHandle);      /* Perform open and authenticate operations */
   

    /*
    ** swecDeregister must be called to close all currently open connections
    ** in the application and to deallocate all resources that were allocated
    ** by the call to the swecRegister function.  If the useSwecDataFileFlag
    ** is set, the data file will also be updated and closed.  This function
    ** should be called after it has been determined that no further
    ** authentication services will be needed.
    */
    
	//有些问题，应该定义一个变量作为来保存输出
	SwecDeregisterRec deregRec;
    rc = swecDeregister(appHandle, &deregRec);
    if (rc != SWEC_STATUS_SUCCESS)
    {
        printSwecError (rc, "swecDeregister");
        return AUTH_ERR_OTHER;
    }

	return rc_open;
}

/*******************************************************************************
**
**  void doOpen (SwecHdl appHandle) - open and close EASSP connections to server
**
**  Before an authentication can be performed, a network connection between
**  the SWEC API and the authentication server must be established.  This
**  Function opens and closes that connection using the configuration
**  information established by swecRegister.  It is possible for an application
**  to have more than one connection open to the server, but each authentication
**  requires its own connection that is unique from that of other
**  authentications occuring at the same time.
**
*******************************************************************************/

int doOpen (SwecHdl appHandle)
//SwecHdl appHandle;
{
    SwecOpenRec   openRec;
    SwecCloseRec  closeRec;
    SwecHdl       connectionHandle;
    int           rc;     

	int rc_auth;

    /*
    ** This sets the maximum number of seconds to spend trying to connect
    ** to the server or wait for a message from the server. The wait time can
    ** also be set in the configuration. To use the value in the configuration,
    ** set this to -1.
    */

    openRec.waitTime = -1;


    /*
    ** If multiple servers are available (swecRegisterRec.numberOfServers > 1),
    ** it is possible to open more than one of them at a time.  Setting this
    ** field to SWEC_TRUE will cause this to happen.  Set to SWEC_FALSE for one.
    */

    openRec.openAllFlag = SWEC_FALSE;


    /*
    ** If multiple servers are available but you are only opening one,
    ** you can select which one to open, or let SWEC API choose automatically
    ** based on weight and least-recently-used.  This code enables the auto-
    ** select method, see swecapi.doc for details of manual selection.
    */

    openRec.server = NULL;
    openRec.serverNumber = 0;


    rc = swecOpen(appHandle, &connectionHandle, &openRec);  /* Open a connection */
    if (rc != SWEC_STATUS_SUCCESS)
    {
        printSwecError (rc, "swecOpen");
        return AUTH_ERR_NET;
    }

    rc_auth = doAuthenticate (appHandle, connectionHandle);   /* Do the authentication */


    /*
    ** Since we can open multiple connections to an authentication server,
    ** swecClose normally closes just one of them, leaving the others still
    ** open.  If we set closeAllFlag in the SwecCloseRec to SWEC_TRUE,
    ** it would cause all open connections for the application to be closed.
    */
    
    closeRec.closeAllFlag = SWEC_FALSE;
    rc = swecClose(appHandle, connectionHandle, &closeRec);
    if (rc != SWEC_STATUS_SUCCESS)
    {
        printSwecError (rc, "swecClose");
        return AUTH_ERR_NET ;
    }

	return rc_auth;
}


/*******************************************************************************
**
**  void doAuthenticate (SwecHdl appHandle, SwecHdl conectionHandle)
**                                          - perform a user authentication
**
**  This is where the actual authentication - where we determine if the user is
**  really who he says he is - takes place.  Each authentication requires its
**  own separate connection, as provided by swecOpen, from other authentications
**  taking place at the same time.  However, one open connection can be used
**  for multiple authentications if they are performed serially (i.e. not
**  concurrantly).
**
**  Authentication is normally performed by querying the user for identification
**  and secret knowledge, but the information could just as easily be provided
**  by an application if it posessed the necessary secret information to
**  satisfy the authentication challenges.
**
*******************************************************************************/

int doAuthenticate (SwecHdl appHandle, SwecHdl connectionHandle)
//SwecHdl appHandle, connectionHandle;
{
    int           rc;     
    SwecAuthenRec authenRec;

    /*
    ** The authenRec.waitTime is not used. We will set it to -1 just to play it
    ** safe for the future.
    */

    authenRec.waitTime = -1;


    /*
    ** If the application already knows the user ID and/or password, it can
    ** provide them here and perform an authentication.  Set the fields
    ** to NULL if they are unknown. This will cause the dialogCallback
    ** routine to be called to get this information.
    */
    
   // authenRec.userId = NULL;
   // authenRec.password = NULL;

	authenRec.userId = g_dyn_user;
	authenRec.password = g_dyn_pwd;

    /*
    ** If this field has the SWEC_PASS_USER bit set, it will cause the
    ** authentication to pass the user.  This may be useful if you just
    ** want to know the action data for the user without having to know
    ** his password.  Set the field to SWEC_FALSE for normal authentication.
    */
    
    authenRec.passUserFlag = SWEC_FALSE;


    /*
    ** The callback function is used to create a dialog between the
    ** authentication server and the user (or application) in order to
    ** provide the information needed for authentication.  Through the
    ** callback function, SafeWord can display messages, prompt for
    ** input, receive responses, provide challenges for dynamic passwords,
    ** etc.
    **
    ** The dialogCallback field is set to point to a function that will
    ** be called when more information is needed.  That function will be
    ** notified of what information is needed, any text that needs to be
    ** displayed, etc.  The function displays any messages, inputs text,
    ** and passes it back to the SWEC API.  See the function itself for
    ** more details.
    **
    ** The dialogCallback field can be set to NULL if it is expected
    ** that all needed information has already been provided in the
    ** authenRec.  swecAuthen will return with an error if it needed to
    ** call the callback function and it was not available.
    **
    ** The dialogContext field value will be passed to the callback
    ** function and can point to any memory structure needed to provide
    ** context for the function.  This may be used, for instance, to
    ** allow the callback function to respond with the data appropriate
    ** to a particular authentication when more than one are taking place
    ** simultaneously.
    */
    
    authenRec.dialogCallback = &dialogCallbackFunction;
    authenRec.dialogContext = (SwecHdl)NULL;


    /*
    ** If the application is a server for some other network resource,
    ** for example a router or commbox, these fields can be used to specify
    ** the other resource. In the case of a web agent, you could set the peerAddr
    ** to the user's IP address. Generally, the IP address is prefered. This might be used by SafeWord to determine
    ** whether or not the user is authorized access through that resource,
    ** or other determinations.  If this is not the case, set these to NULL.
    */
    
    authenRec.peerName = NULL;
    authenRec.peerAddr = NULL;


    /*
    ** A comment can be provided that will appear in the user log along
    ** with the entry for the authentication.  Set to NULL if not used.
    */

    authenRec.comment =  NULL;

    rc = swecAuthen(appHandle, connectionHandle, &authenRec);
    if (rc != SWEC_STATUS_SUCCESS)
    {
        printSwecError (rc, "swecAuthen");
        return AUTH_ERR_OTHER;
    }

    if (authenRec.resultCode == SWEC_RESULT_INCOMPLETE)
    {
        printf ("Authentication incomplete\n");
		return AUTH_ERR_OTHER;
    }
    else if (authenRec.resultCode == SWEC_RESULT_PASSED_CHECK ||
             authenRec.resultCode == SWEC_RESULT_PASS_BAD_PIN ||
             authenRec.resultCode == SWEC_RESULT_PASS_DURESS  ||
             authenRec.resultCode == SWEC_RESULT_PASS_NEED_NEW)
    {
        printf ("Authentication complete, user passed -- ");
        switch (authenRec.resultCode)
        {
            case SWEC_RESULT_PASSED_CHECK:
                printf ("No problems\n");
                break;
            case SWEC_RESULT_PASS_BAD_PIN:
                printf ("Bad personal ID number\n");
                break;
            case SWEC_RESULT_PASS_DURESS:
                printf ("Duress personal ID number used\n");
                break;
            case SWEC_RESULT_PASS_NEED_NEW:
                printf ("New password needed\n");
                break;
        }
		return AUTH_SUCCESS;
    }
    else
    {
        printf ("Authentication complete, user failed -- ");
        switch (authenRec.resultCode)
        {
            case SWEC_RESULT_BAD_ID:
                printf ("Unknown user ID\n");
				return AUTH_USER_NOTEXIST;
                break;
            case SWEC_RESULT_FAILED_AUTHEN:
                printf ("Failed authentication\n");
				return AUTH_PWD_MISMATCH;
                break;
            case SWEC_RESULT_QUOTA_LOCK_ON:
                printf ("Quota exhausted\n");
                break;
            case SWEC_RESULT_TIME_LOCK_ON:
                printf ("Wrong time of day\n");
                break;
            case SWEC_RESULT_DATE_LOCK_ON:
                printf ("Wrong date\n");
                break;
            case SWEC_RESULT_WEEK_LOCK_ON:
                printf ("Wrong day of the week\n");
                break;
            case SWEC_RESULT_ATTACK_LOCK_ON:
                printf ("Attack lockout in effect\n");
                break;
            case SWEC_RESULT_CLOCK_LOCK_ON:
                printf ("System clock inconsistancy\n");
                break;
            case SWEC_RESULT_TAMPER_LOCK_ON:
                printf ("Tamper lockout in effect\n");
                break;
            case SWEC_RESULT_RECORD_LOCKED:
                printf ("User record temporarily locked\n");
                break;
            case SWEC_RESULT_BAD_NEW_FIXED:
                printf ("Invalid new password provided\n");
                break;
            case SWEC_RESULT_SERVER_FAILURE:
                printf ("Server failed during authentication\n");
                break;
            case SWEC_RESULT_NO_SERVICE:
                printf ("No service allowed, the authentication context has disqualified the user\n");
                break;
            case SWEC_RESULT_NO_AUTHEN:
                printf ("User has no authenticators\n");
                break;
            case SWEC_RESULT_WEAK_AUTHEN:
                printf ("User's authenticator strength is too weak\n");
                break;
            default:
                printf ("Unknown resultCode %d\n", authenRec.resultCode);
                break;
        }
		return AUTH_ERR_OTHER;			
    }


    /*
    ** The statusText field usually echos the return code.
    */
    
	/*
    printf("Status text: %s\n", authenRec.statusText ?
        authenRec.statusText : "<none>");
*/

    /*
    ** The user's action data is stored in his record in the SafeWord
    ** user database.  It can be a shell command to execute, or the name
    ** of an authorization group, or whatever the application needs.  It
    ** can vary depending on the type of application, the machine it is
    ** running on (or the peer machine it is connected to), etc.  See the
    ** SafeWord Administration Guide for more details.
    */
    /*
    if (authenRec.actionData)
    {
    	printf ("Action data: %s\n", (*authenRec.actionData == 0xFF) ?
    	                             "<unreadable>" : authenRec.actionData);
    }
    else
    {
    	printf ("Action data: <none>\n");
    }
	*/

}


/*******************************************************************************
**
**  void dialogCallbackFunction (SwecHdl dialogContext,
**                        SwecDialogRec *dialogPtr)
**                                 - service authentication information requests
**
**  This function is called by SWEC API whenever it needs to gather or provide
**  information as part of the authentication process.  It serves as a conduit
**  for communication between the authentication server and the user (or
**  application).
**
**  dialogContext is a pointer provided by the field of the same name in
**  the SwecAuthenRec record provided to swecAuthen.  It can point to any
**  data struction the application requires, its structure is not used by
**  SWEC API.
**
**  dialogPtr is a pointer to a record that specifies what information
**  the callback function needs to display/input.
**
*******************************************************************************/

#define BUFFLEN 256

void dialogCallbackFunction (SwecHdl dialogContext, SwecDialogRec * dialogPtr)
//SwecHdl        dialogContext;
//SwecDialogRec *dialogPtr;
{
    char line[BUFFLEN];
    
    /*
    ** This is a message text to be displayed (if not NULL).
    */
    if (dialogPtr->dialogMessage && *dialogPtr->dialogMessage)
        printf("%s\n", dialogPtr->dialogMessage);


    /*
    ** The dialogType field tells us what function to perform: Either
    ** input a string (with or without echoing it) or outputting a string.
    */
    
    switch (dialogPtr->dialogType)
    {
        case SWEC_DIALOG_INPUT_NO_ECHO:
            /*
            ** This *should* disable the echo when inputting information,
            ** but that is a highly platform-dependent procedure, so we're
            ** just going to fake it for purposes of demonstration.
            */
            printf ("<echo disabled>\n");

        case SWEC_DIALOG_INPUT_ECHO:
            printf (dialogPtr->dialogInputPrompt);   /* Print the prompt */
            if (fgets (line, BUFFLEN, stdin) == NULL)
            {
                dialogPtr->abortFlag = SWEC_TRUE;  /* EOF - abort authentication */
                return;
            }
            line[strlen (line) - 1] = '\0'; /* Remove newline */
            strncpy (dialogPtr->inputBuffer, line, dialogPtr->inputBufferLength);
            break;
            
        case SWEC_DIALOG_INFO:
            /* we displayed any messages above */
            break;
            
        default:
            printf ("Unknown dialog type %d in dialogCallbackFunction\n",
                dialogPtr->dialogType);
            break;
    }
}


/*******************************************************************************
**
**  void printSwecError (int rc, char *func)
**
**  Prints an interpretation of a SWEC API return code.
**
*******************************************************************************/

void printSwecError (int rc, char * func)
//int rc;
//char *func;
{
    printf ("SWEC error in %s: ", func);
    if (rc <= 27017 && rc >= 27054)
    {
        printf("Network error occured\n");
        return;
    }
    switch(rc)
    {
    case SWEC_STATUS_TIME_OUT:
            printf("Time out trying to connect to server\n");
            break;
    case SWEC_STATUS_NO_SERVER:
            printf("Server not found\n");
            break;
    case SWEC_STATUS_ALLOC_ERR:
            printf("Unable to allocate memory\n");
            break;
    case SWEC_STATUS_INTERNAL_ERR:
            printf("Internal error in SWEC module\n");
            break;
    case SWEC_STATUS_FILE_ERR:
            printf("Failure while reading status file\n");
            break;
    case SWEC_STATUS_INVALID_CONFIG_DATA:
            printf("Invalid data in configuration file\n");
            break;
    case SWEC_STATUS_NOT_REGISTERED:
            printf("Attempt to deregister unregistered app\n");
            break;
    case SWEC_STATUS_NOT_INITIALIZED:
            printf ("SWEC module not initialized\n");
            break;
    case SWEC_STATUS_NO_CALLBACK:
            printf("Missing dialog callback routine\n");
            break;
    case SWEC_STATUS_SERVER_BUSY:
            printf("Server busy, all server connections in use\n");
            break;
    case SWEC_STATUS_CONNECT_FAIL:
            printf("Connection to server has failed\n");
            break;
    case SWEC_STATUS_INVALID_CONNECT_HDL:
            printf("Invalid connect handle\n");
            break;
    case SWEC_STATUS_USER_ABORT:
            printf("User aborted callback function\n");
            break;
    case SWEC_STATUS_APP_REGISTERED:
            printf("An application is still registered to the SWEC module\n");
            break;
    case SWEC_STATUS_INVALID_CONFIG_SOURCE:
            printf("Invalid configuration source");
            break;
    case SWEC_STATUS_REGISTRY_ERR:
            printf("Error accessing Windows registry");
            break;
    case SWEC_STATUS_EVENT_LOG_ERR:
            printf("Error posting status message to Windows event log\n");
            break;
    case SWEC_STATUS_INVALID_PARAM:
            printf("Invalid function parameter\n");
            break;
    case SWEC_STATUS_SERVER_UNAVAILABLE:
            printf("The server has become unavailable\n");
            break;
    case SWEC_STATUS_SSL_ERROR:
            printf("Failed to establish an SSL connection with the server\n");
            break;
    case SWEC_STATUS_INVALID_RESPONSE:
            printf("The message from the server has a bad message format\n");
            break;
    case SWEC_STATUS_INVALID_SSL_CERT :
            printf("The server certificate has changed\n");
            break;
    case SWEC_STATUS_LIBRARY_LOAD_ERR:
            printf("Error trying to load required library\n");
            break;
    case SWEC_STATUS_LOAD_SSL_ERR:
            printf("Error trying to load SSL library\n");
            break;
    case SWEC_STATUS_LOAD_CRYPTO_ERR:
            printf("Error trying to load CRYPTO library\n");
            break;
    default:
            printf ("Unknown return code %d\n", rc);
            break;
    }
}
