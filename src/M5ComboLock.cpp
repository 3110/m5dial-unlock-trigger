#include "M5ComboLock.hpp"

static const bool FORMAT_SPIFFS_IF_FAILED = true;
const char* M5ComboLock::LOCK_ICON_PATH = "/lock.png";
const char* M5ComboLock::UNLOCK_ICON_PATH = "/unlock.png";

M5ComboLock::M5ComboLock(const lgfx::GFXfont& font)
    : _onValid(nullptr),
      _prevCount(LONG_MIN),
      _prevElapsed(0),
      _pos(0),
      _maxPos(0),
      _dials{0},
      _state(State::NOT_ENTERED),
      _count_font(&font),
      _canvas(&M5Dial.Display) {
}

M5ComboLock::~M5ComboLock(void) {
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

    reset();

    return true;
}

bool M5ComboLock::update(void) {
    const int32_t count = getCount();
    const unsigned long elapsed = millis();
    unsigned long interval = elapsed - this->_prevElapsed;

    if (isRoteted()) {
        this->_prevCount = count;
        showDialCount(count);
        this->_prevElapsed = elapsed;
    } else {
        if (interval > DIAL_TIMEOUT_MS) {
            M5_LOGE("Timeout");
            reset();
        } else if (interval > DIAL_INTERVAL_MS) {
            if (this->_state == State::NOT_ENTERED) {
                if (this->_dials[this->_pos] == count) {
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
                if (this->_dials[this->_pos] == count) {
                    if (this->_pos + 1 == this->_maxPos) {
                        this->_state = State::VALID;
                        showLockIcon(false);
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
    return true;
}

bool M5ComboLock::isRoteted(void) const {
    return getCount() != this->_prevCount;
}

M5ComboLock::count_t M5ComboLock::getCount(void) const {
    return ((M5Dial.Encoder.read() + DIAL_COUNT_STEP) / DIAL_COUNT_STEP) - 1;
}

void M5ComboLock::reset(void) {
    this->_state = State::NOT_ENTERED;
    this->_pos = 0;
    showLockIcon(true);
    resetDial();
}

void M5ComboLock::showDialCount(count_t count) {
    const String s = String(count);
    const int32_t w = M5.Display.width();
    const int32_t h = M5.Display.fontHeight(this->_count_font);
    this->_canvas.createSprite(w, h);
    this->_canvas.setFont(this->_count_font);
    this->_canvas.setTextSize(1);
    this->_canvas.setTextColor(GREEN);
    this->_canvas.clear(TFT_BLACK);
    this->_canvas.drawCenterString(s, M5Dial.Display.width() / 2, 0,
                                   this->_count_font);
    this->_canvas.pushSprite(
        0, M5Dial.Display.height() / 2 + DIAL_COUNT_POS_Y_OFFSET);
    this->_canvas.deleteSprite();
}

void M5ComboLock::showLockIcon(bool locked) {
    this->_canvas.createSprite(ICON_WIDTH, ICON_HEIGHT);
    this->_canvas.clear(TFT_BLACK);
    this->_canvas.drawPngFile(SPIFFS,
                              locked ? LOCK_ICON_PATH : UNLOCK_ICON_PATH);
    this->_canvas.pushSprite(ICON_POS_X, ICON_POS_Y);
    this->_canvas.deleteSprite();
}

void M5ComboLock::resetDial(void) {
    M5Dial.Encoder.readAndReset();
    this->_prevElapsed = millis();
    this->_prevCount = getCount();
    showDialCount(this->_prevCount);
}
