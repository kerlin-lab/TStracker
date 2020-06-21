#ifndef  _THREAD_SAFE_VARIABLE_H_
#define  _THREAD_SAFE_VARIABLE_H_

#include <afxwin.h>

template <typename T>
class ThreadSafeVariable
{
public:
	HANDLE mtx;
	T var;
public:
	ThreadSafeVariable(T initalValue);
	T read();
	void write(T newValue);
};
#endif // ! _THREAD_SAFE_VARIABLE_H_


