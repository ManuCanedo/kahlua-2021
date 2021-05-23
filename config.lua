local commands = require 'commands'

-- Configuration params
-- Variables indexed by _ are read directly by the host app
_channel = "sufakmurloc"
_oauth = "u8pf2yy60tckz4enope5555474tzmu"
_botname = "coachme_bot"
_users = {"all", "rtchoke", "torene33"}

--
--
-- Function called by Host
function _process_message(host, usr, msg)
    local command = msg:match("^!(%w+)")
    if command and commands[command] then
        local args = msg:match("^!%w+ (%.+)")
        if not validate(args) then
            return
        end
        commands[command](host, usr, args)
    end
end

-- Input validation to avoid malicious code injection
-- (to be implemented)
function validate(msg)
    return true
end
