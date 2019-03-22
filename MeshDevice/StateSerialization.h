#pragma once
#include "Arduino.h"

struct State;

class StateSerialization
{
  public:
    StateSerialization();
    void Init();
    void SerializeState( const State* state );
    bool DeserializeState( State* state );
    void TestEEPROM();
    void TestSerialization();

  private:
    uint16_t WriteString( uint16_t location, const String& contents );
    bool ReadString( uint16_t location, String& buffer );
};

void Assert( bool assertion, const char* message );