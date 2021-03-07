#include "TwitchBot.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

inline bool CheckLua(lua_State* L, int r)
{
	if (r != 0)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cerr << errormsg << std::endl;
		return false;
	}
	return true;
}

TwitchBot::TwitchBot()
{
	LoadConfig();
	Connect("irc.chat.twitch.tv", "6667");
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1000ms);
	LogIn();
	JoinChannel();
}

TwitchBot::~TwitchBot()
{
	Disconnect();
}

void TwitchBot::Run()
{
	auto& messages = MessagesQueue();

	while (m_IsRunning)
	{
		if(messages.size() != 0)
		{
			ProcessMessage(messages.pop_back());
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);
		}
	}
}

void TwitchBot::LoadConfig()
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	if (CheckLua(L, luaL_dofile(L, "script.lua")))
	{
		lua_getglobal(L, "oauth");
		if (lua_isstring(L, -1))
			m_Oauth = lua_tostring(L,-1);

		lua_getglobal(L, "botname");
		if (lua_isstring(L, -1))
			m_Botname = lua_tostring(L,-1);

		lua_getglobal(L, "channel");
		if (lua_isstring(L, -1))
			m_Channel = lua_tostring(L,-1);

		lua_getglobal(L, "users");
		if (lua_istable(L, -1))
		{
			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				if (lua_isstring(L, -1))
					m_Users.insert(lua_tostring(L, -1));
				lua_pop(L, 1);
			}
		}

		lua_getglobal(L, "text");
		if (lua_istable(L, -1))
		{
			lua_pushnil(L);
			while (lua_next(L, -2) != 0)
			{
				if (lua_isstring(L, -1))
					m_TextCommands.insert({ lua_tostring(L, -2), lua_tostring(L, -1) });
				lua_pop(L, 1);
			}
		}
	}
	lua_close(L);
}

void TwitchBot::LogIn()
{
	if (IsConnected())
	{
		const std::string output{"PASS oauth:" + m_Oauth + "\r\nNICK " + m_Botname + "\r\n"};
		Send(output);
	}
}

void TwitchBot::JoinChannel()
{
	if (IsConnected())
	{
		const std::string output{"JOIN #" + m_Channel + "\r\n"};
		Send(output);
	}
}

void TwitchBot::LeaveChannel()
{
	if (IsConnected())
	{
		const std::string output{"PART #" + m_Channel + "\r\n"};
		Send(output);
	}
}

void TwitchBot::AddCommand(CommandType type, const std::string& msg)
{
	switch(type)
	{
	case CommandType::TEXT:
		if (size_t pos = msg.find(" ", 5); pos != std::string::npos)
			m_TextCommands.insert({ msg.substr(5, pos-5), msg.substr(pos+1, msg.size()-pos)});
		break;
	case CommandType::VOICE:
		// TODO
	case CommandType::CUSTOM:
		// TODO
		break;
	}
}

void TwitchBot::RemoveCommand(CommandType type, const std::string& msg)
{
	switch(type)
	{
	case CommandType::TEXT:
		m_TextCommands.erase(msg.substr(4, msg.size()-1));
		break;
	case CommandType::VOICE:
		// TODO
	case CommandType::CUSTOM:
		// TODO
		break;
	}
}

void TwitchBot::ProcessMessage(const std::pair<std::string,std::string>& msg)
{
		if (msg.first != "")
			std::cout << msg.first << ": " << msg.second << std::endl;
		if (msg.first == "nightbot")
			TextCommand("Nightbot, tu calla que eres un bobo.");

		if (auto usr = m_Users.find(msg.first); usr != m_Users.end())
		{
			if ((msg.second).rfind("!add ",0) == 0) 
				AddCommand(CommandType::TEXT, msg.second);
			if ((msg.second).rfind("!rm ", 0) == 0)
				RemoveCommand(CommandType::TEXT, msg.second);
			if (auto cmd = m_TextCommands.find(msg.second);  cmd != m_TextCommands.end())
				TextCommand(cmd->second);
			if (auto cmd = m_VoiceCommands.find(msg.second);  cmd != m_VoiceCommands.end())
				VoiceCommand(cmd->second);
			if (auto cmd = m_CustomCommands.find(msg.second);  cmd != m_VoiceCommands.end())
				CustomCommand(cmd->second);
		}
}

void TwitchBot::TextCommand(std::string_view msg)
{
	if (IsConnected())
	{
		const std::string output{"PRIVMSG #" + m_Channel + " :" + msg.data() + "\r\n"};
		Send(output);
	}
}

void TwitchBot::VoiceCommand(std::string_view msg)
{
	// TODO
}

void TwitchBot::CustomCommand(std::string_view msg)
{
	// TODO
}

int main()
{
	TwitchBot jaynebot;
	jaynebot.Run();

	return 0;
}