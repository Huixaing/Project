#ifndef SWTHREAD_INCLUDE
#define SWTHREAD_INCLUDE

/* swthread.h */

/* Values that may be passed to the
 * flags arg for swThreadInit()
 */
#define SWT_DEBUG_FLAG 1

#define SWTHREAD_NONE  -1
#define MUTEX_LOCK_ID_SIZE      32

/* Data types used in and returned by the
 * functions in this library.
 */ 
typedef unsigned long  SWTLong;
typedef unsigned char  SWTByte;
typedef void * swEvent;
#if WIN32
    /* WINDOWS HEADERS */
    #include <process.h>
    #include <wtypes.h>

    /* WINDOWS TYPEDEFS */
    typedef DWORD   SWTWord;
    typedef LPVOID  SWTVoidPtr;
    typedef HANDLE  SWTThread;
    typedef HANDLE  SWTMutex;
    typedef SWTWord SWTCond;

    /* WINDOWS CONSTANTS */
    #define SWTHREAD_INFINITE INFINITE
    #define SWTGetLastError GetLastError()
#else
    /* UNIX HEADERS */
    #include <errno.h>

    /* UNIX TYPEDEFS */
    typedef int   SWTWord;
    typedef void* SWTVoidPtr;
    
    #ifndef UX_SOLARIS
        /* POSIX HEADERS & TYPEDEFS */
        #include <pthread.h>
        typedef pthread_t        SWTThread;
        typedef pthread_mutex_t* SWTMutex;
        typedef pthread_key_t    SWTKey;
        typedef pthread_cond_t   SWTCond;
    #else
        /* SOLARIS HEADERS & TYPEDEFS */
        #include <thread.h>
        typedef mutex_t*         SWTMutex;
        typedef thread_t         SWTThread;
        typedef cond_t           SWTCond;
        typedef thread_key_t     SWTKey;
    #endif
    
    /* UNIX CONSTANTS */
    #define SWTHREAD_INFINITE 0
    #define SWTGetLastError errno

	#define WINAPI
#endif

#if !THREADS
    /* NON-THREADED APPLICATIONS
     * Place dummy prototypes for
     * non-threaded applications here
     */

    #define swThreadDoNothing             abs
    #define swThreadKill(a)               swThreadDoNothing(0)
    #define swThreadLock(string)          swThreadDoNothing(0)
    #define swThreadUnlockOpen(string)    swThreadDoNothing(0)
    #define swThreadUnlockClose(string)   swThreadDoNothing(0)
    #define swThreadInit(a,b,c)           swThreadDoNothing(0)
    #define swThreadUninit()              swThreadDoNothing(0)
    #define swThreadCreate(a,b,c,d)       a(b)
    #define swThreadWait(a,b,c,d)         swThreadDoNothing(0)

    /* swThreadIdGet() should 
     * return a 1 since even 
     * non-threaded apps have 1 thread
     */
    #define swThreadIdGet()               swThreadDoNothing(1)
    #define swThreadEnd(a,b)              swThreadDoNothing(0)
    #define swThreadYield()               swThreadDoNothing(0)

#else
    /* THREADED APPLICATIONS
     * Place prototypes for
     * threaded applications here
     */
    
    extern SWTWord WINAPI swThreadInit            (SWTWord numThreads,
                                                    SWTLong *flagFunc,
                                                    SWTLong flags);
    extern SWTWord WINAPI swThreadLock            (SWTByte *mutexId);
    extern SWTWord WINAPI swThreadUnlockOpen      (SWTByte *mutexId);
    extern SWTWord WINAPI swThreadUnlockClose     (SWTByte *mutexId);
    extern SWTWord WINAPI swThreadUninit          ();
    extern SWTWord WINAPI swThreadWait            (SWTLong  timeout,
                                                      SWTThread *threadObj,
                                                      SWTByte *mutexId,
                                                      SWTLong *predicate);
    extern SWTWord WINAPI swThreadCreate          (SWTWord threadFunc(),
                                                      SWTVoidPtr threadData,
                                                      SWTWord    stackSize,
                                                      SWTThread *threadObj);
    extern SWTWord WINAPI swThreadYield           ();
    extern void    WINAPI swThreadKill            (SWTThread *threadObj); 
    extern void    WINAPI swThreadEnd             (SWTByte *mutexId,
                                                     SWTLong *predicate);
    extern SWTMutex WINAPI swThreadMutexGet       (SWTByte *mutexId);
    extern SWTLong WINAPI swThreadIdGet           ();
    extern SWTMutex WINAPI swThreadMutexCreate     (SWTWord *rc);
    extern SWTWord WINAPI swThreadMutexDestroy    (SWTMutex lock);
    extern SWTWord WINAPI swThreadMutexLock       (SWTMutex lock, 
                                                     SWTLong lockFlag);
    extern SWTWord WINAPI swThreadMutexUnlock     (SWTMutex lock);
    extern swEvent WINAPI swThreadEventCreate     (SWTWord *rc,
                                                     SWTWord bManualReset,
                                                     SWTWord bInitialState);
    extern SWTWord WINAPI swThreadEventDestroy    (swEvent e);
    extern SWTWord WINAPI swThreadEventSet        (swEvent e);
    extern SWTWord WINAPI swThreadEventReset      (swEvent e);
    extern SWTWord WINAPI swThreadEventWait       (swEvent e,
                                                 SWTLong timeout); 
    extern SWTWord WINAPI swThreadCond            (SWTCond *cond,
                                                     SWTWord condFlag);
#endif /* !THREADS */

#endif /* SWTHREAD_INCLUDE */
