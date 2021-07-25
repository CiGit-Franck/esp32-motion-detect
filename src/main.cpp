#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <soc/soc.h>          // Manage interrupt
#include <soc/rtc_cntl_reg.h> // Manage interrupt
#include <PubSubClient.h> // MQTT
#include "Credential.h"   // your private credential WiFi
#include "index.h"

#define ESP_NAME "ESP32-PIR"
#define TOPIC_DEMO "demo"
#define PIN_MOTION GPIO_NUM_12

WiFiClient espClient;
PubSubClient clientMQTT(mqttServer, mqttPort, espClient);
WebServer server(80);
unsigned int counter = 0U;

void capture(String source)
{
  Serial.print("--> emis depuis ");
  Serial.println(source);
  Serial.println(++counter);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
  char body[1024];
  sprintf(body, ROOT_page, String(counter));
  server.send(200, "text/html", body);
}
void root() { capture("http"); }
void callback(char *topic, byte *payload, unsigned int length)
{
  if (strcmp(topic, TOPIC_DEMO) == 0)
  {
    capture("mqtt");
  }
}

void connectWifi()
{
  // wifi
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      ;

    Serial.print("WiFi connected.\n@IP:");
    Serial.println(WiFi.localIP());
    server.on("/", root);
    server.begin();
  }
  delay(100);
  // mqtt
  if (!clientMQTT.connected())
  {
    if (clientMQTT.connect(ESP_NAME, mqttUser, mqttPassword))
    {
      Serial.println("MQTT connected.");
      clientMQTT.subscribe(TOPIC_DEMO);
    }
    else
    {
      Serial.print("failed with state ");
      Serial.println(clientMQTT.state());
      delay(5e3);
    }
  }
  delay(100);
}

void detectsMovement()
{
  capture("motion");
}

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(PIN_MOTION, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_MOTION), detectsMovement, RISING);
  delay(100);
  // init WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(ESP_NAME);
  // init MQTT
  clientMQTT.setServer(mqttServer, mqttPort);
  clientMQTT.setCallback(callback);
  delay(100);
}

void loop()
{
  if (!clientMQTT.connected())
  {
    connectWifi();
  }
  clientMQTT.loop();
  server.handleClient();
}