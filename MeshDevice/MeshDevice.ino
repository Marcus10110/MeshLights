#include "State.h"
#include "WebServer.h"
#include "Mesh.h"
#include "Button.h"
#include "LedStrip.h"
#include "SerialDimmer.h"
#include "Global.h"
#include "StateSerialization.h"

void OnStateChanged(const State* state);
uint32_t GetMeshTimeMs(); 
StateManager gStateManager(OnStateChanged);
WebServer gWebServer(&gStateManager);
Mesh gMesh;
Button gButton(&gStateManager, BUTTON_PIN);
LedStrip gLedStrip(MAX_LED_COUNT, LEDPIN, &gStateManager, GetMeshTimeMs);
SerialDimmer gSerialDimmer(MAX_DIMMER_COUNT, &gStateManager, GetMeshTimeMs);
StateSerialization gStateSerialization;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("start");

  gMesh.Init();
  gWebServer.Init();
  gButton.Init();
  gLedStrip.Init();
  gSerialDimmer.Init();
  
  Serial.println("init finished");
}

void loop() {
  // put your main code here, to run repeatedly:
  gMesh.Update();
  gWebServer.Update();
  gButton.Update();
  uint32_t time = millis();
  gStateManager.TimeUpdate(time);

  //gStateSerialization.SerializeState(gStateManager.GetState());
  gStateSerialization.TestEEPROM();
}

void OnStateChanged(const State* state)
{
  Serial.println("state changed");
}

uint32_t GetMeshTimeMs()
{
  return gMesh.GetTimeMs();
}
