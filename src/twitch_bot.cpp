#include "twitch_bot.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// CTRL+C Catcher
void signal_callback_handler(int signum)
{
	std::cout << " Caught signal " << signum << ". Exiting." << std::endl;
	exit(signum);
}

// LUA functions
bool check_lua(lua_State* L, int r)
{
	if (r) {
		printf("%s \n", lua_tostring(L, -1));
		return false;
	}
	return true;
}

void load_lua_str(lua_State* L, const char* var_name, std::string& output_str)
{
	lua_getglobal(L, var_name);
	if (!lua_isstring(L, -1))
		return;

	output_str = lua_tostring(L, -1);
}

void load_lua_arr(lua_State* L, const char* var_name, std::set<std::string>& output_arr)
{
	lua_getglobal(L, var_name);
	if (!lua_istable(L, -1))
		return;

	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		if (lua_isstring(L, -1))
			output_arr.emplace(lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

int lua_send(lua_State* L)
{
	if (lua_gettop(L) == 2) {
		TwitchBot* bot = static_cast<TwitchBot*>(lua_touserdata(L, 1));
		if (bot->is_connected())
			bot->send("PRIVMSG #" + bot->channel_name() + " :" + lua_tostring(L, 2) +
				  "\r\n");
	} else {
		std::cerr << "[Global]:[lua_send] invalid number of args returned from lua.\n";
		return 1;
	}
	return 0;
}

// TwitchBot class definitions
TwitchBot::TwitchBot()
{
	load();
	connect("irc.chat.twitch.tv", "6667");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	login();
}

void TwitchBot::run()
{
	auto& messages = messages_queue();

	while (is_running) {
		messages.wait(); // blocking until a new message in the queue
		const auto message = messages.pop_front();

		if (message.first != "") {
			std::cout << message.first << "\t-->\t" << message.second << "\n";
			if (message.second[0] == '!')
				process_message(message.first, message.second);
		}
	}
}

void TwitchBot::load()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	if (check_lua(L, luaL_dofile(L, "config.lua"))) {
		load_lua_str(L, "_oauth", oauth);
		load_lua_str(L, "_botname", botname);
		load_lua_str(L, "_channel", channel);
		load_lua_arr(L, "_users", users);
	}
	lua_close(L);
}

void TwitchBot::login() const
{
	if (is_connected()) {
		send("PASS oauth:" + oauth + "\r\n");
		send("NICK " + botname + "\r\n");
		send("JOIN #" + channel + "\r\n");
	}
}

void TwitchBot::process_message(const std::string& usr, const std::string& msg)
{
	static lua_State* L;
	static bool scr_loaded = false;
	if (!scr_loaded) {
		L = luaL_newstate();
		luaL_openlibs(L);
		scr_loaded = check_lua(L, luaL_dofile(L, "config.lua"));
		lua_register(L, "send_to_chat", lua_send);
	}

	if (auto usrIt = users.find(usr);
	    usrIt != users.end() || users.find("all") != users.end()) {
		lua_getglobal(L, "_process_message");
		if (lua_isfunction(L, -1)) {
			lua_pushlightuserdata(L, this);
			lua_pushstring(L, usr.data());
			lua_pushstring(L, msg.data());
			check_lua(L, lua_pcall(L, 3, 1, 0));
		}
	}
}

int main()
{
	signal(SIGINT, signal_callback_handler);

	std::cout << "\n\tHello streamer! I'm your new bot.\n"
		  << "\t\tClose this window to disconnect me.\n"
		  << "\t\tIf an error message displays below, please restart me.\n\n";

	TwitchBot::start();

	return 0;
}