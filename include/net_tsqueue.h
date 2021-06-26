#ifndef NET_TSQUEUE_H
#define NET_TSQUEUE_H

#include "net_common.h"

namespace net
{
constexpr unsigned MAX_SIZE = 128;

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

	// perfect forwarding for the emplace methods
	template<typename ... Args>
	void emplace_front(Args&& ... args)
	{
		std::scoped_lock lock(deque_mux);
		deque.emplace_front(std::forward<Args>(args)...);
		wake();
	}

	template<typename ... Args>
	void emplace_back(Args&& ... args)
	{
		std::scoped_lock lock(deque_mux);
		deque.emplace_back(std::forward<Args>(args)...);
		wake();
	}

	template<typename U>
	void push_back(U&& item)
	{
		std::scoped_lock lock(deque_mux);
		if (deque.size() < MAX_SIZE) {
			deque.push_back(std::forward<U>(item));
		}
		wake();
	}

	template<typename U>
	void push_front(U&& item)
	{
		std::scoped_lock lock(deque_mux);
		if (deque.size() < MAX_SIZE) {
			deque.push_front(std::forward<U>(item));
		}
		wake();
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

	void sleep()
	{
		while (empty()) {
			std::unique_lock<std::mutex> blocker_lock(blocker_mux);
			blocker.wait(blocker_lock);
		}
	}

	void wake()
	{
		std::unique_lock<std::mutex> blocker_lock(blocker_mux);
		blocker.notify_one();
	}

protected:
	std::mutex deque_mux{};
	std::deque<T> deque{ MAX_SIZE / 8 };
	std::mutex blocker_mux{};
	std::condition_variable blocker{};
};
} // namespace net

#endif // NET_TSQUEUE_H