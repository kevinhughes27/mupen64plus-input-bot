mupen64plus-input-bot
=====================

An input driver for [mupen64plus](https://github.com/mupen64plus/mupen64plus-core) that consumes a JSON response from a web server. Intended for use with my [TensorKart](https://github.com/kevinhughes27/TensorKart) project that trains an AI using TensorFlow to play the classic N64 title MarioKart 64.


Building
--------
```shell
make all
```

The project uses essentially a copy and paste of the Makefile from the [regular input plugin for mupen64plus](https://github.com/mupen64plus/mupen64plus-input-sdl) with the SDL dependencies removed. It includes flags to link `libjson-c`, which it assumes has been installed into `/usr/lib` and `/usr/include`. You can get this library on Ubuntu by installing the following packages (or if you prefer, you can build the latest from source [here](https://github.com/json-c/json-c)):
```
libjson-c2
libjson-c-dev
```

Note that all builds expect the mupen64plus-core source code to be available in the same directory.

eg.
```
src \
  mupen64plus-core \
    ...
  mupen64plus-input-bot \
    ...
```

There is a Makefile argument (`APIDIR`) to specify this directory path if you prefer.


Installing
----------

After building, run `sudo make install` to copy the driver to the default location.

```
/usr/local/lib/mupen64plus/mupen64plus-input-bot.so
```

Installing the driver to a standard system location allows downstream applications to configure mupen64plus automatically (for example the [mupen64plus OpenAI gym](https://github.com/bzier/gym-mupen64plus))

Usage
-----
```sh
mupen64plus --input /usr/local/lib/mupen64plus/mupen64plus-input-bot.so MarioKart64.z64
```

By default, the driver specifies the HTTP server as `localhost:8082` with a single controller connected. This can be overridden per controller via configuration. 

Configuration
-------------
Parameters can be specified via the Mupen64plus config file, or via command line switches (`--set`). There are currently three values which can be configured:

| Parameter | Description                                                             |
| --------- | ----------------------------------------------------------------------- |
| `plugged` | deterimines whether or not the controller is connected (values: 0 or 1) |
| `host`    | the hostname of the controller server (max length: 256 chars)           |
| `port`    | the port for the controller server (type: int)                          |

Mupen64plus config file:
```ini
# Default unix path:
# ~/.config/mupen64plus/mupen64plus.cfg

# [...]

[Input-Bot-Control0]

plugged = "1"
host = "contr_srv_1.example.com"
port = "8082"

[Input-Bot-Control1]

plugged = "1"
host = "contr_srv_1.example.com"
port = "8083"

[Input-Bot-Control2]

plugged = "1"
host = "contr_srv_2.example.com"
port = "8084"

[Input-Bot-Control3]

plugged = "0"
host = "contr_srv_2.example.com"
port = "8085"

# [...]
```

Command line:
```sh
mupen64plus                                                    \
  --input /usr/local/lib/mupen64plus/mupen64plus-input-bot.so  \
                                                               \
  --set Input-Bot-Control0[plugged]=1                          \
  --set Input-Bot-Control0[host]=contr_srv_1.example.com       \
  --set Input-Bot-Control0[port]=8082                          \
                                                               \
  --set Input-Bot-Control1[plugged]=1                          \
  --set Input-Bot-Control1[host]=contr_srv_1.example.com       \
  --set Input-Bot-Control1[port]=8083                          \
                                                               \
  MarioKart64.z64
```


Protocol
--------

This driver supports all 4 controllers. When it issues requests for the controller data, it makes a GET request to the path of the controller index (0-3). This allows server implementations to handle requests for each of the 4 controllers. Alternatively, separate web servers could be used for each controller (see the Configuration section above for details).

Example requests:
```
GET /0 HTTP/1.1
GET /1 HTTP/1.1
GET /2 HTTP/1.1
GET /3 HTTP/1.1
```

The plugin attempts to deserialize the web response as a JSON object. It expects the JSON object to contain properties that represent each of the controller button states (as integers). If any buttons are not included in the response, their values will default to 0.

Example JSON response:
```json
{
  "START_BUTTON": 0,
  "U_CBUTTON": 0,
  "L_DPAD": 0,
  "A_BUTTON": 1,
  "B_BUTTON": 0,
  "X_AXIS": -80,
  "L_CBUTTON": 0,
  "R_CBUTTON": 0,
  "R_TRIG": 0,
  "R_DPAD": 0,
  "D_CBUTTON": 0,
  "Z_TRIG": 0,
  "Y_AXIS": 80,
  "L_TRIG": 0,
  "U_DPAD": 0,
  "D_DPAD": 0
}
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
