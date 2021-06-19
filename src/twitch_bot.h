#ifndef TWITCH_BOT_H
#define TWITCH_BOT_H

class TwitchBot : public net::ClientInterface {
public:
	~TwitchBot();

	TwitchBot(const TwitchBot&) = delete;
	TwitchBot& operator=(const TwitchBot&) = delete;

	static void Start()
	{
		Get().run();
	}

	static void Stop()
	{
		Get().pause();
	}

	[[nodiscard]] static TwitchBot& Get()
	{
		static TwitchBot instance;
		return instance;
	}

	[[nodiscard]] static const std::string& Channel()
	{
		return Get().channel;
	}

private:
	TwitchBot();
	bool load_config();
	void login() const;
	void run();
	void pause();

	bool process_message(std::string&& usr, std::string&& msg);

private:
	std::string oauth{};
	std::string botname{};
	std::string channel{};
	std::unordered_set<std::string> users{};
	bool is_running{ true };
};

#endif // TWITCH_BOT_H
