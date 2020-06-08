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
	WaitForSingleObject(this->mtx);
	T * item = &this->Queue->front();
	this->Queue->pop();
	ReleaseMutex(this->mtx);
	return *item;
}

template<typename T>
T& ThreadSafeQueue<T>::front()
{
	WaitForSingleObject(this->mtx);
	T * item = &this->Queue->front();
	ReleaseMutex(this->mtx);
	return *item;
}

template<typename T>
size_type ThreadSafeQueue<T>::size() const
{
	WaitForSingleObject(this->mtx);
	size_type size = this->Queue->size();
	ReleaseMutex(this->mtx);
	return size;
}
