#include "M5ComboLock.hpp"

static const bool FORMAT_SPIFFS_IF_FAILED = true;
const char* M5ComboLock::LOCK_ICON_PATH = "/lock.png";
const char* M5ComboLock::UNLOCK_ICON_PATH = "/unlock.png";

M5ComboLock::M5ComboLock(void)
    : _onValid(nullptr),
      _prevPos(LONG_MIN),
      _prevElapsed(0),
      _pos(0),
      _maxPos(0),
      _dials{0},
      _state(State::NOT_ENTERED) {
}

M5ComboLock::~M5ComboLock(void) {
}

void M5ComboLock::showDialPosition(long pos) {
    M5Dial.Display.clear();
    M5Dial.Display.drawString(String(pos), M5Dial.Display.width() / 2,
                              M5Dial.Display.height() / 2 + DIAL_POS_Y_OFFSET);
}

bool M5ComboLock::begin(const int8_t dials[], size_t len,
                        std::function<bool(void)> onValid) {
    if (len > MAX_DIALS) {
        return false;
    }
    this->_maxPos = len;
    for (size_t p = 0; p < this->_maxPos; ++p) {
        this->_dials[p] = dials[p];
    }
    this->_onValid = onValid;

    M5Dial.begin(ENABLE_ENCODER, ENABLE_RFID);

    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
        M5_LOGE("Failed to mount SPIFFS");
        return false;
    }

    M5Dial.Display.setTextColor(GREEN);
    M5Dial.Display.setTextDatum(middle_center);
    M5Dial.Display.setFont(&fonts::Orbitron_Light_32);
    M5Dial.Display.setTextSize(1);

    return true;
}

bool M5ComboLock::update(void) {
    const long curPos = M5Dial.Encoder.read();
    const unsigned long elapsed = millis();
    unsigned long interval = elapsed - this->_prevElapsed;

    if (curPos != this->_prevPos) {
        this->_prevPos = curPos;
        showDialPosition(curPos);
        this->_prevElapsed = elapsed;
    } else {
        if (interval > DIAL_TIMEOUT_MS) {
            M5_LOGE("Timeout");
            reset();
        } else if (interval > DIAL_INTERVAL_MS) {
            if (this->_state == State::NOT_ENTERED) {
                if (this->_dials[this->_pos] == curPos) {
                    if (this->_pos + 1 == this->_maxPos) {
                        this->_state = State::VALID;
                        M5_LOGD("State: Valid");
                    } else {
                        this->_state = State::ENTERING;
                        M5_LOGD("State: Entering");
                        ++this->_pos;
                        M5_LOGD("Matched: %d/%d", this->_pos, this->_maxPos);
                    }
                    resetDial();
                }
            } else if (this->_state == State::ENTERING) {
                if (this->_dials[this->_pos] == curPos) {
                    if (this->_pos + 1 == this->_maxPos) {
                        this->_state = State::VALID;
                        if (this->_onValid) {
                            this->_onValid();
                        }
                        M5_LOGD("State: Valid");
                    } else {
                        ++this->_pos;
                        M5_LOGD("Matched: %d/%d", this->_pos, this->_maxPos);
                    }
                    resetDial();
                } else {
                    M5_LOGE("Invalid");
                    reset();
                }
            } else if (this->_state == State::VALID) {
                M5_LOGI("OK");
                reset();
            }
        }
    }

    if (this->_state == State::VALID) {
        showLock(false);
    } else {
        showLock(true);
    }

    return true;
}

void M5ComboLock::reset(void) {
    this->_state = State::NOT_ENTERED;
    this->_pos = 0;
    resetDial();
}

void M5ComboLock::resetDial(void) {
    M5Dial.Encoder.readAndReset();
    this->_prevElapsed = millis();
    this->_prevPos = M5Dial.Encoder.read();
    showDialPosition(this->_prevPos);
}

void M5ComboLock::showLock(bool locked) {
    M5Dial.Display.drawPngFile(SPIFFS,
                               locked ? LOCK_ICON_PATH : UNLOCK_ICON_PATH,
                               ICON_POS_X, ICON_POS_Y);
}
