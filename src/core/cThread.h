#ifndef __CTHREAD_H__
#define __CTHREAD_H__

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

template< class T >
class cThread
{
	// new type method: pointer to a object's method
	typedef DWORD ( T::* Method )( void );
	//
protected:
	HANDLE	hThread;		// unique handle to the thread
private:
	DWORD	threadID;		// thread id - 0 until started
	T*		object;			// the object which owns 
	Method	method;			// the method of the object
	HANDLE	hInterrupt;		// mutex to signal an interrupt via ReleaseSemaphore()
	HANDLE	hSingleStart;	// only one thread allowed to call start() mutex
	//
private:
	// This function gets executed by a concurrent thread
	static DWORD run( LPVOID thread_obj )
	{
		cThread< T >* thread = ( cThread< T >* )thread_obj;
		return( thread->object->*thread->method )();
	}
	// Prevent copying of threads: No sensible implementation!
	cThread( const cThread< T >& other ) {}
	cThread< T >& operator = ( const cThread< T >& other ) {}
	//
public:
	/*
	Creates a new cThread object. object: the one which method should be executed.
	method: pointer to the object's method
	*/
	explicit cThread( T* object, DWORD( T::* method )( void ) )
	{
		this->hThread = NULL;
		this->object = object;
		this->method = method;
		this->threadID = 0;
		this->hInterrupt = CreateSemaphore( NULL, 1, 1, NULL );
		this->hSingleStart = CreateMutex( NULL, FALSE, NULL );
	}

	cThread( shared_ptr< T > object, DWORD( T::* method )( void ) )
	{
		this->hThread = NULL;
		this->object = object.get();
		this->method = method;
		this->threadID = 0;
		this->hInterrupt = CreateSemaphore( NULL, 1, 1, NULL );
		this->hSingleStart = CreateMutex( NULL, FALSE, NULL );
	}
	//
	~cThread()
	{
		if( hInterrupt )
			CloseHandle( hInterrupt );
		if( hThread )
			CloseHandle( hThread );
	}
	//
	/*
	Starts executing the objects method in a concurrent thread. True if the thread was started successfully; otherwise false.
	*/
	bool Init()
	{
		__try{
			if( WaitForSingleObject( hSingleStart, 0 ) != WAIT_OBJECT_0 )
				return false;
			if( hThread )	// Thread had been started sometime in the past
			{
				if( WaitForSingleObject( hThread, 0 ) == WAIT_TIMEOUT )
				{
					// if thread's still running deny new start
					return false;
				}
				CloseHandle( hThread );
			}
			// (Re-)Set not interrupted semaphore state
			WaitForSingleObject( hInterrupt, 0 );

			hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) cThread< T >::run, this, 0, &this->threadID );

			if( hThread )
				return true;
			return false;
		}
		__finally
		{
			ReleaseMutex( hSingleStart );
		}
	}
	//
	// Blocks the calling thread until this thread has stopped.
	inline void Join()
	{
		WaitForSingleObject( hThread, INFINITE );
	}
	//
	/* Asks the thread to exit nicely. Thread function must implement checks.
	return value indicators if the interrupt could be placed not if the thread
	reacts on the interrupt. true indicates success, false an error.
	*/
	inline bool Interrupt()
	{
		if( hInterrupt )
		{
			return( ( ReleaseSemaphore( hInterrupt, 1, NULL ) == FALSE ) ? false : true );
		}
		return false;
	}
	//
	/* True if an interrupt request was set, otherwise false. */
	inline bool IsInterrupt()
	{
		return this->IsInterrupt(0);
	}
	//
	/* True if an interrupt request was set, otherwise false. Waits for millisec 
	milliseconds for the interrupt to take place.*/
	inline bool IsInterrupted( DWORD millisec )
	{
		if( WaitForSingleObject( hInterrupt, millisec ) == WAIT_TIMEOUT )
		{
			return false;
		}
		ReleaseSemaphore( hInterrupt, 1, NULL );	// keep interrupted state
		return true;
	}
	//
	inline bool IsRunning()
	{
		DWORD exitCode = 0;
		if( hThread )
			GetExitCodeThread( hThread, &exitCode );
		if( exitCode == STILL_ACTIVE )
			return true;
		return false;
	}
	//
	__declspec(property(get = getThreadHandle)) HANDLE ThreadHandle;
	inline HANDLE getThreadHandle()
	{
		return hThread;
	}
	//
	__declspec(property(get = getThreadID)) DWORD ThreadID;
    inline DWORD getThreadID()
    {
        return threadID;
    }

};

#endif
