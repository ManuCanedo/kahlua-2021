#ifndef TWITCH_BOT_H
#define TWITCH_BOT_H


#include "net_common.h"
#include "net_connection.h"
#include "net_client.h"
#include "net_tsqueue.h"

class TwitchBot : public net::client_interface
{
	enum class CommandType { TEXT, VOICE, CUSTOM };

public:
	TwitchBot();
	~TwitchBot();

	void LoadConfig();
	void Run();

private:
	void LogIn();

	void AddCommand(const CommandType& type, const std::string& msg);
	void RemoveCommand(const CommandType& type, const std::string& msg);
	void ProcessMessage(const std::pair<std::string,std::string>& msg);
	
	void TextCommand(std::string_view msg);
	void SpeechCommand(std::string_view msg);
	void CustomCommand(std::string_view msg);

private:
	std::string m_Oauth, m_Botname, m_Channel;
	std::set<std::string> m_Users;
	std::set<std::string> m_CustomCommands;
	std::unordered_map<std::string, std::string> m_TextCommands;
	std::unordered_map<std::string, std::string> m_SpeechCommands;

	bool m_IsRunning = true;
};


#endif // TWITCH_BOT_H