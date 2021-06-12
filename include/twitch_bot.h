#ifndef TWITCH_BOT_H
#define TWITCH_BOT_H

#include "net_common.h"
#include "net_connection.h"
#include "net_client.h"
#include "net_tsqueue.h"

class TwitchBot : public net::ClientInterface {
public:
	~TwitchBot();

	static void Start()
	{
		Get().run();
	}

	static void Stop()
	{
		Get.pause();
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
	void load();
	void login() const;
	void run();
	void pause();

	void process_message(const std::string& usr, const std::string& msg);

private:
	std::string oauth{};
	std::string botname{};
	std::string channel{};
	std::set<std::string> users{};
	bool is_running{ true };
};

#endif // TWITCH_BOT_H
