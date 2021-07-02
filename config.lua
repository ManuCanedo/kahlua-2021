local commands = require 'commands'

--
-- Configuration file, loaded once at start of execution
-- Variables indexed by _ are directly accessed by the host app
--

-- Target channel name
_channel = "necros"

-- Bot account name and oauth token
_botname = "coachme_bot"
_oauth = "u8pf2yy60tckz4enope5555474tzmu"

-- Users allowed to interact with the bot. 'all' allows everyone in chat.
_users = {"all", "rtchoke", "torene33"}

--
--
-- Function called by Host
function _process_message(host, usr, msg)
    local command = msg:match("^!(%w+)")
    if not command then
        return
    end
    if command == "reload" then
        commands = require 'commands'
    end
    if commands[command] then
        local args = msg:match("^!%w+ (%.+)")
        if validate(args) then
            commands[command](host, usr, args)
        end
    end
end

--
-- TODO
-- Input validation to avoid malicious code injection
function validate(msg)
    return true
end
