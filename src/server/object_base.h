#pragma once

#include <cstdint>
#include <functional>

typedef uint64_t OID;

class ObjectServer;

class ObjectBase{
private:
    static OID m_next_id;
    static OID fetch_id();

    const OID m_id;
    bool m_is_alive = true;
    bool m_is_ready = false;

    virtual void _(){}
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
