#include <iostream>
#include <string>
#include <sstream>

#include "kv/libwiredtiger/include/wiredtiger.h"

class WTCursor {
public:
    WTCursor(WT_CURSOR* cursor) : _cursor(cursor) {}
    void set(const std::string& key, const std::string& value) {
        _cursor->set_key(_cursor, key.c_str());
        _cursor->set_value(_cursor, value.c_str());
        _cursor->insert(_cursor);
    }
    void scan() {
        _cursor->reset(_cursor);
        const char *key, *value;
        int ret;
        while ((ret = _cursor->next(_cursor)) == 0) {
            _cursor->get_key(_cursor, &key);
            _cursor->get_value(_cursor, &value);
    
            std::cout << "[RECORD] " << key << " : " << value << std::endl;
        }
    }
    void close() {
        _cursor->close(_cursor);
    }

private:
    WT_CURSOR* _cursor;
};

class WTSession {
public:
    WTSession(WT_SESSION* session) : _session(session) {}
    void create(const std::string& name) {
        std::stringstream ss;
        ss << "table:" << name;
        _session->create(_session, ss.str().c_str(), "key_format=S,value_format=S");
    }
    WTCursor cursor(const std::string& name) {
        std::stringstream ss;
        ss << "table:" << name;
        WT_CURSOR *cursor;
        _session->open_cursor(_session, ss.str().c_str(), NULL, NULL, &cursor);
        return WTCursor(cursor);
    }
private:
    WT_SESSION* _session;
};

class WTConnection {
public:
    WTConnection(const std::string& home) {
        wiredtiger_open(home.c_str(), NULL, "create", &_conn);
    }
    WTSession getSession() {
        WT_SESSION* session;
        _conn->open_session(_conn, NULL, NULL, &session);
        return WTSession(session);
    }
private:
    WT_CONNECTION* _conn;
};

int main() {
    WTConnection conn("./data");
    auto session = conn.getSession();
    session.create("tyler");
    auto cursor = session.cursor("tyler");
    cursor.set("name", "tyler");
    cursor.scan();
    cursor.close();
}