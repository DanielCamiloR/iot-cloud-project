# iot-cloud-project

Sistema IoT completo que integra un ESP32 con display OLED para monitorear precios de criptomonedas en tiempo real, con comunicación bidireccional a servicios en la nube.

## 🎯 Objetivos del Proyecto

- Implementar un dispositivo IoT con ESP32 y display SSD1306 OLED
- Integrar API externa de CoinGecko para obtener precios de criptomonedas
- Visualizar datos en tiempo real en pantalla OLED

## 🔧 Componentes y Hardware

### Lista de Componentes

- **ESP32 DevKit V1** - Microcontrolador con WiFi integrado
  ![Diagrama](imagenes/esp32.JPG)
- **Display OLED SSD1306** - Pantalla 128x64 píxeles (I2C)
  ![Diagrama](imagenes/pantalla.jpg)
- **Breadboard** - Para prototipado3
![Diagrama](imagenes/protoboard.jpg)
- **Cables Dunpont** - Para conexiones
- **Cable USB** - Para programación y alimentación

### Diagrama de Conexiones

![Diagrama](imagenes/imagen_conexion_esp32_pantalla_led.png)

## 🌐 Servicios Integrados
### Servicio en la nube
Se implementó el servicio de Amazon Web Services (AWS), utilizando una instancia EC2 como servidor principal para alojar la aplicación Flask.

![Diagrama](imagenes/instancia.JPG)

### API Externa

- **CoinGecko API** - Precios de criptomonedas en tiempo real
- **Endpoint:** `https://api.coingecko.com/api/v3/simple/price?ids=bitcoin,ethereum&vs_currencies=usd`
- **Criptomonedas monitoreadas:** Bitcoin (BTC) y Ethereum (ETH)
- **Frecuencia de actualización:** Cada 60 segundos
  
## 📑 Instrucciones de instalación 
### Servidor AWS
1. Conexión al servidor en la nube (AWS EC2)
- Iniciar sesión en la instancia mediante SSH:
``` ssh -i "llave.pem" ubuntu@<IP_PUBLICA> ```
2. Actualización del sistema
 ```sudo apt update && sudo apt upgrade -y```
3. Instalación de Python y dependencias necesarias
``` sudo apt install python3 python3-pip python3-venv -y ```
4. Creación y activación del entorno virtual
```python3 -m venv venv source venv/bin/activate ```

5. Instalación de Flask y librerías utilizadas
```pip install flask requests ```

6. Clonación o creación del proyecto
- Crear una carpeta para el proyecto:
``` mkdir proyecto_iot && cd proyecto_iot ```
- Dentro de esta carpeta, colocar el archivo server.py.
7. Inicialización de la base de datos SQLite
-La base de datos data.db se crea automáticamente al ejecutar el servidor por primera vez.
- No requiere configuración adicional.
8. Ejecución del servidor Flask
 ``` python3 server.py ```
 - Se confirma que el servidor queda disponible en: 
[Servidor](http://3.17.180.28:5000/)

### Arduino IDE
#### Requisitos Previos

1. **Arduino IDE** (v1.8.19 o superior)
2. **Soporte para ESP32** instalado en Arduino IDE
3. **Librerías requeridas:**

```cpp
#include <WiFi.h>              // Conectividad WiFi
#include <HTTPClient.h>        // Cliente HTTP/HTTPS
#include <ArduinoJson.h>       // Parsing JSON (v6.21.0+)
#include <Adafruit_GFX.h>      // Gráficos para OLED
#include <Adafruit_SSD1306.h>  // Driver OLED SSD1306
#include <Wire.h>              // Comunicación I2C
```

#### Instalación de Librerías

**Método Arduino IDE**

1. Ir a `Sketch > Include Library > Manage Libraries`
2. Buscar e instalar cada librería:
   - "Adafruit SSD1306"
   - "Adafruit GFX Library"
   - "ArduinoJson" by Benoit Blanchon
   - "Arduino BusIO" by Adafruit


## ⚙️ Funcionalidades Implementadas
### 1️⃣ Visualización de Datos (API Externa)

- ✅ Consulta de precios BTC y ETH desde CoinGecko
- ✅ Actualización automática cada 60 segundos
- ✅ Visualización clara en display OLED
- ✅ Rotación entre pantallas de información
- ✅ Actualización tiempo real en la nube

### Configuración de Credenciales

1. **Duplicar archivo de configuración:**

```bash cp firmware/config.h.example firmware/config.h ```
2. **Editar `config.h` con tus credenciales:**

```cpp
// Configuración WiFi
#define WIFI_SSID "TU_RED_WIFI"
#define WIFI_PASSWORD "TU_CONTRASEÑA"

// API CoinGecko (sin API Key requerida)
#define COINGECKO_API "https://api.coingecko.com/api/v3/simple/price"

// Configuración Cloud (ThingSpeak ejemplo)
#define CLOUD_ENDPOINT "https://api.thingspeak.com/update"
#define CLOUD_API_KEY "TU_WRITE_API_KEY"

// Parámetros de actualización
#define UPDATE_INTERVAL 60000  // 60 segundos
#define CLOUD_SEND_INTERVAL 60000  // 1 minuto
```

### Pasos de Instalación

1. **Clonar el repositorio:**

   ```bash
   git clone https://github.com/DanielCamiloR/iot-cloud-project.git
   cd iot-cloud-project
   ```
2. **Conectar el hardware** según el diagrama de conexiones
   ![Diagrama](imagenes/instalacion.jpg)
3. **Comprobación de hardware**:
   Se verificó el correcto funcionamiento del ESP32 y de la pantalla integrada, asegurando la comunicación estable entre ambos y su respuesta adecuada durante las pruebas iniciales.

![Diagrama](imagenes/esp32_oled_funcionando.png)

4. **Configurar credenciales** en `firmware/config.h`
5. **Compilar y cargar:**

   - Abrir `firmware/main.ino` en Arduino IDE
   - Seleccionar placa: "ESP32 Dev Module"
   - Seleccionar puerto COM correcto
   - Presionar "Upload" (Ctrl+U)

## 🚀 Uso del Sistema

### Operación Normal

Al iniciar, el sistema:

1. ✅ Se conecta a la red WiFi configurada
2. ✅ Inicializa el display OLED
3. ✅ Muestra pantalla de bienvenida "BTC"
4. ✅ Comienza a consultar precios cada 60 segundos

### Pantallas Disponibles

**Pantalla Principal:**

![Diagrama](imagenes/pantalla%20sin%20wifi.jpg)

**Flujo de datos:**
1. Conexión de WiFi
2. ESP32 consulta precios a CoinGecko cada 60s
3. Parsea respuesta JSON y actualiza display

## 🐛 Manejo de Errores

```cpp
// Reconexión automática WiFi
if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
}

// Validación de respuestas HTTP
if (httpResponseCode == 200) {
    // Procesar datos
} else {
    Serial.println("Error en API: " + String(httpResponseCode));
    displayError("API Error");
}

// Watchdog timer para reinicios
esp_task_wdt_init(30, true);
```

## 📊 Datos Recolectados

### Métricas en Tiempo Real:

- Precio Bitcoin (USD)
- Precio Ethereum (USD)
- Tiempo de funcionamiento (uptime)
- Calidad de señal WiFi (RSSI en dBm)
- Actualización de valores cada 60 segundos

## 📸 Capturas de Pantalla

### Hardware Ensamblado

![ESP32 con OLED funcionando](imagenes/esp32_oled_funcionando.png)

### Display OLED conectado a WiFi
![Diagrama](imagenes/pantalla%20funcionando3.jpg)

### Display OLED Mostrando Precios

![Pantalla BTC](imagenes/pantalla_funcionando.jpg)

### Display OLED Mostrando peticiones realizadas desde la nube

![Diagrama](imagenes/pantalla%20funcionando3.jpg)

### Dashboard funcional con autoactualización.

![Diagrama](imagenes/Dashboard.JPG)

**Incluye:**

- Conexión inicial del ESP32
- Consulta y visualización de precios
- Manejo de desconexiones

## ⚡ Optimizaciones Implementadas

### Conectividad:

- 🔁 Reconexión automática WiFi

## 🧪 Testing y Validación

### Pruebas Realizadas:

✅ Funcionamiento continuo por 24 horas
✅ Manejo de desconexión WiFi
✅ Recuperación ante fallo de API
✅ Dashboard funcional con autoactualización
✅ Gráficos dinámicos BTC/ETH.
✅ Tabla de registros IoT actualizada en tiempo real.
✅ API REST lista para conectar con dispositivos ESP32.
## ❗ Problemas Encontrados y Soluciones

### Problema 1: Desconexiones WiFi aleatorias

**Solución:** Implementar watchdog timer y función de reconexión automática con reintentos exponenciales.

### Problema 2: Rate limiting de CoinGecko API

**Solución:** Respetar límite de 50 llamadas/minuto, implementar intervalo mínimo de 60 segundos.

## 📚 Referencias y Recursos

### Documentación Técnica:

- [ESP32 Arduino Core](https://docs.espressif.com/projects/arduino-esp32/)
- [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)
- [ArduinoJson Documentation](https://arduinojson.org/)
- [CoinGecko API Docs](https://www.coingecko.com/en/api/documentation)

### Tutoriales Utilizados:

- ESP32 WiFi Connection Guide
- OLED Display I2C Tutorial
- REST API Integration with ESP32

### Herramientas:

- Arduino IDE 1.8.19
- PlatformIO VS Code Extension
- Postman (testing API)
