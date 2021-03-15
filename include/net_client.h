#ifndef NET_CLIENT_H
#define NET_CLIENT_H


#include "net_common.h"
#include "net_connection.h"
#include "net_tsqueue.h"

namespace net
{
class client_interface
{
public:
	~client_interface()
	{
		PROFILE_FUNCTION();
		Disconnect();
	}

public:
	bool IsConnected() const
	{
		return (m_Connection) ? m_Connection->IsConnected() : false;
	}

	bool Connect(std::string_view host, std::string_view port)
	{
		PROFILE_FUNCTION();
		try
		{
			asio::ip::tcp::resolver resolver(m_Context);
			asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, port);
			m_Connection = std::make_unique<connection>(m_Context, asio::ip::tcp::socket(m_Context), m_Messages);
			m_Connection->Connect(endpoints);
			m_Thread = std::thread([this](){ m_Context.run(); });
		}
		catch (std::exception& e)
		{
			std::cerr << "[net_client]:[Connect] " << e.what() << '\n';
			return false;
		}
		return true;
	}

	void Disconnect()
	{
		PROFILE_FUNCTION();
		if (IsConnected()) 
			m_Connection->Disconnect();
		m_Context.stop();
		if (m_Thread.joinable()) 
			m_Thread.join();
		m_Connection.release();
	}

	void Send(std::string_view msg) const
	{
		PROFILE_FUNCTION();
		if (IsConnected())
			m_Connection->Send(msg);
	}

protected:
	auto& MessagesQueue()
	{
		return m_Messages;
	}

protected:
	asio::io_context m_Context;
	std::thread m_Thread;
	std::unique_ptr<connection> m_Connection;
	tsqueue<std::pair<std::string,std::string>> m_Messages;
};
}


#endif // NET_CLIENT_H
