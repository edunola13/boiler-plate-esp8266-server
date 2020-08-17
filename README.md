# Bolierplate ESP8266 Server
Bolierplate para crear aplicaciones de server con el ESP8266. Caracteristicas:
- Server API REST
- MQTT
- Manejo de configuraciones
- Manejo de sensores y actuadores
- Guardado en EEPROM

## Architecture
The idea of the architecture below is that you make that you wish but if the boiler plate is updated with some feature the application based on can be easily updated.

### Main
Here you only need imports your modules and update constants.
Its important update DEVICE_ID.

### Core
This is the core of the functionality. Its grouped in 3 parts: config, memory and controllers (routes).
Each leave a hook to add your logic.

# Config
In this folder you need to setup your own needs implementing the hooks. The code in the folder is all an example.
You can delete all.

# Modules
Here you can put all the logic of your application.
