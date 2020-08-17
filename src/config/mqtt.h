#include <Arduino.h>

void get_topic(char *buffer, char *extra) {
  String topic = String(MQTT_CHANNEL_PREFIX) + String(DEVICE_ID) + String(extra);
  topic.toCharArray(buffer, topic.length() + 1);
}

void subscribe_mqtt(PubSubClient &mqttClient) {
  char topic[50];
  get_topic(topic, "/sub");
  mqttClient.subscribe(topic);
}

void publish_mqtt() {
  char topic[50];
  get_topic(topic, "/pub");
  mqttClient.publish(topic, "payload");
}

void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  // DO SOMETHING
}
