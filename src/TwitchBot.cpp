#define PROFILING 0
#if PROFILING
#define START_PROFILER() Instrumentor::Get().BeginSession("TwitchBot Profiling")
#define STOP_PROFILER() Instrumentor::Get().EndSession()
#define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
#else
#define START_PROFILER() 
#define STOP_PROFILER() 
#define PROFILE_SCOPE(name) 
#define PROFILE_FUNCTION() 
#endif

#include "TwitchBot.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "flite.h"
}

// CTRL+C Catcher
void signal_callback_handler(int signum) 
{
   std::cout << " Caught signal " << signum << ". Exiting." << std::endl;
   Instrumentor::Get().EndSession();
   exit(signum);
}

// FLITE functions
extern "C" cst_voice *register_cmu_us_kal();

inline void TextToSpeech(const char *str)
{
	PROFILE_FUNCTION();
	cst_voice *v;
	flite_init();
	v = register_cmu_us_kal();
	flite_text_to_speech(str, v, "play");
}

// LUA functions
inline bool CheckLua(lua_State *L, int r)
{
	PROFILE_FUNCTION();
	if (r != 0)
	{
		printf("%s \n", lua_tostring(L, -1));
		return false;
	}
	return true;
}

inline void LoadLuaString(lua_State *L, const char *varName, std::string &str)
{
	PROFILE_FUNCTION();
	lua_getglobal(L, varName);
	if (lua_isstring(L, -1))
		str = lua_tostring(L, -1);
}

inline void LoadLuaArray(lua_State *L, const char *varName, std::set<std::string> &arr)
{
	PROFILE_FUNCTION();
	lua_getglobal(L, varName);
	if (lua_istable(L, -1))
	{
		lua_pushnil(L);
		while (lua_next(L, -2) != 0)
		{
			if (lua_isstring(L, -1))
				arr.emplace(lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
}

inline void LoadLuaTable(lua_State *L, const char *varName, std::unordered_map<std::string, std::string> &table)
{
	PROFILE_FUNCTION();
	lua_getglobal(L, varName);
	if (lua_istable(L, -1))
	{
		lua_pushnil(L);
		while (lua_next(L, -2) != 0)
		{
			if (lua_isstring(L, -1))
				table.emplace(lua_tostring(L, -2), lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
}

int lua_SendToChat(lua_State *L)
{
	PROFILE_FUNCTION();
	if (lua_gettop(L) == 2)
	{
		TwitchBot *tb = static_cast<TwitchBot *>(lua_touserdata(L, 1));
		if (tb->IsConnected())
			tb->Send("PRIVMSG #" + tb->GetChannelName() + " :" + lua_tostring(L, 2) + "\r\n");
	}
	else
		std::cerr << "[Global]:[lua_SendToChat] invalid number of args returned from lua.\n";
	return 0;
}

// TwitchBot class definitions
TwitchBot::TwitchBot()
{
	PROFILE_FUNCTION();
	LoadConfig();
	Connect("irc.chat.twitch.tv", "6667");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	LogIn();
}

TwitchBot::~TwitchBot()
{
	PROFILE_FUNCTION();
	Disconnect();
}

void TwitchBot::Run()
{
	PROFILE_FUNCTION();
	auto &messages = MessagesQueue();

	while (m_IsRunning)
	{
		if (messages.size() != 0)
		{
			const auto message = messages.pop_front();
			ProcessMessage(message.first, message.second);
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
	}
}

void TwitchBot::Stop()
{
	PROFILE_FUNCTION();
	m_IsRunning = false;
}

void TwitchBot::LoadConfig()
{
	PROFILE_FUNCTION();
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	if (CheckLua(L, luaL_dofile(L, "config.lua")))
	{
		LoadLuaString(L, "_oauth", m_Oauth);
		LoadLuaString(L, "_botname", m_Botname);
		LoadLuaString(L, "_channel", m_Channel);
		LoadLuaArray(L, "_users", m_Users);
		LoadLuaArray(L, "_script_commands", m_ScriptCommands);
		LoadLuaTable(L, "_text_commands", m_TextCommands);
		LoadLuaTable(L, "_speech_commands", m_SpeechCommands);
		LoadLuaTable(L, "_emote_commands", m_EmoteCommands);
	}
	lua_close(L);
}

void TwitchBot::LogIn()
{
	PROFILE_FUNCTION();
	if (IsConnected())
	{
		Send("PASS oauth:" + m_Oauth + "\r\n");
		Send("NICK " + m_Botname + "\r\n");
		Send("JOIN #" + m_Channel + "\r\n");
	}
}

void TwitchBot::AddCommand(CommandType type, std::string_view msg)
{
	PROFILE_FUNCTION();
	switch (type)
	{
	case CommandType::TEXT:
		if (size_t pos = msg.find(" ", 5); pos != std::string_view::npos)
			m_TextCommands.emplace(msg.substr(5, pos - 5), msg.substr(pos + 1, msg.size() - pos));
		break;
	case CommandType::VOICE:  // TODO
	case CommandType::CUSTOM: // TODO
		break;
	}
}

void TwitchBot::RemoveCommand(CommandType type, std::string_view msg)
{
	PROFILE_FUNCTION();
	switch (type)
	{
	case CommandType::TEXT:
		m_TextCommands.erase(msg.substr(4, msg.size() - 1).data());
		break;
	case CommandType::VOICE:  // TODO
	case CommandType::CUSTOM: // TODO
		break;
	}
}

void TwitchBot::ProcessMessage(std::string_view usr, std::string_view msg)
{
	PROFILE_FUNCTION();
	if (usr != "")
		std::cout << usr << "\t\t--> " << msg << '\n';
	if (usr == "nightbot")
		ScriptCommand(usr, usr);

	if (auto usrIt = m_Users.find(msg.data()); usrIt != m_Users.end() || m_Users.find("all") != m_Users.end())
	{
		// Checking !add, !rm and !play commands
		if (msg.rfind("!add ", 0) == 0)
		{
			AddCommand(CommandType::TEXT, msg);
			return;
		}
		if (msg.rfind("!rm ", 0) == 0)
		{
			RemoveCommand(CommandType::TEXT, msg);
			return;
		}
		if (msg.rfind("!play ", 0) == 0)
		{
			SpeechCommand(msg.substr(6, msg.size()));
			return;
		}
		
		// Checking if command is defined
		if (auto cmdIt = m_TextCommands.find(msg.data()); cmdIt != m_TextCommands.end())
			TextCommand(cmdIt->second);
		if (auto cmdIt = m_SpeechCommands.find(msg.data()); cmdIt != m_SpeechCommands.end())
			SpeechCommand(cmdIt->second);
		if (auto cmdIt = m_EmoteCommands.find(msg.data()); cmdIt != m_EmoteCommands.end())
			EmoteCommand(usr, cmdIt->second);
		if (auto cmdIt = m_ScriptCommands.find(msg.data()); cmdIt != m_ScriptCommands.end())
			ScriptCommand(std::string_view(*cmdIt).substr(1, cmdIt->size()), usr);
	}
}

void TwitchBot::TextCommand(std::string_view msg) const
{
	PROFILE_FUNCTION();
	if (IsConnected())
		Send("PRIVMSG #" + m_Channel + " :" + msg.data() + "\r\n");
}

void TwitchBot::SpeechCommand(std::string_view msg) const
{
	PROFILE_FUNCTION();
	TextToSpeech(msg.data());
}

void TwitchBot::EmoteCommand(std::string_view usr, std::string_view msg) const
{
	PROFILE_FUNCTION();
	if (IsConnected())
		Send("PRIVMSG #" + m_Channel + " :" + usr.data() + " " + msg.data() + "\r\n");
}

void TwitchBot::ScriptCommand(std::string_view cmd, std::string_view usr)
{
	PROFILE_FUNCTION();
	static lua_State *L;
	static bool bScriptLoaded = false;
	if (!bScriptLoaded)
	{
		L = luaL_newstate();
		luaL_openlibs(L);
		bScriptLoaded = CheckLua(L, luaL_dofile(L, "config.lua"));
		lua_register(L, "SendToChat", lua_SendToChat);
	}

	lua_getglobal(L, cmd.data());
	if (lua_isfunction(L, -1))
	{
		lua_pushlightuserdata(L, this);
		lua_pushstring(L, usr.data());
		CheckLua(L, lua_pcall(L, 2, 1, 0));
	}
}

int main()
{
	START_PROFILER();
	signal(SIGINT, signal_callback_handler);
	std::cout << "\nHello streamer! I'm your new bot.\n\n";
	TwitchBot jaynebot;
	jaynebot.Run();
	STOP_PROFILER();

	return 0;
}