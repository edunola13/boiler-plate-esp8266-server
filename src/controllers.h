#include <ESP8266WebServer.h>

void not_found();
void forbidden();
bool has_access();

void get_info();
void get_config();
void put_config();
void post_config();
void get_relays();
void get_relay();
void put_relay();
void get_sensors();
void get_hum_temp();
void get_sensor();
void put_sensor();

void config_rest_server_routing() {
  /* SIN HTTPS */
  http_rest_server.on("/", HTTP_GET, []() {
      http_rest_server.send(200, "text/html", "Running");
  });

  // HANDLE
  http_rest_server.on("/info", HTTP_GET, get_info);  // Info de identificacion
  http_rest_server.on("/config", HTTP_GET, get_config);  // Configuracion de Conexion y General
  http_rest_server.on("/config", HTTP_PUT, put_config);
  http_rest_server.on("/config", HTTP_POST, post_config);
  http_rest_server.on("/relays", HTTP_GET, get_relays);  // Relays
  http_rest_server.on("/relays/detail", HTTP_GET, get_relay);
  http_rest_server.on("/relays/detail", HTTP_PUT, put_relay);
  http_rest_server.on("/sensors", HTTP_GET, get_sensors);  // Sensores
  http_rest_server.on("/sensors/hum_temp", HTTP_GET, get_hum_temp);  // Humedad / Temperatura
  http_rest_server.on("/sensors/detail", HTTP_GET, get_sensor);  // More Generic
  http_rest_server.on("/sensors/detail", HTTP_PUT, put_sensor);  // More Generic
  // http_rest_server.on('route', function);  // No filtra por METHOD -> despues pedir con http_rest_server.method()

  /* CON HTTPS */
  /*http_rest_server.on("/", []() {
    http_rest_server.sendHeader("Location", String("https://") + WiFi.localIP().toString(), true);
    http_rest_server.send(301, "text/plain", "");
  });
  http_rest_server.begin();

  http_rest_server_ssh.setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  http_rest_server_ssh.on("/", HTTP_GET, []() {
      http_rest_server_ssh.send(200, "text/html", "Running");
  });

  // HANDLE
  http_rest_server_ssh.on("/info", HTTP_GET, get_info);  // Info de identificacion
  http_rest_server_ssh.on("/config", HTTP_GET, get_config);  // Configuracion de Conexion
  http_rest_server_ssh.on("/config", HTTP_PUT, put_config);
  http_rest_server_ssh.on("/config", HTTP_POST, post_config);
  http_rest_server_ssh.on("/sensors", HTTP_GET, get_sensors);  // Sensores
  http_rest_server_ssh.on("/sensors/detail", HTTP_GET, get_sensor);
  http_rest_server_ssh.on("/sensors/detail", HTTP_PUT, put_sensor);
  // http_rest_server_ssh.on('route', function);  // No filtra por METHOD -> despues pedir con http_rest_server_ssh.method()
  */

  http_rest_server.onNotFound(not_found);
  // http_rest_server_ssh.onNotFound(not_found);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"Key"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  http_rest_server.collectHeaders(headerkeys, headerkeyssize);
  // http_rest_server_ssh.collectHeaders(headerkeys, headerkeyssize);

  http_rest_server.begin();
  // http_rest_server_ssh.begin();
}

void not_found() {
  http_rest_server.send(404);
}

void forbidden() {
  http_rest_server.send(403);
}

bool has_access() {
  if (String("") == String(config.access_key)) {
    return true;
  }

  if (http_rest_server.hasHeader("Key")){
    String key = http_rest_server.header("Key");
    if (key == String(config.access_key)) {
      return true;
    }
  }

  forbidden();
  return false;
}

void _response_config() {
  StaticJsonDocument<750> jsonBuffer;
  char JSONmessageBuffer[750];

  jsonBuffer["timeStart"] = millis();

  jsonBuffer["name"] = config.name;
  jsonBuffer["access_key"] = config.access_key;
  jsonBuffer["mq_client"] = config.mq_client;
  jsonBuffer["mq_server"] = config.mq_server;
  jsonBuffer["mq_user"] = config.mq_user;
  jsonBuffer["mq_pass"] = config.mq_pass;

  jsonBuffer["ssid"] = config.ssid;
  jsonBuffer["passwd"] = config.passwd;
  jsonBuffer["ap_ssid"] = config.ap_ssid;
  jsonBuffer["ap_passwd"] = config.ap_passwd;
  jsonBuffer["staticIp"] = config.staticIp;

  JsonArray ipArray = jsonBuffer["ip"].to<JsonArray>();
  ipArray.add(config.ip[0]);
  ipArray.add(config.ip[1]);
  ipArray.add(config.ip[2]);
  ipArray.add(config.ip[3]);

  JsonArray gatewayArray = jsonBuffer["gateway"].to<JsonArray>();
  gatewayArray.add(config.gateway[0]);
  gatewayArray.add(config.gateway[1]);
  gatewayArray.add(config.gateway[2]);
  gatewayArray.add(config.gateway[3]);

  JsonArray subnetArray = jsonBuffer["subnet"].to<JsonArray>();
  subnetArray.add(config.subnet[0]);
  subnetArray.add(config.subnet[1]);
  subnetArray.add(config.subnet[2]);
  subnetArray.add(config.subnet[3]);

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

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

void get_info() {
  StaticJsonDocument<250> jsonBuffer;
  char JSONmessageBuffer[250];

  // Status
  String(ESP.getChipId(), HEX);
  jsonBuffer["uniqueId"] = String(ESP.getChipId(), HEX);
  jsonBuffer["type"] = DEVICE_TYPE;
  jsonBuffer["version"] = DEVICE_VERSION;
  jsonBuffer["name"] = config.name;
  jsonBuffer["actualIp"] = status.ip.toString();
  jsonBuffer["status"] = String(status.status);

  serializeJson(jsonBuffer, JSONmessageBuffer);
  http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void get_config() {
  if (! has_access()) {
    return;
  }

  _response_config();
}

void put_config() {
  if (! has_access()) {
    return;
  }

  StaticJsonDocument<500> jsonBuffer;
  String post_body = http_rest_server.arg("plain");

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(jsonBuffer, post_body);
  if (error) {
      Serial.println("error in parsin json body");
      http_rest_server.send(400, "application/json", "{\"error\": \"Invalid Json\"}");
  } else {
      if (jsonBuffer.containsKey("name")) {
        String value = jsonBuffer["name"];
        value.toCharArray(config.name, 20);
      }
      if (jsonBuffer.containsKey("access_key")) {
        String value = jsonBuffer["access_key"];
        value.toCharArray(config.access_key, 30);
      }
      if (jsonBuffer.containsKey("mq_client")) {
        String value = jsonBuffer["mq_client"];
        value.toCharArray(config.mq_client, 15);
      }
      if (jsonBuffer.containsKey("mq_server")) {
        String value = jsonBuffer["mq_server"];
        value.toCharArray(config.mq_server, 15);
      }
      if (jsonBuffer.containsKey("mq_user")) {
        String value = jsonBuffer["mq_user"];
        value.toCharArray(config.mq_user, 10);
      }
      if (jsonBuffer.containsKey("mq_pass")) {
        String value = jsonBuffer["mq_pass"];
        value.toCharArray(config.mq_pass, 10);
      }

      if (jsonBuffer.containsKey("ssid")) {
        String value = jsonBuffer["ssid"];
        value.toCharArray(config.ssid, 30);
      }
      if (jsonBuffer.containsKey("passwd")) {
        String value = jsonBuffer["passwd"];
        value.toCharArray(config.passwd, 30);
      }
      if (jsonBuffer.containsKey("ap_ssid")) {
        String value = jsonBuffer["ap_ssid"];
        value.toCharArray(config.ap_ssid, 20);
      }
      if (jsonBuffer.containsKey("ap_passwd")) {
        String value = jsonBuffer["ap_passwd"];
        value.toCharArray(config.ap_passwd, 20);
      }
      if (jsonBuffer.containsKey("staticIp")) {
        config.staticIp = jsonBuffer["staticIp"];//.as<bool*>();
      }

      if (jsonBuffer.containsKey("ip")) {
        JsonArray ipArray = jsonBuffer["ip"].as<JsonArray>();
        if (ipArray.size() == 4) {
          config.ip[0] = ipArray[0];
          config.ip[1] = ipArray[1];
          config.ip[2] = ipArray[2];
          config.ip[3] = ipArray[3];
        }
      }
      if (jsonBuffer.containsKey("gateway")) {
        JsonArray gatewayArray = jsonBuffer["gateway"].as<JsonArray>();
        if (gatewayArray.size() == 4) {
          config.gateway[0] = gatewayArray[0];
          config.gateway[1] = gatewayArray[1];
          config.gateway[2] = gatewayArray[2];
          config.gateway[3] = gatewayArray[3];
        }
      }
      if (jsonBuffer.containsKey("subnet")) {
        JsonArray subnetArray = jsonBuffer["subnet"].as<JsonArray>();
        if (subnetArray.size() == 4) {
          config.subnet[0] = subnetArray[0];
          config.subnet[1] = subnetArray[1];
          config.subnet[2] = subnetArray[2];
          config.subnet[3] = subnetArray[3];
        }
      }

      _response_config();
      // Wait 3 seconds -> If no wait dont receive response the client
      long initial = millis() + 3000;
      while (initial > millis()) {
        delay(5);
      }
      initWifi();
  }
}

void post_config() {
  if (! has_access()) {
    return;
  }

  saveConfig();
  http_rest_server.send(200);
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
