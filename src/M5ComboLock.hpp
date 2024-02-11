#pragma once

// clang-format off
#include <SPIFFS.h>
#include <M5Dial.h>
// clang-format on

class M5ComboLock {
public:
    static const bool ENABLE_ENCODER = true;
    static const bool ENABLE_RFID = false;

    static const size_t MAX_DIALS = 10;
    static const uint8_t DIAL_COUNTER_STEP = 4;
    static const unsigned long DIAL_INTERVAL_MS = 1500;
    static const unsigned long DIAL_TIMEOUT_MS = 5000;

    static const uint16_t DIAL_POS_Y_OFFSET = 70;

    static const char* LOCK_ICON_PATH;
    static const char* UNLOCK_ICON_PATH;
    static const uint16_t ICON_POS_X = 90;
    static const uint16_t ICON_POS_Y = 90;

    enum class State
    {
        NOT_ENTERED,
        ENTERING,
        VALID,
    };

    M5ComboLock(void);
    virtual ~M5ComboLock(void);

    virtual bool begin(const int8_t dials[], size_t len,
                       std::function<bool(void)> onValid = nullptr);
    virtual bool update(void);
    virtual bool isRoteted(void) const;
    virtual int32_t getCount(void) const;

    virtual void reset(void);

protected:
    virtual void showDialPosition(long pos);
    virtual void showLock(bool locked);
    virtual void resetDial(void);

private:
    std::function<bool(void)> _onValid;

    int32_t _prevCount;
    unsigned long _prevElapsed;

    size_t _pos;
    size_t _maxPos;
    int8_t _dials[MAX_DIALS];

    State _state;
};
