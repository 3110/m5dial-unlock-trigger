#include "M5BLEKeyboard.hpp"
#include "M5ComboLock.hpp"

M5BLEKeyboard keyboard("M5CombLock");
M5ComboLock comboLock;

const int8_t DIALS[] = {3, -1, 4, -1, 5, -9, 2, -6};
const size_t N_DIALS = sizeof(DIALS) / sizeof(DIALS[0]);

bool onValid(void) {
    M5_LOGI("Sending Ctrl+Alt+Delete...");
    keyboard.press(KEY_LEFT_CTRL);
    keyboard.press(KEY_LEFT_ALT);
    keyboard.press(KEY_DELETE);
    M5.delay(100);
    keyboard.releaseAll();
    return true;
}

bool onConnect(void) {
    return true;
}

bool onDisconnect(void) {
    return true;
}

void setup(void) {
    keyboard.begin(onConnect, onDisconnect);
    comboLock.begin(DIALS, N_DIALS, onValid);
}

void loop(void) {
    keyboard.update();
    comboLock.update();

    M5.delay(1);
}
