#include "OpenPixelControl.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "Global.h"
WiFiUDP gUdp;
const unsigned int OPC_PORT = 7890;
char rxBuffer[255];

OpenPixelControl::OpenPixelControl(StateManager* state_manager, std::function<void(uint8_t*, uint16_t)> render_callback) : mStateManager(state_manager), mRenderCallback(render_callback) {}

void OpenPixelControl::Init()
{
    /*if( WiFi.status() != WL_CONNECTED )
    {
        //if regular wifi is not connected, don't activate the OPC UDP server at all.
        return; 
    }*/
    
    mEnabled = true;
    gUdp.begin(OPC_PORT);
}

void OpenPixelControl::Update()
{
    if( !mEnabled )
        return;

    int packet_size = gUdp.parsePacket();

    if (packet_size <= 0)
        return;

    //Serial.printf("Received %d bytes from %s, port %d\n", packet_size, gUdp.remoteIP().toString().c_str(), gUdp.remotePort());

    int len = gUdp.read(rxBuffer, 255);
    if (len > 0)
    {
        rxBuffer[len] = 0;
    }

    OpcPacket packet = OpcPacket::ParsePacket((uint8_t*)rxBuffer, len);

    //packet.Print();

    if( packet.mLength == 0 )
        return;
    
    uint8_t dimmer_buffer[MAX_DIMMER_COUNT];
    for( auto& x : dimmer_buffer)
    {
        x = 0; //clear
    }

    auto dimmer_count = GetLuminanceFromPacket(&packet, dimmer_buffer, MAX_DIMMER_COUNT);

    if( dimmer_count == 0 )
        return;

    //Serial.printf("dimmer_count %d\n", dimmer_count);
    for( int i = 0; i < dimmer_count; ++i)
    {
        //Serial.printf("dim[%d] = %d\n", i, dimmer_buffer[i]);
    }

    mRenderCallback(dimmer_buffer, dimmer_count);
}

OpcPacket OpcPacket::ParsePacket( uint8_t* buffer, int count)
{
    OpcPacket packet;

    if(count < 4)
        return packet;
    packet.mChannel = *buffer++;
    packet.mCommand = *buffer++;
    packet.mLength = (*buffer++) << 8;
    packet.mLength |= *buffer++;
    packet.mPayload = buffer;
    return packet;
}

void OpcPacket::Print()
{
    if( mLength == 0 )
    {
        Serial.println("empty OPC packet");
        return;
    }
    Serial.printf("channel %d\n", mChannel);
    Serial.printf("command %d\n", mCommand);
    Serial.printf("length %d\n", mLength);
}

uint8_t RgbToLuminance(uint8_t r, uint8_t g, uint8_t b)
{
    //basically green = bright.

    //Wikipedia & stack overflow: 
    //https://en.wikipedia.org/wiki/Relative_luminance  
    //https://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color
    float luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    if(luminance > 255 )
        luminance = 255;
    else if( luminance < 0)
        luminance = 0;

    return (uint8_t)luminance;
}

uint16_t GetLuminanceFromPacket(const OpcPacket* packet, uint8_t* output, uint16_t buffer_size )
{
    uint16_t color_count = packet->mLength / 3;
    if( color_count > buffer_size )
        color_count = buffer_size;
    
    for(int i = 0; i < color_count; ++i)
    {
        uint8_t* color = packet->mPayload + (i*3);
        output[i] = RgbToLuminance(*color, *(color+1), *(color+2));
    }

    return color_count;
}