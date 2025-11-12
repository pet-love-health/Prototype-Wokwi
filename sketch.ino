#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --- CONFIGURACIÓN DE RED ---
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define SERVER_URL "https://iot-3475-doshiro.free.beeceptor.com/api/v1/data-records"

// --- PINES ---
#define ONE_WIRE_BUS 4    
#define ALERT_PIN 13     

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// --- Variables simuladas ---
String petName = "Luna";
float humCorporal = 60.0;
int frecuenciaCardiaca = 90;
float actividad = 0.5;
float ubicacionLat = -34.6037;
float ubicacionLon = -58.3816;

// --- Variables de tiempo ---
unsigned long lastUpdate = 0;
unsigned long intervalo = 10000; 

// --- Conexión Wi-Fi ---
void conectarWiFi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 15) {
    delay(1000);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Conectado a Wi-Fi");
    Serial.print("IP local: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ No se pudo conectar a Wi-Fi");
  }
}

// --- Envío de datos al servidor ---
void enviarDatos(float temp, float hum, int bpm, float act, float lat, float lon, String mensaje) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ No hay conexión Wi-Fi. Reintentando...");
    conectarWiFi();
    if (WiFi.status() != WL_CONNECTED) return;
  }

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  String json = "{";
  json += "\"petName\":\"" + petName + "\",";
  json += "\"temperature\":" + String(temp, 1) + ",";
  json += "\"humidity\":" + String(hum, 1) + ",";
  json += "\"heartRate\":" + String(bpm) + ",";
  json += "\"activity\":" + String(act, 2) + ",";
  json += "\"latitude\":" + String(lat, 5) + ",";
  json += "\"longitude\":" + String(lon, 5) + ",";
  json += "\"message\":\"" + mensaje + "\"";
  json += "}";

  int httpCode = http.POST(json);
  if (httpCode > 0) {
    Serial.print("📡 Enviado al servidor (HTTP ");
    Serial.print(httpCode);
    Serial.println(")");
    Serial.println("Payload: " + json);
  } else {
    Serial.print("❌ Error al enviar datos: ");
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
}

// --- SETUP ---
void setup() {
  Serial.begin(115200);
  sensors.begin();
  pinMode(ALERT_PIN, OUTPUT);
  digitalWrite(ALERT_PIN, LOW);

  Serial.println("=== Collar IoT para Mascotas ===");
  conectarWiFi();
}

// --- LOOP PRINCIPAL ---
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate > intervalo) {
    lastUpdate = currentMillis;

    // --- Lectura de temperatura ---
    sensors.requestTemperatures();
    float tempCorporal = sensors.getTempCByIndex(0);

    // --- Simulación de fluctuaciones ---
    tempCorporal += random(-15, 16) / 10.0;  // ±1.5°C
    humCorporal += random(-3, 4);
    if (humCorporal < 40) humCorporal = 40;
    if (humCorporal > 80) humCorporal = 80;

    frecuenciaCardiaca += random(-15, 16);
    if (frecuenciaCardiaca < 70) frecuenciaCardiaca = 70;
    if (frecuenciaCardiaca > 160) frecuenciaCardiaca = 160;

    actividad = random(0, 100) / 100.0;

    ubicacionLat += random(-5, 6) / 10000.0;
    ubicacionLon += random(-5, 6) / 10000.0;

    // --- Evaluación del estado ---
    String mensaje = "Todo normal";
    if (tempCorporal >= 38.0 || frecuenciaCardiaca > 120) {
      mensaje = "⚠️ Riesgo de sobrecalentamiento o estrés";
    } else if (tempCorporal <= 36.5) {
      mensaje = "❄️ Posible hipotermia, revise la mascota";
    }

    // --- Control de LED ---
    bool alerta = (mensaje != "Todo normal");
    digitalWrite(ALERT_PIN, alerta ? HIGH : LOW);

    // --- Mostrar resultados ---
    Serial.println("\n---------------------------------");
    Serial.print("Nombre de mascota: "); Serial.println(petName);
    Serial.print("Temperatura corporal: "); Serial.print(tempCorporal); Serial.println(" °C");
    Serial.print("Humedad corporal: "); Serial.print(humCorporal); Serial.println(" %");
    Serial.print("Frecuencia cardíaca: "); Serial.print(frecuenciaCardiaca); Serial.println(" bpm");
    Serial.print("Nivel de actividad: "); Serial.println(actividad > 0.6 ? "Alta" : (actividad > 0.3 ? "Moderada" : "Baja"));
    Serial.print("Ubicación: ");
    Serial.print(ubicacionLat, 5);
    Serial.print(", ");
    Serial.println(ubicacionLon, 5);
    Serial.print("Mensaje: "); Serial.println(mensaje);
    Serial.println("---------------------------------");

    // --- Enviar a servidor ---
    enviarDatos(tempCorporal, humCorporal, frecuenciaCardiaca, actividad, ubicacionLat, ubicacionLon, mensaje);
  }
}
