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
	~ThreadSafeVariable();
	T read();
	void write(T newValue);
	void waitTillZero(DWORD timeBeforeCheck=100);
};

template<typename T>
ThreadSafeVariable<T>::ThreadSafeVariable(T initalValue)
{
	mtx = CreateMutex(NULL, FALSE, NULL);
	var = initalValue;
}

template<typename T>
inline ThreadSafeVariable<T>::~ThreadSafeVariable()
{
	CloseHandle(this->mtx);
}

template<typename T>
T ThreadSafeVariable<T>::read()
{
	WaitForSingleObject(this->mtx, INFINITE);
	T copy = this->var;
	ReleaseMutex(this->mtx);
	return copy;
}

template<typename T>
void ThreadSafeVariable<T>::write(T newValue)
{
	WaitForSingleObject(mtx,INFINITE);
	var = newValue;
	ReleaseMutex(mtx);
}

template<typename T>
inline void ThreadSafeVariable<T>::waitTillZero(DWORD timeBeforeCheck)
{
	// This function wait until the variable reaches zero or false or ...
	T tmp;
	do
	{
		Sleep(timeBeforeCheck);
		WaitForSingleObject(this->mtx,INFINITE);
		tmp = this->var;
		ReleaseMutex(this->mtx);
	} while (tmp);
}



#endif // ! _THREAD_SAFE_VARIABLE_H_


