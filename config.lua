--[[	THIS FILE CONTAINS THE CONFIGURATION THAT THE SPANISH STREAMER TORENE33 USES AT HIS STREAM. I KEEP IT THIS WAY AS AN EXAMPLE OF THE BOT FUNCTIONALITY	]]
-- Twitch Connection params
_channel = "torene33"
_oauth = "" --Oauth code needed
_botname = "" --Bot twitch account username needed


-- Users that can interact with bot
_users = { 
	"all",
	"letidguez",
	"mixi6",
	"rtchoke",
	"bycarcar_",
	"coachme_bot",
	"loisgalicia20",
	"groguka"
}


-- Commands that trigger a TEXT REPLY
_text_commands = {}
_text_commands["!quien"] = "Soy un bot en desarollo. En estos momentos me están desparasitando cual chimpancé."
_text_commands["!dev"] = "Me está desarrollando arrichou... rtichok... hortichoke... yo qué se cómo se dice! El que spamea en el chat."


-- Commands that trigger a SPEECH REPLY
_speech_commands = {}
_speech_commands["!hi"] = "Hello! I am a custom bot ready to kill all humans."


-- Commands that trigger an EMOTE REPLY
_emote_commands = {}
_emote_commands["!bailar"] = "baila como baila el mar."
_emote_commands["!asentir"] = "asiente con aprobación."
_emote_commands["!enfado"] = "se pone roj@ de enfado."
_emote_commands["!aplaudir"] = "comienza a aplaudir con efusividad."
_emote_commands["!ladrar"] = "comienza a ladrar."
_emote_commands["!suplicar"] = "se pone de rodillas y suplica."
_emote_commands["!morder"] = "te lanza un mordisquito juguetón"
_emote_commands["!besar"] = "te besa."
_emote_commands["!reverenciar"] = "hace una solemne reverencia."
_emote_commands["!paja"] = "descarga unos fardos de paja al alba. Ah! La vida en el campo..."
_emote_commands["!calmar"] = "te calma reconfortante."
_emote_commands["!felicitar"] = "te felicita."
_emote_commands["!saludar"] = "dice hola a los presentes."
_emote_commands["!cringe"] = "siente vergüenza ajena..."
_emote_commands["!acariciar"] = "te acaricia con delicadeza."
_emote_commands["!cagar"] = "caga de campo y se limpia con un helecho."


-- Possible replies to Nightbot
_bot_rivalry = { 
	"de verdad tio, callate la boca.",
 	"eres el Kim Jong-Un de los bots de Twitch.",
	"he visto farolas con más gracia.",
	"un día de estos tenemos que irnos de juerga. No me aguantas media noche.",
	"cuantos bots hacen falta para cambiar una bombilla? Matao.",
	"ba dum ts!",
	"eso dijo ella.",
	"BOT DIFF.",
	"fascista.",
	"cuéntame más."
}


-- Commands that trigger a SCRIPTED REPLY
_script_commands = {
	"!nightbot",
	"!dados",
	"!muerto"
}

-- Functions that define the behaviour of the scripted commands
function nightbot(host, user)
	i = math.random(#_bot_rivalry)
	SendToChat(host, user ..", ".. _bot_rivalry[i])
end

function dados(host, user)
	SendToChat(host, user .. " tira los dados y saca " .. math.random(100))
end

deaths = 0
function muerto(host, user)
	deaths = deaths + 1
	local str = _channel .. " ha muerto " .. deaths .. " veces."
	if deaths > 10 then
		str = str .. " Ha llegado a bot500!"
	end
	SendToChat(host, str)
end


-- Private area, do not entry
math.randomseed(os.time())
math.random(); math.random(); math.random()
