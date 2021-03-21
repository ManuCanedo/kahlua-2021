-- Configuration params
-- Variables indexed by _ are read directly by the host app
_channel = "basedtrolso"
_oauth = "u8pf2yy60tckz4enope5555474tzmu"
_botname = "coachme_bot"
_users = { "all", "rtchoke", "torene33", }


-- User defined commands
commands = {}

-- Use this template to add new commands, just replace XXXX and add the body:

-- commands.XXXX = function (host, user, args)
-- 	if args == nil then 
-- 		-- What to do if called with format "!XXXX"
-- 		send_to_chat(host, "add a message to be posted to chat")
-- 	else
-- 		-- What to do if called with format "!XXXX args"
-- 		send_to_chat(host, "add a message to be posted to chat")
-- 	end
-- end

commands.dados = function (host, user, args)
	-- What to do if called with format "!dados"
	-- What to do if called with format "!dados args"
	send_to_chat(host, user .. " tira los dados y saca " .. math.random(100))
end


deaths = 0
commands.deaths = function (host, user, args)
	if args == nil then 
		args = "" 
	end
	deaths = deaths + 1
	local str = _channel .. " ha muerto " .. deaths .. " veces."
	if deaths > 10 then 
		str = str .. " Ha llegado a bot500!" 
	end
	-- What to do if called with format "!dados"
	-- What to do if called with format "!dados args"
	send_to_chat(host, str)
end


-- Function called by Host
function process_message(host, usr, msg)
	local command = msg:match("^!(%w+)")
	if command ~= nil and commands[command] ~= nil then
		local args = msg:match("^!%w+ (%.+)")
		commands[command](host, usr, args)
	end
end


-- Global variables and initializations
math.randomseed(os.time())
math.random(); math.random(); math.random()