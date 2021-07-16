#ifndef NET_CONNECTION_H
#define NET_CONNECTION_H

#include "net_common.h"
#include "net_tsqueue.h"

namespace net
{
constexpr std::size_t BUFFER_SIZE = 1024;

class Connection {
public:
	Connection(asio::io_context& context, asio::ip::tcp::socket socket,
		   ThreadSafeQueue<std::pair<std::string, std::string> >& messages)
		: context(context), socket(std::move(socket)), messages_queue(messages)
	{
	}

	void connect(asio::ip::tcp::resolver::results_type& endpoints)
	{
		auto on_connect = [this](std::error_code ec, asio::ip::tcp::endpoint endpoints) {
			if (!ec) {
				read();
			} else {
				std::cerr << "[net_connection][connect]: " << ec.message() << '\n';
				std::cerr << "[net_connection][connect]: Endpoints - "
					  << endpoints.address();
			}
		};
		asio::async_connect(socket, endpoints, on_connect);
	}

	void disconnect()
	{
		if (is_connected())
			asio::post(context, [this]() { socket.close(); });
	}

	[[nodiscard]] bool is_connected() const
	{
		return socket.is_open();
	}

	void send(std::string_view msg)
	{
		socket.write_some(asio::buffer(msg.data(), msg.size()));
	}

	void read()
	{
		auto on_read = [&](std::error_code ec, std::size_t len) {
			if (!ec) {
				std::string msg(buffer.data(), len);
				msg.rfind("PING ", 0) == 0 ? send("PONG tmi.twitch.tv\r\n") :
								   add_to_queue(msg);
				read();
			} else {
				std::cerr << "[net_connection]:[read] " << ec.message() << '\n';
				disconnect();
			}
		};
		socket.async_read_some(asio::buffer(buffer.data(), buffer.size()), on_read);
	}

	void add_to_queue(const std::string& msg)
	{
		std::regex_search(msg, match, regex);
		messages_queue.emplace_back(std::move(match[1]), std::move(match[2]));
	}

protected:
	asio::io_context& context;
	asio::ip::tcp::socket socket;
	ThreadSafeQueue<std::pair<std::string, std::string> >& messages_queue;
	std::array<char, BUFFER_SIZE> buffer{};

private:
	std::regex regex{ "!(.+)@.+ PRIVMSG #[^\\s]+ :(.*)" };
	std::smatch match;
};
} // namespace net

#endif // NET_CONNECTION_H