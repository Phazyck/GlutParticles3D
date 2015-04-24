#ifndef POOL
#define POOL

#include <vector>

using namespace std;

template <class T>

class Pool
{

private:

	int liveCount;
	vector<T> objects;
	 
public:

	Pool();

	T new_object();
	void mark_dead(int index);
	T at(int index);
	int count();

};

#endif