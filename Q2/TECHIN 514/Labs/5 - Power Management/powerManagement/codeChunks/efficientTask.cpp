#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <FirebaseClient.h>
#include <WiFiClientSecure.h>


#define WIFI_SSID "Jadens IPhone"
#define WIFI_PASSWORD "password" // Replace with your network password

#define DATABASE_SECRET "API Key" // Replace with your database URL
#define DATABASE_URL "https://esp32c3-powermanagement-default-rtdb.firebaseio.com/" // Replace with your API key

#define STAGE_INTERVAL 12000 // 12 seconds each stage
#define MAX_WIFI_RETRIES 5 // Maximum number of WiFi connection retries

WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
LegacyToken dbSecret(DATABASE_SECRET);

int uploadInterval = 1000; // 1 seconds each upload
unsigned long sendDataPrevMillis = 0;
int count = 0;
// bool signupOK = false;

// HC-SR04 Pins
const int trigPin = D9;
const int echoPin = D10;

// Define sound speed in cm/usec
const float soundSpeed = 0.034;

//array
struct DistanceData {
    String dateTime;
    float distance;
};

//functions
float measureDistance();
void connectToWiFi();
void initFirebase();
void sendDataToFirebase(float distance);
String getDateTime();

DistanceData distanceArray[59];


void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void setup(){
  Serial.begin(115200);
  delay(500);
  // while(!Serial){
  //   yield();
  // }

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize time
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    Serial.println("Waiting for time");
    while (!time(nullptr)) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("");
}

void loop() {
    for (int i = 0; i < 59; i++) {
        // Sleep for 900ms
        Serial.println("Entering light sleep mode for 900ms...");
        delay(900);

        // Read sensor data for 100ms
        Serial.println("Reading sensor data...");
        float distance = measureDistance();
        String dateTime = getDateTime();
        distanceArray[i] = {dateTime, distance};
        Serial.print("DateTime: ");
        Serial.print(dateTime);
        Serial.print(" Distance: ");
        Serial.print(distance);
        Serial.println(" cm");

        // Sleep for 100ms to complete the 1-second cycle
        delay(100);
    }

    // Send data to Firebase after 59 cycles
    Serial.println("Sending data to Firebase...");
    float distance = measureDistance();
    initFirebase();
    connectToWiFi();
    sendDataToFirebase(distanceArray);
    sendDataPrevMillis = millis();
    WiFi.disconnect();
}

float measureDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * soundSpeed / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}

void connectToWiFi()
{
  // Print the device's MAC address.
  Serial.println(WiFi.macAddress());
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi");
  int wifiCnt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifiCnt++;
    if (wifiCnt > MAX_WIFI_RETRIES){
      Serial.println("WiFi connection failed");
      ESP.restart();
    }
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void initFirebase(){
  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    ssl.setInsecure();
#if defined(ESP8266)
    ssl.setBufferSizes(1024, 1024);
#endif

    // Initialize the authentication handler.
    Serial.println("Initializing the app...");
    initializeApp(client, app, getAuth(dbSecret));

    // Binding the authentication handler with your Database class object.
    app.getApp<RealtimeDatabase>(Database);

    // Set your database URL (requires only for Realtime Database)
    Database.url(DATABASE_URL);

    // In sync functions, we have to set the operating result for the client that works with the function.
    client.setAsyncResult(result);
}

void sendDataToFirebase(DistanceData distanceArray[59]) {
    for (int i = 0; i < 59; i++) {
        String path = "/test2/entry" + String(i);
        
        Serial.print("Pushing the dateTime value... ");
        if (Database.set(path + "/dateTime", distanceArray[i].dateTime.c_str())) {
            Serial.println("ok");
        } else {
            Serial.print("Failed: ");
            Serial.println(Database.errorReason());
        }

        Serial.print("Pushing the distance value... ");
        if (Database.set(path + "/distance", distanceArray[i].distance)) {
            Serial.println("ok");
        } else {
            Serial.print("Failed: ");
            Serial.println(Database.errorReason());
        }
    }
    Serial.println("Data sent to Firebase successfully");
}