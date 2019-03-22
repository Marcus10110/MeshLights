#include "Button.h"
#include "Global.h"
#include "LedStrip.h"
#include "Mesh.h"
#include "OpenPixelControl.h"
#include "SerialDimmer.h"
#include "State.h"
#include "WebServer.h"
#include "WifiSecrets.h"
#include <ESP8266WiFi.h>

const char* WifiSsid = WIFI_SSID;
const char* WifiPassword = WIFI_PASSWORD;

void OnStateChanged( const State* state );
uint32_t GetMeshTimeMs();
StateManager gStateManager( OnStateChanged );
WebServer gWebServer( &gStateManager );
Mesh gMesh;
Button gButton( &gStateManager, BUTTON_PIN );
LedStrip gLedStrip( MAX_LED_COUNT, LEDPIN, &gStateManager, GetMeshTimeMs );
SerialDimmer gSerialDimmer( MAX_DIMMER_COUNT, &gStateManager, GetMeshTimeMs );
OpenPixelControl gOpenPixelControl( &gStateManager,
                                    [&gSerialDimmer]( uint8_t* values, uint16_t count ) { gSerialDimmer.UpdateFromOpc( values, count ); } );

// cache connection_mode_t at bootup.
connection_mode_t gConnectionMode;

void setup()
{
    // put your setup code here, to run once:
    delay( 250 );
    Serial.begin( 115200 );
    delay( 1000 );
    Serial.println( "start" );
    gStateManager.Init();

    // Init Mesh or normal WIFI
    gConnectionMode = gStateManager.GetState()->mConnectionMode;
    if( gConnectionMode == connection_mode_t::MESH )
    {
        Serial.println( "Using Mesh" );
        gMesh.Init();
    }
    else
    {
        Serial.println( "Using Wifi" );
        WiFi.begin( WifiSsid, WifiPassword );
        uint32_t wifi_waiting_counter = 0;
        const uint32_t wifi_max_wait = 10000; // 10 seconds.
        while( WiFi.status() != WL_CONNECTED )
        {
            if( wifi_waiting_counter >= wifi_max_wait )
            {
                // lets fall back to Mesh mode.
                Serial.println( "wifi failed to connect after 10 seconds, falling back to mesh" );
                gStateManager.SetConnectionMode( connection_mode_t::MESH );
                ESP.reset();
            }
            delay( 500 );
            wifi_waiting_counter += 500;
            Serial.print( "." );
        }
        Serial.print( "Connected, IP address: " );
        Serial.println( WiFi.localIP() );
    }
    gWebServer.Init();
    gButton.Init();
    gLedStrip.Init();
    gSerialDimmer.Init();
    gOpenPixelControl.Init();

    Serial.println( "init finished" );
}

void loop()
{
    // put your main code here, to run repeatedly:
    // Handle Mesh or network:
    if( gConnectionMode == connection_mode_t::MESH )
    {
        gMesh.Update();
    }
    gWebServer.Update();
    gButton.Update();
    uint32_t time = millis();
    gStateManager.TimeUpdate( time );
    gOpenPixelControl.Update();
}

void OnStateChanged( const State* state )
{
    Serial.println( "state changed" );
}

uint32_t GetMeshTimeMs()
{
    return gMesh.GetTimeMs();
}
