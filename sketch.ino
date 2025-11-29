#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <time.h>   

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

const char* MQTT_SERVER = "test.mosquitto.org";
const int   MQTT_PORT   = 1883;
const char* MQTT_CLIENT_ID = "upet-001";
const char* TOPIC_STATUS = "upet/devices/upet-001/status";

unsigned long lastUpdate = 0;
unsigned long intervalo = 10000;

struct CollarData {
  bool online = true;
  float latitude = -34.6037;
  float longitude = -58.3816;
  float temperature = 37.0;
  float humidity = 50.0;
  int pulse = 80;
  String last_seen = "";
} collar;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void initTime() {
  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");  

  Serial.print("⌛ Sincronizando hora...");
  struct tm timeinfo;

  while (!getLocalTime(&timeinfo)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n⏱️ Hora sincronizada.");
}

String getLastSeen() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return "00/00/0000 00:00:00"; 
  }

  char buffer[25];
  strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);

  return String(buffer);
}

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\n✅ WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  Serial.print("Conectando a MQTT...");
  
  while (!mqttClient.connected()) {
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println(" conectado!");
    } else {
      Serial.print(" fallo (");
      Serial.print(mqttClient.state());
      Serial.println("). Reintentando en 2s...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  connectWiFi();
  initTime();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  randomSeed(analogRead(0));
}

void loop() {

  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate > intervalo) {
    lastUpdate = currentMillis;

    collar.temperature += ((float)(random(-5, 6))) / 10.0;
    collar.temperature = constrain(collar.temperature, 36.0, 38.5);

    collar.latitude += ((float)(random(-5, 6))) / 10000.0;
    collar.longitude += ((float)(random(-5, 6))) / 10000.0;

    collar.pulse += random(-3, 4);
    collar.pulse = constrain(collar.pulse, 60, 160);

    collar.humidity = random(30, 70);

    collar.last_seen = getLastSeen();

    String json = "{";
    json += "\"online\":" + String(collar.online ? "true" : "false") + ",";
    json += "\"latitude\":" + String(collar.latitude, 5) + ",";
    json += "\"longitude\":" + String(collar.longitude, 5) + ",";
    json += "\"temperature\":" + String(collar.temperature, 1) + ",";
    json += "\"humidity\":" + String(collar.humidity) + ",";
    json += "\"pulse\":" + String(collar.pulse) + ",";
    json += "\"last_seen\":\"" + collar.last_seen + "\"";
    json += "}";

    if (mqttClient.publish(TOPIC_STATUS, json.c_str())) {
      Serial.println("📡 MQTT Enviado:");
    } else {
      Serial.println("❌ Error al enviar MQTT");
    }

    Serial.println(json);
  }
}
