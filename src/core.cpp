#include <iostream>

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

std::vector<char> buffer(20 * 1024);

void GrabSomeData(asio::ip::tcp::socket& socket)
{
	socket.async_read_some(asio::buffer(buffer.data(), buffer.size()),
		[&](std::error_code ec, std::size_t len)
		{
			if (!ec)
			{
				for (uint i = 0; i < len; ++i)
					std::cout << buffer[i];
				GrabSomeData(socket);
			};
		}
	);
}

int main()
{
	// Create error handler
	asio::error_code ec;
	// Create ASIO context
	asio::io_context context;
	// Provide context with idle work so it doesn't finish
	asio::io_context::work idleWork(context);
	// Start the context
	std::thread contextThread = std::thread([&](){ context.run(); });
	
	// Define target host and port
	const std::string host = "irc.chat.twitch.tv";
	const std::string port = "6667";

	// Resolve endpoint
	asio::io_service io_service;
	asio::ip::tcp::resolver resolver(io_service);
	asio::ip::tcp::resolver::query query(host, port);
	asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
	asio::ip::tcp::endpoint endpoint = iter->endpoint();

	// Create a socket
	asio::ip::tcp::socket socket(context);
	// Try to connect to endpoint
	socket.connect(endpoint, ec);

	if (!ec)
		std::cout << "Connected!" << std::endl;
	else
		std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;

	if (socket.is_open())
	{
		GrabSomeData(socket);
		std::string request = 
			"PASS oauth:r738bdlj83efvfspefgg46w9q51c2c\r\n"
			"NICK rtchoke\r\n";
		socket.write_some(asio::buffer(request.data(), request.size()), ec);
		request = 
			"JOIN #torene33\r\n";
		socket.write_some(asio::buffer(request.data(), request.size()), ec);
	}

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(5000ms);

	std::string request = 
			"PRIVMSG #torene33 :Estoy escribiendo desde mi bot C++!\r\n";
	socket.write_some(asio::buffer(request.data(), request.size()), ec);

	std::this_thread::sleep_for(5000ms);
	std::cout << "End" << std::endl;

	context.stop();
	if (contextThread.joinable()) contextThread.join();

	return 0;
}