#pragma once

#include <cstdint>
#include <functional>

typedef uint64_t OID;

class ObjectServer;

class ObjectBase{
private:
    int m_layer;
    static OID m_next_id;
    static OID fetch_id();

    const OID m_id;
    bool m_is_alive = true;
    bool m_is_ready = false;
    bool m_is_freeze = false;

    virtual void _(){}
protected:

public:
    ObjectBase()
    : m_id(fetch_id()){}
    virtual ~ObjectBase(){};

    OID get_id() const;
    bool is_alive() const;
    bool is_ready() const;
    bool is_freeze() const;

    void set_freeze(bool p_value);

    void queue_free();

    void set_layer(int p_layer);
    int get_layer();

    friend class ObjectServer;
};
