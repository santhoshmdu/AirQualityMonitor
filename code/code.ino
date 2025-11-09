#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// WiFi Credentials
const char* ssid = "Santhosh";
const char* password = "sandy1903";

// Flask Server Address (Replace with your PC's Local IP)
const char* serverName = "http://192.168.75.145:5000/update";  // ✅ Correct



// DHT Sensor Configuration
#define DHTPIN D4        
#define DHTTYPE DHT11    
DHT dht(DHTPIN, DHTTYPE);

// MQ-135 Sensor
#define MQ135PIN A0  

WiFiClient client; // Create WiFiClient object

void setup() {
    Serial.begin(115200);
    dht.begin();
    pinMode(MQ135PIN, INPUT);

    // Connect to WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // Read Temperature & Humidity
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    // Read Gas Levels from MQ-135
    int gasValue = analogRead(MQ135PIN);

    // Convert Raw Sensor Values to Approximate Gas Concentrations
    float co2 = gasValue * 0.1;      // Example scaling factor
    float nh3 = gasValue * 0.05;     // Example scaling factor
    float benzene = gasValue * 0.02; // Example scaling factor
    float smoke = gasValue * 0.15;   // Example scaling factor
    float alcohol = gasValue * 0.08; // Example scaling factor

    // Validate Sensor Readings
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        delay(2000);
        return;
    }

    // Print Sensor Data
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.print("°C | Hum: ");
    Serial.print(humidity);
    Serial.print("% | CO2: ");
    Serial.print(co2);
    Serial.print(" ppm | NH3: ");
    Serial.print(nh3);
    Serial.print(" ppm | Benzene: ");
    Serial.print(benzene);
    Serial.print(" ppm | Smoke: ");
    Serial.print(smoke);
    Serial.print(" ppm | Alcohol: ");
    Serial.println(alcohol);

    // Check WiFi Connection Before Sending Data
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;  // Create HTTPClient object

        // Construct HTTP URL with Query Parameters
        String serverPath = String(serverName) +
                            "?temp=" + String(temperature, 1) +
                            "&hum=" + String(humidity, 1) +
                            "&co2=" + String(co2, 1) +
                            "&nh3=" + String(nh3, 1) +
                            "&benzene=" + String(benzene, 1) +
                            "&smoke=" + String(smoke, 1) +
                            "&alcohol=" + String(alcohol, 1);

        // ✅ Fix: Use WiFiClient in HTTP Request
        http.begin(client, serverPath);  
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            Serial.print("Server response: ");
            Serial.println(http.getString());
        } else {
            Serial.print("Error in HTTP request: ");
            Serial.println(httpResponseCode);
        }
        http.end();  // Close HTTP connection
    } else {
        Serial.println("WiFi not connected.");
    }

    delay(2000); // Wait 2 seconds before next reading
}
