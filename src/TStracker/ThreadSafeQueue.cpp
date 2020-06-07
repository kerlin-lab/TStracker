#include "ThreadSafeQueue.h"

template <typename T>
ThreadSafeQueue<T>::ThreadSafeQueue()
{
	this->Queue = new queue<T>();
	this->mtx = CreateMutex(NULL, FALSE, NULL);
}

template <typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue()
{
	delete this->Queue;		// Delete the queue
	CloseHandle(this->mtx);	// Delete the handle
}

template<typename T>
void ThreadSafeQueue<T>::enqueue(T item)
{
	WaitForSingleObject(this->mtx);
	this->Queue->push(item);
	ReleaseMutex(this->mtx);
}

template<typename T>
void ThreadSafeQueue<T>::pop()
{
	WaitForSingleObject(this->mtx);
	this->Queue->pop();
	ReleaseMutex(this->mtx);
}

template<typename T>
T& ThreadSafeQueue<T>::dequeue()
{
	T * item = &this->Queue->front();
	return *item;
}

template<typename T>
T& ThreadSafeQueue<T>::front()
{
	return this->Queue->front();
}
