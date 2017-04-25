# xLogger - ESP8266 logger library. 
It can show logs and execute commands via telnet 

## Logger can do:

- put log to the serial port (hardware, software, stream). Can be enabled and disabled.
- save part of the log in RAM. (LOG_SIZE - length of memory buffer)
- put log to the connected terminal via the telnet port (23 by default)
- password can be settled for telnet connection
- filter the log by the log level: info /warning/error
- display the level in message list
- show the time in message list in intervals between lines, milliseconds, seconds and GMT time
- execute built-in and user's commands

## Sample initial telnet message with password and user commands

```*** Telnet debug for ESP8266.
Program version: 0.92 Logger version: 1.0.
Host name: XXXXXX IP:192.168.99.168 Mac address:XX:XX:XX:XX:XX:XX
Free Heap RAM: 30088

Command serial [enable|disable|?] write log to serial debug port.
Serial: enable
Command showdebuglvl [enable|disable|?] shows debug level in log lines.
Show debug level: enable
Command loglvl [info|warning|error|?] filters messages by log level.
Log level: Info
Command time [none|str|ms|btw|gmt|?] shows time in log lines.
Time format: gmt time (needs NTP available)
Command reboot reboots ESP.
Command startwificfg puts ESP to configure mode. Show configuration AP.
Command cfgdevice writes RS-485 device type to ESP memory and reboot.

Please, enter password before entering commands. Password length may be up to 10 symbols.```


