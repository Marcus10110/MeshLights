#pragma once

#include <Arduino.h>

class Mesh
{
public:
    Mesh();
    void Init();
    void Update();
    uint32_t GetTimeMs();
private:

    void OnReceived( uint32_t from, String &msg );
    void OnNewConnection(uint32_t nodeId);
    void OnChangedConnection();
    void OnNodeTimeAdjusted(int32_t offset);

};