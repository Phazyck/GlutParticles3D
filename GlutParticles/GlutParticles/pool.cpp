#include "pool.h"

using namespace std;

template <class T>
Pool<T>::Pool()
{
	liveCount = 0;
	objects = vector<T>();
}

template <class T>
T Pool<T>::new_object()
{
		
	int s = objects.size();

	if (liveCount < s)
	{
		return objects[liveCount++];
	}

	T obj = new T();

	objects.push_back(obj);

	liveCount++;

	return obj;

}

template <class T>
void Pool<T>::mark_dead(int index)
{
	T tmp = objects[index];
	objects[index] = objects[--liveCount];
	objects[liveCount] = tmp;
}

template <class T>
T Pool<T>::at(int index)
{
	return objects[index];
}

template <class T>
int Pool<T>::count()
{
	return liveCount;
}