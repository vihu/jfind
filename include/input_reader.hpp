#ifndef INPUT_READER_HPP
#define INPUT_READER_HPP

#include "key.hpp"
#include "event_dispatch.hpp"
#include "mouse_event.hpp"
#include "logger.hpp"
#include "cancellable_reader.hpp"

#include <string>
#include <vector>
#include <thread>

extern "C" {
#include <sys/select.h>
}

bool isContinuationByte(unsigned char c);
int utf8CharLen(unsigned char c);

class InputReader : public EventListener {
public:
    InputReader();

    bool getKey(Key *key);
    void setFileDescriptor(int fd);

    void onLoop() override;
    void onStart() override;
    void preOnEvent(EventType type) override;
    void onEvent(std::shared_ptr<Event> event) override;

    void onSigInt();

private:
    EventDispatch& m_dispatch = EventDispatch::instance();
    Logger m_logger = Logger("InputReader");

    CancellableReader m_reader;

    // payload for events which require more than just
    // the Key enum value
    char m_widechar[4];
    std::vector<MouseEvent> m_mouseEvents;

    std::chrono::time_point<std::chrono::system_clock> m_lastClickTime;
    int m_clickCount = 0;
    MouseButton m_lastClickButton = MB_NONE;

    bool getch(char *c);
    bool parseEsc(Key *key);
    void parseAltKey(char c, Key *key);
    void parseMouse(std::string& seq, Key *key);
    void parseEscSeq(std::string& seq, Key *key);
    bool parseUtf8(char c, Key *key);
};

#endif
