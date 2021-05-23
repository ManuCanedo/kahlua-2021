#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include "net_common.h"
#include "net_connection.h"
#include "net_tsqueue.h"

namespace net
{
class ClientInterface {
public:
	ClientInterface() = default;

	virtual ~ClientInterface()
	{
		disconnect();
	}

	ClientInterface(const ClientInterface&) = delete;
	ClientInterface(ClientInterface&&) = delete;

	[[nodiscard]] bool is_connected() const
	{
		return (connection) ? connection->is_connected() : false;
	}

	bool connect(std::string_view host, std::string_view port)
	{
		try {
			asio::ip::tcp::resolver resolver(context);
			auto endpoints = resolver.resolve(host, port);
			connection = std::make_unique<Connection>(
				context, asio::ip::tcp::socket(context), messages);
			connection->connect(endpoints);
			thread = std::thread([this]() { context.run(); });
		} catch (std::exception& e) {
			std::cerr << "[net_client]:[connect] " << e.what() << '\n';
			return false;
		}
		return true;
	}

	void disconnect()
	{
		if (is_connected())
			connection->disconnect();
		context.stop();
		if (thread.joinable())
			thread.join();
		connection.release();
	}

	void send(std::string_view msg) const
	{
		if (is_connected())
			connection->send(msg);
	}

protected:
	[[nodiscard]] auto& messages_queue()
	{
		return messages;
	}

protected:
	asio::io_context context{};
	std::thread thread{};
	std::unique_ptr<Connection> connection{};
	ThreadSafeQueue<std::pair<std::string, std::string> > messages{};
};
} // namespace net

#endif // NET_CLIENT_H
