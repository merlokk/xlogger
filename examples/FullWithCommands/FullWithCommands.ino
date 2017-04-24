#include <xlogger.h>
#include <ArduinoOTA.h>

#define               PROGRAM_VERSION   "1.00"

#define DEBUG

#ifdef DEBUG
  // log level: info
  #define             DEBUG_PRINT(...)    logger.print(__VA_ARGS__)
  #define             DEBUG_PRINTLN(...)  logger.println(__VA_ARGS__)
  // log level: warning
  #define             DEBUG_WPRINT(...)   logger.print(llWarning, __VA_ARGS__)
  #define             DEBUG_WPRINTLN(...) logger.println(llWarning, __VA_ARGS__)
  // log level: error
  #define             DEBUG_EPRINT(...)   logger.print(llError, __VA_ARGS__)
  #define             DEBUG_EPRINTLN(...) logger.println(llError, __VA_ARGS__)
#else
  #define             DEBUG_PRINT(...)
  #define             DEBUG_PRINTLN(...)
  #define             DEBUG_WPRINT(...)
  #define             DEBUG_WPRINTLN(...)
  #define             DEBUG_EPRINT(...)
  #define             DEBUG_EPRINTLN(...)
#endif

xLogger               logger;
char                  HARDWARE_ID[7] = {0};

///////////////////////////////////////////////////////////////////////////
//   NTP
///////////////////////////////////////////////////////////////////////////
boolean syncEventTriggered = false;   // True if a time even has been triggered
NTPSyncEvent_t ntpEvent;              // Last triggered event

void processSyncEvent(NTPSyncEvent_t ntpEvent) {
  if (ntpEvent) {
    DEBUG_EPRINT(F("NTP sync error: "));
    switch (noResponse) {
      case noResponse: DEBUG_EPRINTLN(F("NTP server not reachable")); break;
      case invalidAddress: DEBUG_EPRINTLN(F("Invalid NTP server address")); break;
      default: DEBUG_EPRINTLN(F(""));
    }
  } else {
    DEBUG_PRINT(F("Got NTP time: "));
    DEBUG_PRINTLN(NTP.getTimeDateString(NTP.getLastNTPSync()));
  }
}

///////////////////////////////////////////////////////////////////////////
//   ESP
///////////////////////////////////////////////////////////////////////////
/*
  Function called to restart the switch
*/
void restart() {
  DEBUG_PRINTLN(F("Restart..."));
  ESP.reset();
  delay(1000);
}

/*
  Function called to reset the configuration of the switch
*/
void reset() {
  DEBUG_PRINTLN(F("Reset..."));
  WiFi.disconnect();
  delay(1000);
  ESP.reset();
  delay(1000);
}
///////////////////////////////////////////////////////////////////////////
//   ArduinoOTA
///////////////////////////////////////////////////////////////////////////

void setupArduinoOTA() {
  ArduinoOTA.onError([](ota_error_t error) {
    DEBUG_EPRINT(F("OTA error: "));
    DEBUG_EPRINTLN(error);
    switch (error) {
      case OTA_AUTH_ERROR:    DEBUG_EPRINTLN(F("OTA: Auth Failed")); break;
      case OTA_BEGIN_ERROR:   DEBUG_EPRINTLN(F("OTA: Begin Failed")); break;
      case OTA_CONNECT_ERROR: DEBUG_EPRINTLN(F("OTA: Connect Failed")); break;
      case OTA_RECEIVE_ERROR: DEBUG_EPRINTLN(F("OTA: Receive Failed")); break;
      case OTA_END_ERROR:     DEBUG_EPRINTLN(F("OTA: End Failed")); break;
    }

    ESP.restart();
  });

  ArduinoOTA.onStart([]() {
    DEBUG_PRINTLN(F("Start OTA"));
  });
  ArduinoOTA.onEnd([]() {
    DEBUG_PRINTLN(F("End OTA"));
  });

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // No authentication by default
  // ArduinoOTA.setPassword("1212");
  ArduinoOTA.setHostname(HARDWARE_ID);
  ArduinoOTA.begin();
}

///////////////////////////////////////////////////////////////////////////
//  execute commands
///////////////////////////////////////////////////////////////////////////
const char* strCommandsDesc = "Command 'reboot' reboots ESP.";

bool CmdCallback(String &cmd) {
  if (cmd == "reboot") {
    DEBUG_PRINTLN(F("COMMAND: reboot. Rebooting..."));
    restart();
    delay(200);
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////
//   Setup() and loop()
///////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.setDebugOutput(false);
  Serial1.setDebugOutput(true);
  Serial.begin(115200);
  Serial1.begin(115200);

  sprintf(HARDWARE_ID, "%06X", ESP.getChipId());
  logger.cmdCallback(CmdCallback, strCommandsDesc);
  logger.begin(HARDWARE_ID, &Serial1, true);
  logger.setProgramVersion(PROGRAM_VERSION);
  logger.setTimeFormat(ltNone);
  logger.setShowDebugLevel(true);                // show debug level in logs
  logger.setFilterDebugLevel(llInfo);            // filter debug level log lines

  DEBUG_PRINTLN(F("Starting..."));
  DEBUG_PRINT(F("Hardware id:"));
  DEBUG_PRINTLN(HARDWARE_ID);
  DEBUG_PRINT(F("MAC address: "));
  DEBUG_PRINTLN(WiFi.macAddress());

  WiFi.mode(WIFI_STA);
  WiFi.begin("ap name", "ap password");   // ssid, password
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  DEBUG_PRINT("WiFi status=");
  DEBUG_PRINTLN(WiFi.status());
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());

  // NTP config
  NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
    ntpEvent = event;
    syncEventTriggered = true;
  });
  NTP.begin("pool.ntp.org", 0, false);
  NTP.setInterval(30 * 60);                  // twice a hour

  // ArduinoOTA
  setupArduinoOTA();

  // set password in work mode
  logger.setPassword("1212");
}

void loop() {
  DEBUG_WPRINTLN(F("*****************************************"));

  // show var
  logger.print('c');                              // char
  logger.println("-char");                        // *char
  logger.println(F("flash char"));                // *char from flash
  logger.println(String("String class"));         // String
  logger.println(SF("String class from flash"));  // String from flash
  logger.print("DEC:");
  logger.println(10);
  logger.print("HEX:");
  logger.println(10, HEX);
  logger.print("float:");
  logger.println(10.101);
  logger.printf(llError, "printf dec=%d hex=%02x char=%c str=%s \r\n", 10, 10, 'c', "strline");

  // debug macro
  DEBUG_PRINTLN(F("Debug info level"));
  DEBUG_WPRINTLN(F("Debug warning level"));
  DEBUG_EPRINTLN(F("Debug error level"));

  // time
  DEBUG_PRINT("NTP time: ");
  DEBUG_PRINTLN((timeStatus() == timeSet)? llInfo:llError, NTP.getTimeDateString());

  for(int i = 0; i < 10; i ++) {
    logger.handle();
    ArduinoOTA.handle();
    yield();

    // NTP
    if (syncEventTriggered) {
      processSyncEvent(ntpEvent);
      syncEventTriggered = false;
    }

    delay(500);
  }
}
