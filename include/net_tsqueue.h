#ifndef NET_TSQUEUE_H
#define NET_TSQUEUE_H

#include "net_common.h"

namespace net
{
template <typename T> class ThreadSafeQueue {
public:
	ThreadSafeQueue() = default;

	~ThreadSafeQueue()
	{
		clear();
	}

	ThreadSafeQueue(ThreadSafeQueue&&) = default;
	ThreadSafeQueue& operator=(ThreadSafeQueue&&) = default;

	ThreadSafeQueue(const ThreadSafeQueue&) = delete;
	ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

public:
	[[nodiscard]] const T& front() const
	{
		std::scoped_lock lock(mux);
		return deque.front();
	}

	[[nodiscard]] const T& back()
	{
		std::scoped_lock lock(mux);
		return deque.back();
	}

	void push_front(const T& item)
	{
		std::scoped_lock lock(mux);
		deque.push_front(item);
	}

	void push_front(T&& item) noexcept
	{
		std::scoped_lock lock(mux);
		deque.push_front(std::move(item));
	}

	void push_back(const T& item)
	{
		std::scoped_lock lock(mux);
		deque.push_back(item);
	}

	void push_back(T&& item) noexcept
	{
		std::scoped_lock lock(mux);
		deque.push_back(std::move(item));
	}

	[[nodiscard]] size_t size()
	{
		std::scoped_lock lock(mux);
		return deque.size();
	}

	void clear()
	{
		std::scoped_lock lock(mux);
		deque.clear();
	}

	T pop_front()
	{
		std::scoped_lock lock(mux);
		auto t = std::move(deque.front());
		deque.pop_front();
		return t;
	}

	T pop_back()
	{
		std::scoped_lock lock(mux);
		auto t = std::move(deque.back());
		deque.pop_back();
		return t;
	}

protected:
	std::mutex mux{};
	std::deque<T> deque{};
};
} // namespace net

#endif // NET_TSQUEUE_H