#ifndef ModuleSensors_h
#define ModuleSensors_h

#include <sensors/HumTempDHT.h>

struct SensorConfig {
  long refreshTime = 1000 * 60;  // 1 Minuto
  long lastRefreshTime = 0;
} sensorConfig;

HumTempDHT humTemp(D5, 11);  // DHT11

void updateSensors() {
  if(millis() - sensorConfig.lastRefreshTime >= sensorConfig.refreshTime || millis() - sensorConfig.lastRefreshTime < 0) {
    // HumTempDHT
    humTemp.updateSensor();
    if (! humTemp.getErrorRead()) {
      StaticJsonDocument<255> jsonBuffer;
      char JSONmessageBuffer[255];

      jsonBuffer["hum"] = humTemp.getHumedad();
      jsonBuffer["temp"] = humTemp.getTemperatura();
      serializeJson(jsonBuffer, JSONmessageBuffer);

      mqttClient.publish("/esp/hum_temp", JSONmessageBuffer);
    }

    // Actualizo el tiempo
    sensorConfig.lastRefreshTime = millis();
  }
}

#endif
