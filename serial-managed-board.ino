const uint16_t SERIAL_BAUD_RATE = 9600;
const uint16_t COMMAND_BUFFER_SIZE = 100;
const String WELCOME_MESSAGE = "Welcome to Adriano's Arduino UNO Serial TTY Console!";
const char NEWLINE_CHARACTER = '\n';
const char CARRIAGE_RETURN_CHARACTER = '\r';
const String ON = "ON";
const String OFF = "OFF";
const String STATUS = "STATUS";
const uint8_t ARDUINO_MIN_PIN = 2;
const uint8_t ARDUINO_MAX_PIN = 19;

String command;
bool commandReceived = false;

void setup() {
  command.reserve(COMMAND_BUFFER_SIZE);

  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println(WELCOME_MESSAGE);
}

void loop() {
  if (commandReceived) {
    processCommand(generateRandomHex(), command);
  }
}

String generateRandomHex() {
  String result = "";
  for (int i = 0; i < 5; i++) {
    int randValue = random(16);
    if (randValue < 10) {
      result += (char)('0' + randValue);
    } else {
      result += (char)('A' + (randValue - 10));
    }
  }
  return result;
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char) Serial.read();
    if (inChar == NEWLINE_CHARACTER || inChar == CARRIAGE_RETURN_CHARACTER) {
      commandReceived = true;
      Serial.println();
    } else {
      command += inChar;
      Serial.print(inChar); // echo characters back
    }
  }
}

void resetCommandBuffer() {
  command = "";
  commandReceived = false;
}

void processCommand(const String &cmdId, const String &cmd) {
  String received = cmd;
  cmd.trim();

  // echo command back
  Serial.println("[" + cmdId + "] Received command: " + cmd);

  // handle command
  if (cmd.startsWith(ON)) {
    setPinStateCmd(cmdId, received, true);
    resetCommandBuffer();
    return;
  }
  if (cmd.startsWith(OFF)) {
    setPinStateCmd(cmdId, received, false);
    resetCommandBuffer();
    return;
  }
  if (cmd.startsWith(STATUS)) {
    printBoardStatusCmd(cmdId);
    resetCommandBuffer();
    return;
  }

  Serial.println("[" + cmdId + "] Invalid command discarded");
  resetCommandBuffer();
}

void setPinStateCmd(const String &cmdId, const String &received, bool state) {
  int pinStartIndex = state ? ON.length() + 1 : OFF.length() + 1;
  int pin = received.substring(pinStartIndex).toInt();
  
  if (pin >= ARDUINO_MIN_PIN && pin <= ARDUINO_MAX_PIN) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state ? HIGH : LOW);
    Serial.print("[" + cmdId + "] Turned ");
    Serial.print(state ? ON : OFF);
    Serial.println(" pin " + String(pin));
  } else {
    Serial.println("[" + cmdId + "] Invalid pin number: " + String(pin));
  }
}

void printBoardStatusCmd(const String &cmdId) {
  for (int i = ARDUINO_MIN_PIN; i <= ARDUINO_MAX_PIN; i++) {
    Serial.println("[" + cmdId + "] Pin " + String(i) + " is " + (digitalRead(i) ? ON : OFF));
  }
}