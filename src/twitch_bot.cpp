#include "net_interface.h" // pch

#include "twitch_bot.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// CTRL+C catcher
void signal_callback_handler(int signum)
{
	TwitchBot::Stop();
	std::cerr << "\n\nCaught signal " << signum << ".";
	std::cerr << "\nExited gracefully.";
}

// LUA functions
lua_State* get_lua_handler()
{
	static lua_State* L = luaL_newstate();
	return L;
}

bool check_lua(lua_State* L, int r)
{
	if (r) {
		printf("%s \n", lua_tostring(L, -1));
		return false;
	}
	return true;
}

std::string load_lua_str(lua_State* L, std::string_view var_name)
{
	lua_getglobal(L, var_name.data());
	if (!lua_isstring(L, -1)) {
		std::cout << "[Global]:[load_lua_str] var is not a string.\n";
		return {};
	}
	return lua_tostring(L, -1);
}

void load_lua_set(lua_State* L, std::string_view var_name,
		  std::unordered_set<std::string>& output_set)
{
	lua_getglobal(L, var_name.data());
	if (!lua_istable(L, -1))
		return;

	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		if (lua_isstring(L, -1))
			output_set.emplace(lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

int lua_send(lua_State* L)
{
	if (lua_gettop(L) != 2) {
		std::cerr << "[Global]:[lua_send] invalid number of args returned from lua.\n";
		return 1;
	}
	auto* bot = static_cast<TwitchBot*>(lua_touserdata(L, 1));
	if (bot->is_connected())
		bot->send("PRIVMSG #" + bot->Channel() + " :" + lua_tostring(L, 2) + "\r\n");

	return 0;
}

// TwitchBot class definitions
TwitchBot::TwitchBot()
{
	if (!load_config()) {
		pause();
		return;
	}
	connect("irc.chat.twitch.tv", "6667");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	login();
}

TwitchBot::~TwitchBot()
{
	lua_close(get_lua_handler());
}

bool TwitchBot::load_config()
{
	lua_State* L = get_lua_handler();
	luaL_openlibs(L);
	lua_register(L, "send_to_chat", lua_send);

	if (!check_lua(L, luaL_dofile(L, "config.lua"))) {
		std::cerr << "[TwitchBot]:[load_config] error loading config file.\n";
		return false;
	}
	oauth = load_lua_str(L, "_oauth");
	botname = load_lua_str(L, "_botname");
	channel = load_lua_str(L, "_channel");
	load_lua_set(L, "_users", users);

	return true;
}

void TwitchBot::login() const
{
	if (!is_connected())
		return;

	send("PASS oauth:" + oauth + "\r\n");
	send("NICK " + botname + "\r\n");
	send("JOIN #" + channel + "\r\n");
	std::cout << "\nConnected to " << channel << "'s chat.\n";
}

void TwitchBot::run()
{
	static std::future<bool> promise;
	auto& messages = messages_queue();

	while (is_running) {
		messages.sleep();
		const auto message = messages.pop_front();

		if (message.first.empty())
			continue;
		std::cout << message.first << "\t-->\t" << message.second << "\n";
		if (message.second[0] != '!')
			continue;

		if (promise.valid())
			promise.wait();
		promise = std::async(std::launch::async, &TwitchBot::process_message, this,
				     std::move(message.first), std::move(message.second));
	}
}

void TwitchBot::pause()
{
	is_running = false;
	messages_queue().push_front(std::make_pair<std::string, std::string>({}, {})); // wakes awaiting thread
}

bool TwitchBot::process_message(std::string&& usr, std::string&& msg)
{
	if (users.find(usr) == users.end() && users.find("all") == users.end())
		return true;

	lua_State* L = get_lua_handler();
	lua_getglobal(L, "_process_message");
	if (!lua_isfunction(L, -1)) {
		std::cerr
			<< "[TwitchBot]:[process_message] error calling lua '_process_message'.\n";
		return false;
	}
	lua_pushlightuserdata(L, this);
	lua_pushstring(L, usr.data());
	lua_pushstring(L, msg.data());
	check_lua(L, lua_pcall(L, 3, 1, 0));

	return true;
}

int main()
{
	signal(SIGINT, signal_callback_handler);

	std::cout << "\n\tHello streamer! I'm your new bot.\n"
		  << "\t\tClose this window to disconnect me.\n"
		  << "\t\tIf an error message displays below, please restart me.\n";

	TwitchBot::Start();

	return EXIT_SUCCESS;
}
