# Mesh Lights

## Mesh Device

### Dependencies

Install the following from the library manager, latest versions:
- CRC32
- NeoPixelBus
- ArduinoJson

Download and install the following libraries:
- [PainlessMesh](https://gitlab.com/BlackEdder/painlessMesh)
- [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [ESPAsyncUDP](https://github.com/me-no-dev/ESPAsyncUDP) - note, this is not used yet, but will be needed for using Open Pixel Control (OPC).

Note, ESPAsyncUDP requires an extremely specific version of the ESP8266 Arduino board library. Details here:
https://github.com/me-no-dev/ESPAsyncUDP/issues/4#issuecomment-354225831

Once OPC support is integrated, details will be added here for installation.

## Mesh Device Dimmer Driver

### Dependencies

Install the following from the library manager, latest versions:
- CRC32

Download and install the following libraries:
- [Dimmer](https://github.com/circuitar/Dimmer)

Note, in order to change the interrupt pin for the TRIAC zero crossing detector, you have to modify Dimmer.h in the above library. This should work by default for the PCB as designed now.s