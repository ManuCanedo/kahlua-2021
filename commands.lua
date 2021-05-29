-- User defined commands
local commands = {}

-- Use this template to add new commands. Replace XXXX and add the body:

-- commands.XXXX = function (host, user, args)
-- 	if args then
-- 		-- What to do if called with format "!XXXX args"
-- 		send_to_chat(host, "add a message to be posted to chat")
-- 	else
-- 		-- What to do if called with format "!XXXX"
-- 		send_to_chat(host, "add a message to be posted to chat")
-- 	end
-- end

commands.roll = function(host, user, args)
    if args then
        -- What to do if called with format "!roll args"
        send_to_chat(host, user .. " tira los dados y saca " .. math.random(100))
        return
    else
        -- What to do if called with format "!roll"
        send_to_chat(host, user .. " tira los dados y saca " .. math.random(100))
    end
end

local deaths = 0
commands.deaths = function(host, user, args)
    args = args or ""
    deaths = deaths + 1
    local str = _channel .. " has died " .. deaths .. " times."

    if deaths > 10 then
        str = str .. " has reached bot500!" -- Overwatch top500 joke
    end
    send_to_chat(host, str)
end

math.randomseed(os.time())
math.random()
math.random()
math.random()

return commands