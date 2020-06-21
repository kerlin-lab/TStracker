#ifndef _THREAD_SAFE_QUEUE_H_
#define _THREAD_SAFE_QUEUE_H_

#include "afxwin.h"

#include <queue>

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

	void enqueue(T item);	// Enqueue the item to the queue

	void pop();				// Dequeue the the item at front of the queue

	T& dequeue();			// Dequeue the object at the front of the queue and return it

	T& front();				// The object at the front of the queue

	size_type size() const;
};
#endif // !_THREAD_SAFE_QUEUE_H_
