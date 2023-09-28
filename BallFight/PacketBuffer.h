#pragma once

#include "framework.h"

#define BUFFER_SIZE     128

class PacketBuffer
{
public:
    PacketBuffer() {}
    ~PacketBuffer() {}

public:
    template<typename TYPE> void Append(TYPE data)
    {
        if (m_index + sizeof(TYPE) >= BUFFER_SIZE) return;
        memcpy(&m_buffer[m_index], &data, sizeof(TYPE));
        m_index += sizeof(TYPE);
    }
    template<typename TYPE> void Read(TYPE& data)
    {
        if (m_index + sizeof(TYPE) >= BUFFER_SIZE) return;
        TYPE read;
        memcpy(&read, &m_buffer[m_index], sizeof(TYPE));
        m_index += sizeof(TYPE);
        data = read;
    }

    char* GetBuffer()
    {
        return m_buffer;
    }


private:
    char m_buffer[BUFFER_SIZE];
    int m_index = 0; // m_index + 1 is size of the packet! // TODO: rename => cursor
};
