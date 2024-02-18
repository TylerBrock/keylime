#pragma once

#include <iostream>
#include <string>
#include <sstream>

#include "kv/libwiredtiger/include/wiredtiger.h"

namespace wiredtiger {

class Cursor {
public:
    Cursor(WT_CURSOR* cursor) : _cursor(cursor) {}
    Cursor(Cursor&& rhs) : _cursor(nullptr) {
        _cursor = rhs._cursor;
        rhs._cursor = nullptr;
    }
    Cursor(const Cursor&) = delete;
    void operator=(Cursor) = delete;
    void operator=(Cursor&&) = delete;
    ~Cursor() { if (_cursor) { close(); } }
    void set(const std::string& key, const std::string& value) {
        _cursor->set_key(_cursor, key.c_str());
        _cursor->set_value(_cursor, value.c_str());
        _cursor->insert(_cursor);
    }
    void unset(const std::string& key) {
        seek(key);
        _cursor->remove(_cursor);
    }
    std::string get(const std::string& key) {
        const char *value;
        auto ret = seek(key);
        if (ret == WT_NOTFOUND) {
            return "";
        }
        _cursor->get_value(_cursor, &value);
        return value;
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
    int seek(const std::string& key) {
        _cursor->reset(_cursor);
        _cursor->set_key(_cursor, key.c_str());
        return _cursor->search(_cursor);
    }
    WT_CURSOR* _cursor;
};

class Session {
public:
    Session(WT_SESSION* session) : _session(session) {}
    Session(Session&& rhs) : _session(nullptr) {
        _session = rhs._session;
        rhs._session = nullptr;
    }
    Session(const Session&) = delete;
    void operator=(Session) = delete;
    void operator=(Session&&) = delete;
    ~Session() { _session->close(_session, nullptr); }
    static std::string tableName(const std::string& name) {
        std::stringstream ss;
        ss << "table:" << name;
        return ss.str();
    }
    void create(const std::string& name) {
        _session->create(_session, tableName(name).c_str(), "key_format=S,value_format=S");
    }
    void drop(const std::string& name) {
        _session->drop(_session, tableName(name).c_str(), "force=true");
    }
    void begin_transaction() {
        int ret = _session->begin_transaction(_session, nullptr);
        std::cout << "[TXN] started transaction: " << ret << std::endl;
    }
    void commit_transaction() {
        int ret = _session->commit_transaction(_session, nullptr);
        std::cout << "[TXN] committed transaction: " << ret << std::endl;
    }
    void rollback_transaction() {
        int ret = _session->rollback_transaction(_session, nullptr);
        std::cout << "[TXN] rolled back transaction: " << ret << std::endl;
    }
    void checkpoint() {
        int ret = _session->checkpoint(_session, nullptr);
        std::cout << "[CHK] checkpointed: " << ret << std::endl;
    }
    Cursor cursor(const std::string& name) {
        WT_CURSOR *cursor;
        _session->open_cursor(_session, tableName(name).c_str(), nullptr, nullptr, &cursor);
        return Cursor(cursor);
    }
private:
    WT_SESSION* _session;
};

class Connection {
public:
    Connection(const std::string& home) {
        wiredtiger_open(home.c_str(), nullptr, "create,log=(enabled)", &_conn);
    }
    ~Connection() { _conn->close(_conn, nullptr); }

    Connection(const Connection&) = delete;
    Connection(const Connection&&) = delete;
    void operator=(const Connection&) = delete;
    void operator=(const Connection&&) = delete;
    std::string getHome() { return _conn->get_home(_conn); }
    Session getSession() {
        WT_SESSION* session;
        _conn->open_session(_conn, nullptr, nullptr, &session);
        return Session(session);
    }
private:
    WT_CONNECTION* _conn;
};

}