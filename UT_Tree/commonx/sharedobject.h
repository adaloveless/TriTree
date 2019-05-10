
#ifndef _SHARED_OBJECT_H_
#define _SHARED_OBJECT_H_

#include"typex.h"
#include <Windows.h>

typedef _RTL_CRITICAL_SECTION CriticalSection;



class SharedObject
{
protected:
	CriticalSection _cs;
public:
	SharedObject(void) { InitializeCriticalSection(&_cs); }
	~SharedObject(void) { DeleteCriticalSection(&_cs); }
	void lock(void) { EnterCriticalSection(&_cs); };
	void unlock(void) { LeaveCriticalSection(&_cs); };
	friend class scoped_lock;
};

class scoped_lock
{
	CriticalSection &_mtx;
public:

	scoped_lock(CriticalSection &mtx) : _mtx(mtx)
	{
		EnterCriticalSection(&_mtx);
	}

	scoped_lock(SharedObject* so) : _mtx(so->_cs)
	{
		EnterCriticalSection(&_mtx);
	}

	~scoped_lock()
	{
		LeaveCriticalSection(&_mtx);
	}
};

#endif


