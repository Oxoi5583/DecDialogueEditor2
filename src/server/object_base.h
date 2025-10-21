#pragma once

#include <cstdint>
#include <functional>

typedef uint64_t OID;

class ObjectServer;

class ObjectBase{
private:
    OID m_next_id = 100;
    OID fetch_id();

    const OID m_id;
    bool m_is_alive = true;
    bool m_is_ready = false;
protected:

public:
    ObjectBase()
    : m_id(fetch_id()){}
    ~ObjectBase(){};

    OID get_id() const;
    bool is_alive() const;
    bool is_ready() const;

    void queue_free();

    friend class ObjectServer;
};
