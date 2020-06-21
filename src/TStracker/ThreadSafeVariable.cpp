#include "ThreadSafeVariable.h"

template<typename T>
inline ThreadSafeVariable<T>::ThreadSafeVariable(T initalValue)
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
	WaitForSingleObject(mtx);
	var = newValue;
	ReleaseMutex(mtx);
}


