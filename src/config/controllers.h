#include <Arduino.h>
#include <ESP8266WebServer.h>
// #include <ESP8266WebServerSecure.h>

void get_relays();
void get_relay();
void put_relay();
void get_hum_temp();
void get_sensors();
void get_sensor();
void put_sensor();

void config_routing_modules(ESP8266WebServer &http_rest_server) {
  // Make your routes
  http_rest_server.on("/relays", HTTP_GET, get_relays);  // Relays
  http_rest_server.on("/relays/detail", HTTP_GET, get_relay);
  http_rest_server.on("/relays/detail", HTTP_PUT, put_relay);
  http_rest_server.on("/sensors", HTTP_GET, get_sensors);  // Sensores
  http_rest_server.on("/sensors/hum_temp", HTTP_GET, get_hum_temp);  // Humedad / Temperatura
  http_rest_server.on("/sensors/detail", HTTP_GET, get_sensor);  // More Generic
  http_rest_server.on("/sensors/detail", HTTP_PUT, put_sensor);  // More Generic
}

// void config_routing_modules_secure(BearSSL::ESP8266WebServerSecure &http_rest_server) {
//   DO SOMETHING
// }

//
// Logic of your routes
//

void _response_relay(Relay &relay) {
  StaticJsonDocument<255> jsonBuffer;
  char JSONmessageBuffer[255];

  jsonBuffer["start"] = relay.getStarted();
  jsonBuffer["pin"] = relay.getPinD();
  jsonBuffer["state"] = relay.getState();

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void _response_sensor_hum_temp(HumTempDHT &humTemp) {
  StaticJsonDocument<255> jsonBuffer;
  char JSONmessageBuffer[255];

  jsonBuffer["start"] = humTemp.getStarted();
  jsonBuffer["pin"] = humTemp.getPin();
  jsonBuffer["hum"] = humTemp.getHumedad();
  jsonBuffer["temp"] = humTemp.getTemperatura();
  jsonBuffer["error_read"] = humTemp.getErrorRead();

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void _response_sensor(uint8_t id) {
  StaticJsonDocument<255> jsonBuffer;
  char JSONmessageBuffer[255];

  // jsonBuffer["id"] = id;
  // jsonBuffer["start"] = sensors[id].getStarted();
  // jsonBuffer["type"] = sensors[id].type;
  // jsonBuffer["pin"] = sensors[id].getPin();
  // jsonBuffer["in"] = sensors[id].getIn();

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_relays() {
  if (! has_access()) {
    return;
  }

  StaticJsonDocument<500> jsonBuffer;
  char JSONmessageBuffer[500];
  JsonArray array = jsonBuffer.to<JsonArray>();

  for (uint8_t i= 0; i < sizeof(relays) / sizeof(Relay); i++) {
    JsonObject nested = array.createNestedObject();
    nested["id"] = i + 1;  // Start from 1
    nested["start"] = relays[i].getStarted();
    nested["pin"] = relays[i].getPinD();
    nested["state"] = relays[i].getState();
  }

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_relay() {
  if (! has_access()) {
    return;
  }

  uint8_t id = http_rest_server.arg("id").toInt() - 1;  // Start from 1
  if (id < 0 || id >= sizeof(relays) / sizeof(Relay)) {
      http_rest_server.send(404);
  }
  _response_relay(relays[id]);
}

void put_relay() {
  if (! has_access()) {
    return;
  }

  uint8_t id = http_rest_server.arg("id").toInt() - 1;  // Start from 1
  if (id < 0 || id >= sizeof(relays) / sizeof(Relay)) {
      http_rest_server.send(404);
  }

  StaticJsonDocument<500> jsonBuffer;
  String post_body = http_rest_server.arg("plain");

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(jsonBuffer, post_body);
  if (error) {
      Serial.println("error in parsin json body");
      http_rest_server.send(400, "application/json", "{\"error\": \"Invalid Json\"}");
  }
  else {
    bool on = jsonBuffer["on"];

    if (on) {
      relays[id].commonlyOpen();
    } else {
      relays[id].commonlyClose();
    }

    _response_relay(relays[id]);
  }
}

void get_hum_temp() {
  if (! has_access()) {
    return;
  }

  _response_sensor_hum_temp(humTemp);
}

void get_sensors() {
  if (! has_access()) {
    return;
  }

  StaticJsonDocument<1000> jsonBuffer;
  char JSONmessageBuffer[1000];
  JsonArray array = jsonBuffer.to<JsonArray>();

  JsonObject nested = array.createNestedObject();
  nested["code"] = humTemp.getCode();
  nested["start"] = humTemp.getStarted();
  nested["pin"] = humTemp.getPin();
  nested["hum"] = humTemp.getHumedad();
  nested["temp"] = humTemp.getTemperatura();
  nested["error_read"] = humTemp.getErrorRead();

  // for (uint8_t i= 0; i < senSize; i++) {
  //   JsonObject nested = array.createNestedObject();
  //   nested["id"] = i;
  //   nested["start"] = sensors[i].getStarted();
  //   nested["type"] = sensors[i].type;
  //   nested["pin"] = sensors[i].getPin();
  //   nested["in"] = sensors[i].getIn();
  // }

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_sensor() {
  if (! has_access()) {
    return;
  }

  // uint8_t id = http_rest_server.arg("id").toInt();
  // if (id < 0 || id >= senSize) {
  //     http_rest_server.send(404);
  // }
  // _response_sensor(id);
}

void put_sensor() {
  if (! has_access()) {
    return;
  }

  // uint8_t id = http_rest_server.arg("id").toInt();
  // if (id < 0 || id >= senSize) {
  //     http_rest_server.send(404);
  // }
  //
  // StaticJsonDocument<500> jsonBuffer;
  // String post_body = http_rest_server.arg("plain");
  //
  // // Deserialize the JSON document
  // DeserializationError error = deserializeJson(jsonBuffer, post_body);
  // if (error) {
  //     Serial.println("error in parsin json body");
  //     http_rest_server.send(400, "application/json", "{\"error\": \"Invalid Json\"}");
  // }
  // else {
  //   bool start = jsonBuffer["start"];
  //   sensors[id].type = jsonBuffer["type"];
  //   sensors[id].setPin(jsonBuffer["pin"]);
  //
  //   if (start) {
  //     sensors[id].begin();
  //   } else {
  //     sensors[id].stop();
  //   }
  //
  //   _response_sensor(id);
  // }
}
