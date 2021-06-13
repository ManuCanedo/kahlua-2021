# Kahlua

<p align="justify">
A lightweight local Twitch Bot written in C++. The behaviour of commands is defined in a lua script. This allows for easy expansion and customization whithout the need to recompile. Commands can be implemented on live stream as soon as Twitch Chat requests them.
</p>

## Dependencies

 - ASIO STANDALONE
*sudo apt-get install -y libasio-dev*
 - LUA 5.1
*sudo apt-get install -y lua5.1*

## Getting Started

1) Satisfy dependencies.
2) Clone and 'make'.
  If necessary, modify the include paths for *lua5.1* in the makefile.
3) Modify the '_oauth' (twitch token) and '_botname' (account name) variables in *config.lua*.
  Point the bot to the desired channel modifying '_channel'.
4) Implement commands in *commands.lua*

## Development Checklist

+ Net framework: client and connection interfaces (ASIO).
+ Implemented initial bot that receives and displays Twitch Chat.
+ Embedding Lua: configuration params obtained from 'config.lua' file.
+ Integrated FLITE text to speech generator.
+ Implemented customs params that trigger lua scripts.

## Stay in touch

+ Linkedin - https://www.linkedin.com/in/manuel-tabares/

<br><hr>
<p align="center">
  <br> <img width="1024" height="1024" src="media/colourful-science-fiction-galaxy-ai.jpg">
  <br> Science-fiction Galaxy - AI generated (vqgan+clip)
</p>

