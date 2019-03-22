# Mesh Lights

## Mesh Device

### Dependencies

Install the following from the library manager, latest versions:
- CRC32
- NeoPixelBus
- ArduinoJson
- PainlessMesh
- TaskScheduler

Download and install the following libraries:
- [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [ESPAsyncUDP](https://github.com/me-no-dev/ESPAsyncUDP) - note, this is not used yet, but will be needed for using Open Pixel Control (OPC).


Once OPC support is integrated, details will be added here for installation.

## Mesh Device Dimmer Driver

### Dependencies

Install the following from the library manager, latest versions:
- CRC32

Download and install the following libraries:
- [Dimmer](https://github.com/circuitar/Dimmer)

Note, in order to change the interrupt pin for the TRIAC zero crossing detector, you have to modify Dimmer.h in the above library. This should work by default for the PCB as designed now.s