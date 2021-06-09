#ifndef NET_CONNECTION_H
#define NET_CONNECTION_H

#include "net_common.h"
#include "net_tsqueue.h"

namespace net
{
constexpr size_t buffer_size = 20 * 1024;

class Connection {
public:
	Connection(asio::io_context& context, asio::ip::tcp::socket socket,
		   ThreadSafeQueue<std::pair<std::string, std::string> >& messages)
		: context(context), socket(std::move(socket)), messages_queue(messages),
		  buffer(buffer_size)
	{
	}

	void connect(asio::ip::tcp::resolver::results_type& endpoints)
	{
		asio::async_connect(
			socket, endpoints,
			[this](std::error_code ec, asio::ip::tcp::endpoint endpoints) {
				if (!ec) {
					read();
				} else {
					std::cerr << "[net_connection][connect]: " << ec.message()
						  << '\n';
					std::cerr << "[net_connection][connect]: Endpoints - "
						  << endpoints.address();
				}
			});
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
		socket.async_read_some(asio::buffer(buffer.data(), buffer.size()),
				       [&](std::error_code ec, std::size_t len) {
					       if (!ec) {
						       std::string msg(buffer.data(), len);
						       (msg.rfind("PING ", 0) == 0) ?
								     send("PONG :tmi.twitch.tv\r\n") :
								     add_to_queue(msg);
						       read();
					       } else {
						       std::cerr << "[net_connection]:[read] "
								 << ec.message() << '\n';
					       }
				       });
	}

	void add_to_queue(const std::string& msg)
	{
		std::smatch match;
		std::regex_search(msg, match, regex);
		messages_queue.push_back(std::make_pair<std::string, std::string>(
			std::move(match[1]), std::move(match[3])));
	}

protected:
	asio::io_context& context;
	asio::ip::tcp::socket socket;
	ThreadSafeQueue<std::pair<std::string, std::string> >& messages_queue;
	std::vector<char> buffer;

private:
	std::regex regex{ "!(.+)@.+ PRIVMSG #([^\\s]+) :(.*)" };
};
} // namespace net

#endif // NET_CONNECTION_H