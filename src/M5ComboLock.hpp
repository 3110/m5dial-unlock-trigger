#pragma once

// clang-format off
#include <LittleFS.h>
#include <M5Dial.h>
// clang-format on

class M5ComboLock {
public:
    typedef int32_t count_t;

    static const bool ENABLE_ENCODER = true;
    static const bool ENABLE_RFID = false;

    static const size_t MAX_DIALS = 10;
    static const count_t DIAL_COUNT_STEP = 4;
    static const unsigned long DIAL_INTERVAL_MS = 1500;
    static const unsigned long DIAL_TIMEOUT_MS = 3000;

    static const int DIAL_RIGHT_COLOR = GREEN;
    static const int DIAL_LEFT_COLOR = DIAL_RIGHT_COLOR;
    static const uint16_t DIAL_COUNT_POS_Y_OFFSET = 50;

    static const char* LOCK_ICON_PATH;
    static const char* UNLOCK_ICON_PATH;
    static const uint16_t ICON_WIDTH = 64;
    static const uint16_t ICON_HEIGHT = 64;
    static const uint16_t ICON_POS_X = 90;
    static const uint16_t ICON_POS_Y = 90;

    enum class State
    {
        NOT_ENTERED,
        ENTERING,
        VALID,
    };

    M5ComboLock(const lgfx::GFXfont& font = fonts::Orbitron_Light_32);
    virtual ~M5ComboLock(void);

    virtual bool begin(const int8_t dials[], size_t len,
                       std::function<bool(void)> onValid = nullptr);
    virtual bool update(void);

    virtual bool isRoteted(void) const;
    virtual count_t getCount(void) const;

    virtual void reset(void);

protected:
    virtual void showDialCount(count_t count);
    virtual void showLockIcon(bool locked);
    virtual void resetDial(void);

private:
    std::function<bool(void)> _onValid;

    count_t _prevCount;
    unsigned long _prevElapsed;

    size_t _pos;
    size_t _maxPos;
    int8_t _dials[MAX_DIALS];

    State _state;

    const lgfx::GFXfont* _count_font;
    M5Canvas _canvas;
};
