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

template<typename T>
ThreadSafeVariable<T>::ThreadSafeVariable(T initalValue)
{
	mtx = CreateMutex(NULL, FALSE, NULL);
	var = initalValue;
}

template<typename T>
T ThreadSafeVariable<T>::read()
{
	return var;
}

template<typename T>
void ThreadSafeVariable<T>::write(T newValue)
{
	WaitForSingleObject(mtx,INFINITE);
	var = newValue;
	ReleaseMutex(mtx);
}

#endif // ! _THREAD_SAFE_VARIABLE_H_


