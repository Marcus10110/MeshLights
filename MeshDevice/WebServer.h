#pragma once

#include "State.h"

#include <Arduino.h>

class AsyncWebServer;
class IPAddress;
class AsyncWebServerRequest;


class WebServer
{
    public:
        WebServer(StateManager* state_manager);
        void Init();
        void Update();

    private:
        void OnIndex(AsyncWebServerRequest* request);
        void OnSetDeviceType(AsyncWebServerRequest* request);
        void OnSetLedCount(AsyncWebServerRequest* request);
        void OnSetBrightness(AsyncWebServerRequest* request);
        void OnSetGroup(AsyncWebServerRequest* request);
        void OnSetName(AsyncWebServerRequest* request);

        AsyncWebServer* mServer; //not owned
        IPAddress* mIp; //not owned
        StateManager* mStateManager; //not owned.

        //thread crossing bits.
        volatile device_type_t mNewDeviceType;
        volatile bool mNewDeviceTypeFlag{false};

        volatile uint16_t mNewLedCount;
        volatile bool mNewLedCountFlag{false};

        volatile uint8_t mNewBrightness;
        volatile bool mNewBrightnessFlag{false};

        volatile uint8_t mNewGroup;
        volatile bool mNewGroupFlag{false};

        String mNewName;
        volatile bool mNewNameFlag{false};
};

class TableHelper
{
public:
    TableHelper();
    void AddRow(const String& r1);
    void AddRow(const String& r1, const String& r2);
    String ToString();
    void Clear();
private:
    String mWorkingString;
};

class FormHelper
{
public:
    FormHelper();
    void AddLabel(const String& label);
    void AddTextBox(const String& name, const String& existing_value);
    void AddRatio(const String& name, const String& value, const String& contents, bool checked);
    void AddNumberInput(const String& name, int min_value, int max_value, int existing_value);
    void AddSubmitButton();
    String ToString(const String& action);
    void Clear();
private:
    String mWorkingString;
};