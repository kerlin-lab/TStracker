#ifndef _THREAD_SAFE_QUEUE_H_
#define _THREAD_SAFE_QUEUE_H_

#include <queue>
#include "synchapi.h"

template <typename T>
class ThreadSafeQueue
{
public:
	queue<T> * Queue;		// The queue
	HANDLE mtx;				// The handle to help this queue thread-safe
public:
	ThreadSafeQueue();

	~ThreadSafeQueue();

	void enqueue(T item);	// Enqueue the item to the queue

	void pop();				// Dequeue the the item at front of the queue

	T& dequeue();			// Dequeue the object at the front of the queue and return it

	T& front();				// The object at the front of the queue

};
#endif // !_THREAD_SAFE_QUEUE_H_
