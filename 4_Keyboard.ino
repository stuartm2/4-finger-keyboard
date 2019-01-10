
#include <Keyboard.h>

const int BTN_PINS[] = {6, 7, 8, 9};
const int MODE_LED_PINS[] = {10, 15, 14, 16};
const int LOCK_LED_PINS[] = {5, 2, 3, 4};
const int MODE_NORMAL = 0;
const int MODE_CTRL = 1;
const int MODE_SHIFT = 2;
const int MODE_NUM = 3;
const int MODE_LANG = 4;

const String MAP[28] = {
//  " ", "n",   "p",   "j",   "l",   "i",   "w",
    "8", "8c8", "8a8", "898", "8c4", "8a2", "891",
//  "a", "m",   "f",   "g",   "o",   "h",   "c",
    "4", "4c4", "464", "454", "4c8", "462", "451",
//  "e", "r",   "s",   "x",   "y",   "t",   "v",
    "2", "2a2", "262", "232", "2a8", "264", "231",
//  "_", "b",   "k",   "z",   "u",   "d",   "q"
    "1", "191", "151", "131", "198", "154", "132"
};

const String NORMAL = " npjliwamfgohcersxytv_bkzudq";
const String SHIFT  = "_NPJLIWAMFGOHCERSXYTV_BKZUDQ";
const String NUM    = " *\\-139_+,&257=/.#46^_%|$08@";
const String LANG   = " ñpjlïwámfgóhçérsxýtv_bkzúdq";
const String PROG   = " *\\-{[<?+,&}?~_/.#]`^_%|$>£@";
const String CTRL   = "_:?^^^^_;,^^^^_!.(^^^_'\")^^^";

String buf = "";
int releaseNext = 0;
int pressed = 0;
int mode = MODE_NORMAL;
bool lock = false;

bool debounce(int pin) {
  return true;
}

void processBuffer(String buf) {
  int index = -1;

  for (int i = 0; i < 28; i += 1) {
    if (MAP[i] == buf) {
      index = i;
      break;
    }
  }

  if (index >= 0) {
    char c = "";

    switch (mode) {
      case MODE_NORMAL:
        c = NORMAL.charAt(index);
        break;

      case MODE_CTRL:
        c = CTRL.charAt(index);
        break;

      case MODE_SHIFT:
        c = SHIFT.charAt(index);
        break;

      case MODE_NUM:
        c = NUM.charAt(index);
        break;

      case MODE_LANG:
        c = PROG.charAt(index);
        break;
    }

    if (c == 95) { // '_' indicates mode change
      if (!lock) {
        if (mode == MODE_NORMAL) {
          setMode(MODE_CTRL, false);
        } else if (mode == MODE_CTRL) {
          if (buf == "8") {
            setMode(MODE_SHIFT, false);
          } else if (buf == "4") {
            setMode(MODE_NUM, false);
          } else if (buf == "2") {
            setMode(MODE_LANG, false);
          } else if (buf == "1") {
            setMode(mode, true);
          }
        } else if (mode == MODE_SHIFT) {
          if (buf == "8") {
            setMode(mode, true);
          } else {
            setMode(MODE_CTRL, false);
          }
        } else if (mode == MODE_NUM) {
          if (buf == "4") {
            setMode(mode, true);
          } else if (buf == "1") {
            setMode(MODE_CTRL, false);
          }
        } else if (mode == MODE_LANG) {
          if (buf == "2") {
            setMode(mode, true);
          } else if (buf == "1") {
            setMode(MODE_CTRL, false);
          }
        }
      }
    } else if (c == 94 && mode == MODE_CTRL) { // '^' indicates special character
      if (buf == "132") { // up
        Keyboard.write(KEY_UP_ARROW);
      } else if (buf == "154") { // left
        Keyboard.write(KEY_LEFT_ARROW);
      } else if (buf == "198") { // home
        Keyboard.write(KEY_HOME);
      } else if (buf == "231") { // down
        Keyboard.write(KEY_DOWN_ARROW);
      } else if (buf == "264") { // backsp
        Keyboard.write(KEY_BACKSPACE);
      } else if (buf == "2a8") { // ctrl l?
        pressKey(KEY_LEFT_CTRL);
      } else if (buf == "451") { // right
        Keyboard.write(KEY_RIGHT_ARROW);
      } else if (buf == "454") { // alt 2?
        pressKey(KEY_RIGHT_ALT);
      } else if (buf == "462") { // del
        Keyboard.write(KEY_DELETE);
      } else if (buf == "4c8") { // nl
        Keyboard.write(KEY_RETURN);
      } else if (buf == "891") { // end
        Keyboard.write(KEY_END);
      } else if (buf == "898") { // alt 1?
        pressKey(KEY_LEFT_ALT);
      } else if (buf == "8a2") { // ctrl r?
        pressKey(KEY_RIGHT_CTRL);
      } else if (buf == "8c4") { // tab
        Keyboard.write(KEY_TAB);
      }

      if (!lock) {
        setMode(MODE_NORMAL, false);
      }
    } else if (c == 63 && mode == MODE_LANG) { // '?' indicates special character
      if (buf == "4") { //
        Keyboard.write(KEY_RETURN);
      } else if (buf == "462") { //
        Keyboard.write("_");
      }
    } else {
      Keyboard.write(c);

      if (!lock) {
        setMode(MODE_NORMAL, false);
      }
    }
    
    if (releaseNext == 1) {
      Keyboard.release(pressed);
      pressed = 0;
      releaseNext = 0;
    } else if (releaseNext == 2) {
      releaseNext = 1;
    }
  }
}

void setMode(int setMode, bool setLock) {
  mode = setMode;
  lock = setLock;

  for (int i = 0; i < 4; i += 1) {
    if (mode == i + 1) {
      digitalWrite(LOCK_LED_PINS[i], lock);
      digitalWrite(MODE_LED_PINS[i], !lock);
    } else {
      digitalWrite(LOCK_LED_PINS[i], LOW);
      digitalWrite(MODE_LED_PINS[i], LOW);
    }
  }
}

void pressKey(int key) {
  if (releaseNext > 0 && key == pressed) {
    Keyboard.release(key);
    pressed = 0;
    releaseNext = 0;
  } else {
    Keyboard.press(key);
    pressed = key;
    releaseNext = 2;
  }
}

void setup() {
  int flashDur = 100;

  for (int i = 0; i < 4; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
    pinMode(MODE_LED_PINS[i], OUTPUT);
    pinMode(LOCK_LED_PINS[i], OUTPUT);
  }

  for (int i = 1; i < 4; i += 1) {
    digitalWrite(MODE_LED_PINS[i], HIGH);
    delay(flashDur);
    digitalWrite(MODE_LED_PINS[i], LOW);
  }

  digitalWrite(MODE_LED_PINS[0], HIGH);
  delay(flashDur);
  digitalWrite(MODE_LED_PINS[0], LOW);

  for (int i = 1; i < 4; i += 1) {
    digitalWrite(LOCK_LED_PINS[i], HIGH);
    delay(flashDur);
    digitalWrite(LOCK_LED_PINS[i], LOW);
  }

  digitalWrite(LOCK_LED_PINS[0], HIGH);
  delay(flashDur);
  digitalWrite(LOCK_LED_PINS[0], LOW);
}

void loop() {
  int btnsState = 0;

  for (int i = 0; i < 4; i += 1) {
    int btn = BTN_PINS[i];

    if (digitalRead(btn) == LOW && debounce(btn)) {
      btnsState += 1 << i;
    }
  }

  // Keys released; end of sequence
  if (btnsState == 0 && buf != "") {
    processBuffer(String(buf));
    buf = "";
  }

  // 3 keys pressed; set mode to 'normal'
  else if (btnsState == 7 || btnsState == 11 || btnsState == 13 ||
      btnsState == 14 || btnsState == 15) {
    setMode(MODE_NORMAL, false);
    buf = "";
    delay(250);
  }

  // Append new button states to the buffer
  else if (btnsState != 0 && !buf.endsWith(String(btnsState, HEX))) {
    buf += String(btnsState, HEX);
  }
}

