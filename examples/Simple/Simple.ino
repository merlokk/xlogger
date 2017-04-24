#include <xlogger.h>

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

void setup() {
//  Serial.setDebugOutput(false);
//  Serial1.setDebugOutput(true);
  Serial.begin(115200);
//  Serial1.begin(115200);

  sprintf(HARDWARE_ID, "%06X", ESP.getChipId());
  logger.begin(HARDWARE_ID, &Serial, true);
  logger.setProgramVersion(PROGRAM_VERSION);

  DEBUG_PRINTLN(F("Starting..."));
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

  for(int i = 0; i < 10; i ++) {
    logger.handle();
    yield();

    delay(500);
  }
}
