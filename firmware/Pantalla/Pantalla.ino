#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

// ------------------ OLED CONFIG ------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI 23
#define OLED_CLK  18
#define OLED_DC   16
#define OLED_CS   5
#define OLED_RESET 17

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

// ------------------ WiFi CONFIG ------------------
const char* ssid = "Alexa";
const char* password = "Al12345678!";

// ------------------ SERVIDOR ------------------
const char* coinApiUrl = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin,ethereum&vs_currencies=usd";
const char* serverBase = "http://3.17.180.28:5000";  // Cambia si usas otro
String serverCommandUrl = String(serverBase) + "/api/command";
String serverPostUrl = String(serverBase) + "/api/data";

// ------------------ VARIABLES ------------------
unsigned long lastCoinFetch = 0;
unsigned long lastEnvPost = 0;
unsigned long lastRotate = 0;
unsigned long nowMs = 0;

unsigned long updateInterval = 30000;
unsigned long envPostInterval = 60000;
unsigned long rotateInterval = 10000;

int screenIndex = 0; // 0: cripto, 1: wifi, 2: comando

float lastBTC = 0.0;
float lastETH = 0.0;
String lastCommand = "N/A";
int lastRSSI = 0;
unsigned long bootTime = 0;

// ------------------ FUNCIONES OLED ------------------
void showCrypto() {
  display.clearDisplay();

  // Título amarillo
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("Precios Cripto:");

  // BTC grande azul
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 18);
  display.print("BTC ");
  display.print((int)lastBTC);

  // ETH más pequeño
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 46);
  display.print("ETH ");
  display.print((int)lastETH);

  display.display();
}

void showWifi() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Estado WiFi:");
  display.println();
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.print("RSSI: ");
  display.print(lastRSSI);
  display.println(" dBm");
  display.display();
}

void showServerCommand() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Ultimo comando:");
  display.setTextSize(2);
  display.setCursor(0, 24);
  display.print(lastCommand);
  display.display();
}

// ------------------ FUNCIONES HTTP ------------------
bool connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  int intento = 0;
  while (WiFi.status() != WL_CONNECTED && intento < 20) {
    delay(500);
    Serial.print(".");
    intento++;
  }
  Serial.println();
  return WiFi.status() == WL_CONNECTED;
}

bool fetchCoins() {
  if (WiFi.status() != WL_CONNECTED) return false;
  HTTPClient http;
  http.begin(coinApiUrl);
  int code = http.GET();
  if (code != 200) {
    http.end();
    return false;
  }

  String payload = http.getString();
  http.end();

  DynamicJsonDocument doc(512);
  if (deserializeJson(doc, payload)) return false;

  lastBTC = doc["bitcoin"]["usd"];
  lastETH = doc["ethereum"]["usd"];
  return true;
}

bool fetchServerCommand() {
  if (WiFi.status() != WL_CONNECTED) return false;
  HTTPClient http;
  http.begin(serverCommandUrl);
  int code = http.GET();
  if (code != 200) {
    http.end();
    return false;
  }

  String payload = http.getString();
  http.end();

  DynamicJsonDocument doc(256);
  if (deserializeJson(doc, payload)) return false;

  if (doc.containsKey("command")) {
    lastCommand = doc["command"].as<String>();
    if (lastCommand == "fast") updateInterval = 10000;
    else if (lastCommand == "slow") updateInterval = 60000;
    else if (lastCommand == "pause") updateInterval = ULONG_MAX;
    else if (lastCommand == "resume") updateInterval = 30000;
  }
  return true;
}

bool sendEnvData() {
  if (WiFi.status() != WL_CONNECTED) return false;
  HTTPClient http;
  http.begin(serverPostUrl);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(256);
  doc["device_id"] = "esp32_01";
  doc["rssi"] = lastRSSI;
  doc["uptime_s"] = (millis() - bootTime) / 1000;
  doc["btc"] = lastBTC;
  doc["eth"] = lastETH;

  String body;
  serializeJson(doc, body);

  int code = http.POST(body);
  http.end();

  return (code >= 200 && code < 300);
}

// ------------------ SETUP ------------------
void setup() {
  Serial.begin(115200);
  delay(500);

  SPI.begin(OLED_CLK, -1, OLED_MOSI, OLED_CS);
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("Fallo OLED");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Iniciando...");
  display.display();

  if (connectWiFi()) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi conectado!");
    display.println(WiFi.localIP());
  } else {
    display.clearDisplay();
    display.println("Error WiFi");
  }
  display.display();
  delay(1000);

  bootTime = millis();
  lastCoinFetch = millis() - updateInterval;
  lastEnvPost = millis() - envPostInterval;
  lastRotate = millis() - rotateInterval;
}

// ------------------ LOOP ------------------
void loop() {
  nowMs = millis();
  lastRSSI = WiFi.RSSI();

  // Rotación de pantallas
  if (nowMs - lastRotate >= rotateInterval) {
    screenIndex = (screenIndex + 1) % 3;
    lastRotate = nowMs;
  }

  if (screenIndex == 0) showCrypto();
  else if (screenIndex == 1) showWifi();
  else showServerCommand();

  // Actualización de precios
  if (updateInterval != ULONG_MAX && (nowMs - lastCoinFetch >= updateInterval)) {
    fetchCoins();
    lastCoinFetch = nowMs;
  }

  // Consultar comando
  if (nowMs % 15000 < 200) fetchServerCommand();

  // Enviar datos
  if (nowMs - lastEnvPost >= envPostInterval) {
    sendEnvData();
    lastEnvPost = nowMs;
  }

  delay(200);
}
