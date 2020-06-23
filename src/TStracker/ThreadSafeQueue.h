#ifndef _THREAD_SAFE_QUEUE_H_
#define _THREAD_SAFE_QUEUE_H_

#include "afxwin.h"

#include <queue>
#include <stack>

using namespace std;

template <typename T>
class ThreadSafeQueue
{
public:
	queue<T> * Queue;		// The queue
	HANDLE mtx;				// The handle to help this queue thread-safe
public:
	ThreadSafeQueue();

	~ThreadSafeQueue();

	void enqueue(T& item);	// Enqueue the item to the queue

	void pop();				// Dequeue the the item at front of the queue

	T& dequeue();			// Dequeue the object at the front of the queue and return it

	T& front();				// The object at the front of the queue

	bool erase(T& item, bool all = true);		// Remove the elements in the queue matches item, return true if deletion happened
											// if all = true, remove all instance, otherwise, remove the one closest to the back of the queue
	unsigned size() const;
};

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
void ThreadSafeQueue<T>::enqueue(T& item)
{
	WaitForSingleObject(this->mtx, INFINITE);
	this->Queue->push(item);
	ReleaseMutex(this->mtx);
}

template<typename T>
void ThreadSafeQueue<T>::pop()
{
	WaitForSingleObject(this->mtx, INFINITE);
	if (this->Queue->size())
	{
		this->Queue->pop();
	}
	ReleaseMutex(this->mtx);
}

template<typename T>
T& ThreadSafeQueue<T>::dequeue()
{
	T * item;
	WaitForSingleObject(this->mtx, INFINITE);
	if (this->Queue->size())
	{
		item = &(this->Queue->front());
		this->Queue->pop();
	}
	ReleaseMutex(this->mtx);
	return *item;
}

template<typename T>
T& ThreadSafeQueue<T>::front()
{
	WaitForSingleObject(this->mtx,INFINITE);
	T * item = &this->Queue->front();
	ReleaseMutex(this->mtx);
	return *item;
}

template<typename T>
unsigned ThreadSafeQueue<T>::size() const
{
	WaitForSingleObject(this->mtx, INFINITE);
	unsigned size = this->Queue->size();
	ReleaseMutex(this->mtx);
	return size;
}

// Remove the elements in the queue matches item, return true if deletion happened
// if all = true, remove all instance, otherwise, remove the one closest to the back of the queue
template<typename T>
bool ThreadSafeQueue<T>::erase(T& item, bool all)
{
	queue<T> tmp;
	bool result = false;
	WaitForSingleObject(this->mtx, INFINITE);
	while (this->Queue->size())
	{
		if (this->Queue->front() == item)
		{
			this->Queue->pop();
			result = true;
			if (!all)
			{
				// Transer the rest to tmp queue
				while (this->Queue->size())
				{
					tmp.push(this->Queue->front());
					this->Queue->pop();
				}
				break;
			}
		}
		else
		{
			tmp.push(this->Queue->front());
			this->Queue->pop();
		}
	}
	// Put things in the queue front to list
	while (tmp.size())
	{
		this->Queue->push(tmp.front());
		tmp.pop();
	}
	ReleaseMutex(this->mtx);
	return result;
}
#endif // !_THREAD_SAFE_QUEUE_H_
