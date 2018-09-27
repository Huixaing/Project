#ifndef SCCERRNO_INCLUDE
#define SCCERRNO_INCLUDE

/************************************************************
 ** Generic result codes.  They should all start with
 ** a prefix of "SCC_GEN_". 
 **
 ************************************************************/

    /* Generic result code for a successful operation.
     * This should always be 0 to maintain backward
     * compatibility with the old SafeWord SUCCESS 
     * constant.
     */
    #define SCC_GEN_SUCCESS            0

    /* Generic result code for a failed operation.
     * This should always be 2000 to maintain backward
     * compatibility with the old SafeWord NO_SUCCESS
     * constant.
     */
    #define SCC_GEN_FAIL               2000

/************************************************************ 
 ** Result codes specific to threading and mutex operations
 ** They should all start with a prefix of "SCC_THD_".
 ************************************************************/

    #define SCC_THD_NOT_THREADED_APP       5001
    #define SCC_THD_ALREADY_INITIALIZED    5002
    #define SCC_THD_NOT_INITIALIZED        5003
    #define SCC_THD_ALREADY_OPEN           5004
    #define SCC_THD_ALREADY_CLOSED         5005
    #define SCC_THD_NO_EXIST               5006
    #define SCC_THD_FUNCTION_NOT_SUPPORTED 5007
    #define SCC_THD_CANT_ALLOC_MUTEX       5008
    #define SCC_THD_CANT_CREATE_MUTEX      5009
    
    /* This code is returned by functions such as
     * swThreadWait() when the targeted thread does
     * not terminate before the timeout dictated in
     * swThreadWait()'s timeout argument.  It is
     * mapped to an OS specific return code for backward
     * compatability with SafeWord 5.X
     */
    #if WIN32
        #define SCC_THD_TIMED_OUT          WAIT_TIMEOUT
    #else
        #define SCC_THD_TIMED_OUT          ETIMEDOUT
    #endif

    /* For the Windows implementation it is
     * mapped to an OS specific return code for backward
     * compatability with SafeWord 5.X
     */
    #if WIN32
        #define SCC_THD_DEAD               ERROR_INVALID_HANDLE
    #else
        #define SCC_THD_DEAD               5010
    #endif

    /* For the Windows implementation it is
     * mapped to an OS specific return code for backward
     * compatability with SafeWord 5.X
     */
    #if WIN32
        #define SCC_THD_WAIT_FAILED        WAIT_FAILED
    #else
        #define SCC_THD_WAIT_FAILED        5011
    #endif

    #define SCC_THD_CANT_CREATE_EVENT      5012

/************************************************************
 ** Values for flags that may be used across an application.
 ** They should all start with a prefix of "SCC_GFLAG_".
 ** Note that these values are not the values of the flags
 ** themselves, but the bitwise location within an unsigned
 ** long
 ************************************************************/

    /* If the bit in this location is set, then the application
     * was instructed to shutdown.  All loops, whiles, etc testing
     * for this flag should break
     */
    #define SCC_GFLAG_SHUTDOWN         0

    /* If the bit in this location is set, then the application
     * may report debugging information at its discression
     */
    #define SCC_GFLAG_DEBUG            1

#endif /* SCCERRNO_INCLUDE */
