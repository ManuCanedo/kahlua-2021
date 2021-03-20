#ifndef NET_TSQUEUE_H
#define NET_TSQUEUE_H

#include "net_common.h"

namespace net
{
template<typename T>
class tsqueue
{
public:
	tsqueue() = default;
	tsqueue(const tsqueue&) = delete;
	tsqueue(tsqueue&&) = default;
	~tsqueue() { clear(); }

public:
	const T& front() const
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.front();
	}

	const T& back()
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.back();
	}

	void push_front(const T& item)
	{
		std::scoped_lock lock(muxQueue);
		deqQueue.push_front(item);
	}

	void push_front(T&& item) noexcept
	{
		std::scoped_lock lock(muxQueue);
		deqQueue.push_front(std::move(item));
	}

	void push_back(const T& item)
	{
		std::scoped_lock lock(muxQueue);
		deqQueue.push_back(item);
	}

	void push_back(T&& item) noexcept
	{
		std::scoped_lock lock(muxQueue);
		deqQueue.push_back(std::move(item));
	}

	size_t size()
	{	
		std::scoped_lock lock(muxQueue);
		return deqQueue.size();
	}

	void clear()
	{	
		std::scoped_lock lock(muxQueue);
		deqQueue.clear();
	}

	T pop_front()
	{
		std::scoped_lock lock(muxQueue);
		auto t = std::move(deqQueue.front());
		deqQueue.pop_front();
		return t;
	}

	T pop_back()
	{
		std::scoped_lock lock(muxQueue);
		auto t = std::move(deqQueue.back());
		deqQueue.pop_back();
		return t;
	}

protected:
	std::mutex muxQueue;
	std::deque<T> deqQueue;
};
}

#endif // NET_TSQUEUE_H