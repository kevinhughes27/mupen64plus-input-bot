mupen64plus-input-bot
=====================

An input driver for [mupen64plus](https://github.com/mupen64plus/mupen64plus-core) that consumes from a python web server. Intended for use with my [TensorKart](https://github.com/kevinhughes27/TensorKart) project that trains an AI using TensorFlow to play the classic N64 title MarioKart 64.


Building
--------
```shell
make all
```

The project uses essentially a copy and paste of the Makefile from the [regular input plugin for mupen64plus](https://github.com/mupen64plus/mupen64plus-input-sdl) With the dynamic config lib and SDL dependencies removed.

Note that all builds expect the mupen64plus-core source code to be available in the same directory.

eg.
```
src \
  mupen64plus-core \
    ...
  mupen64plus-input-bot \
    ...
```

Usage
-----
```shell
mupen64plus --input ~/src/mupen64plus-input-bot/mupen64plus-input-bot.so MarioKart64.z64
```

assuming this repo is located in a directory called `src` under your home directory


Testing
-------
An example server is included in `/test/server.py`. This file is a minimal example that allows you to hard code the joystick input. This example is intended to be extracted into a program that determines the appropriate input somehow.


Future Work / Ideas
-------------------
* I only implemented what I required for buttons. This could be expanded and built into a fully fledged solution
* Use a more structured protocol between the plugin and server (aka JSON)
* `controller.c` could be refactored into smaller functions
* I'm sure my c code can be optimized and improved
* This plugin might be useful for implementing network play with mupen64plus


Contributing
------------
Open a PR! I'm friendly I promise :)
