mupen64plus-input-bot
=====================

An input driver for [mupen64plus](https://github.com/mupen64plus/mupen64plus-core) that consumes a JSON response from a web server. Intended for use with my [TensorKart](https://github.com/kevinhughes27/TensorKart) project that trains an AI using TensorFlow to play the classic N64 title MarioKart 64.


Building
--------
```shell
make all
```

The project uses essentially a copy and paste of the Makefile from the [regular input plugin for mupen64plus](https://github.com/mupen64plus/mupen64plus-input-sdl) With the dynamic config lib and SDL dependencies removed. It includes flags to link libjson-c, which it assumes has been installed in /usr/local/lib and /usr/local/include (you can get this library [here](https://github.com/json-c/json-c)).

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


Protocol
--------
The server attempts to deserialize the web response as a JSON object. It expects the JSON object to contain properties that represent each of the controller button states (as integers). If any buttons are not included in the response, their values will default to 0.

Example JSON response:
```json
{ "START_BUTTON": 0, "U_CBUTTON": 0, "L_DPAD": 0, "A_BUTTON": 1, "B_BUTTON": 0, "X_AXIS": -80, "L_CBUTTON": 0, "R_CBUTTON": 0, "R_TRIG": 0, "R_DPAD": 0, "D_CBUTTON": 0, "Z_TRIG": 0, "Y_AXIS": 80, "L_TRIG": 0, "U_DPAD": 0, "D_DPAD": 0 }
```


Testing
-------
An example server is included in `/test/server.py`. This file is a minimal example that allows you to hard code the joystick input. This example is intended to be extracted into a program that determines the appropriate input somehow.


Future Work / Ideas
-------------------
* `controller.c` could be refactored into smaller functions
* I'm sure my c code can be optimized and improved
* This plugin might be useful for implementing network play with mupen64plus


Contributing
------------
Open a PR! I'm friendly I promise :)
