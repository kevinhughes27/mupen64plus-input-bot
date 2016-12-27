mupen64plus-input-bot
=====================

An input driver for [mupen64plus](https://github.com/mupen64plus/mupen64plus-core) that consumes from a python web server. Intended for use with my TensorKart project that trains an AI using TensorFlow to play the classic N64 title MarioKart 64.


Building
--------
```shell
make all
```

The project uses essentially a copy and paste of the Makefile from the [regular input plugin for mupen64plus](https://github.com/mupen64plus/mupen64plus-input-sdl) With the dynamic config lib and SDL dependencies removed.


Usage
-----
```shell
mupen64plus --input ~/src/mupen64plus-input-bot/mupen64plus-input-bot.so MarioKart64.z64
```

assuming this repo is located in a directory called `src` in your home directory


Testing
-------
An example server is included in `/test/server.py`. This file is a minimal example that allows you to hard code the joystick input. This example is intended to be extracted into a program that determines the appropriate input somehow.


ToDo
----
Currently the server just returns a String and the driver prints it.
1. make server return an appropriate array
2. consume this array and use it to set the controller buttons (note that this controller is available globally in this plugin)
3. profit
