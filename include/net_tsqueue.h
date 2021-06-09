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

	ThreadSafeQueue(const ThreadSafeQueue&) = delete;
	ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

public:
	[[nodiscard]] const T& front()
	{
		std::scoped_lock lock(deque_mux);
		return deque.front();
	}

	[[nodiscard]] const T& back()
	{
		std::scoped_lock lock(deque_mux);
		return deque.back();
	}

	void push_front(const T& item)
	{
		std::scoped_lock lock(deque_mux);
		deque.push_front(item);
		std::unique_lock<std::mutex> blocker_lock(blocker_mux);
		blocker.notify_one();
	}

	void push_front(T&& item)
	{
		std::scoped_lock lock(deque_mux);
		deque.push_front(std::move(item));
		std::unique_lock<std::mutex> blocker_lock(blocker_mux);
		blocker.notify_one();
	}

	void push_back(const T& item)
	{
		std::scoped_lock lock(deque_mux);
		deque.push_back(item);
		std::unique_lock<std::mutex> blocker_lock(blocker_mux);
		blocker.notify_one();
	}

	void push_back(T&& item)
	{
		std::scoped_lock lock(deque_mux);
		deque.push_back(std::move(item));
		std::unique_lock<std::mutex> blocker_lock(blocker_mux);
		blocker.notify_one();
	}

	[[nodiscard]] size_t size()
	{
		std::scoped_lock lock(deque_mux);
		return deque.size();
	}

	[[nodiscard]] bool empty()
	{
		return size() == 0;
	}

	void clear()
	{
		std::scoped_lock lock(deque_mux);
		deque.clear();
	}

	T pop_front()
	{
		std::scoped_lock lock(deque_mux);
		auto t = std::move(deque.front());
		deque.pop_front();
		return t;
	}

	T pop_back()
	{
		std::scoped_lock lock(deque_mux);
		auto t = std::move(deque.back());
		deque.pop_back();
		return t;
	}

	void wait()
	{
		while (empty()) {
			std::unique_lock<std::mutex> blocker_lock(blocker_mux);
			blocker.wait(blocker_lock);
		}
	}

protected:
	std::mutex deque_mux{};
	std::deque<T> deque{};

	std::mutex blocker_mux{};
	std::condition_variable blocker{};
};
} // namespace net

#endif // NET_TSQUEUE_H