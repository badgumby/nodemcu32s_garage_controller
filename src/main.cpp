#include <Arduino.h>      // Used for Arduino framework (which ESP32 chips support)
#include <WiFi.h>         // Used to connect to WiFi
#include <PubSubClient.h> // Used for MQTT communication
#include "credentials.h"  // Header file containing wifi and HA creds

// If you don't want to WebSerial access, disable these headers, and any WebSerial print
// Also remove 'ESP Async WebServer' and 'ayushsharma82/WebSerial @ ^1.1.0' from the platformio.ini file
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

// Sensor pins
const int DOOR_1_PIN = 16; // Magnetic reed switch
const int DOOR_2_PIN = 17; // Magnetic reed switch
const int LED_1_PIN = 21; // LED used for Door 1
const int LED_2_PIN = 22; // LED used for Door 2
const int RELAY_1_PIN = 32; // Relay to toggle Door 1
const int RELAY_2_PIN = 33; // Relay to toggle Door 2

// Default states (0 = Closed, 1 = Open, 2 = Fetch new state)
int DOOR_1_STATE = 0;
int DOOR_2_STATE = 0;
int LAST_DOOR_1_STATE = 2;
int LAST_DOOR_2_STATE = 2;
int LOOP_COUNTER = 0; // This is used to send checkin every 15 seconds

// MQTT settings/topics
const char *ID = "gumby_garage_controller";  // Name of our device, must be unique
const char *REFRESH_LISTENER = "gumby_garage/refresh_status"; // Used to trigger a forced update
const char *DOOR_1_POS_TOPIC = "gumby_garage/door_1_pos"; // Position of Door 1
const char *DOOR_2_POS_TOPIC = "gumby_garage/door_2_pos"; // Position of Door 1
const char *DOOR_1_RELAY_TOPIC = "gumby_garage/door_1_toggle"; // Toggle door open/close
const char *DOOR_2_RELAY_TOPIC = "gumby_garage/door_2_toggle"; // Toggle door open/close

// Door position variables
const char *OPEN = "100";
const char *CLOSE = "0";

// Setup WiFi client
WiFiClient wclient;
// Setup MQTT client
PubSubClient client(wclient);
bool state=0;

// Function to toggle the doors
void ToggleDoor(int DOOR_RELAY, int WAIT, const char* TOPIC, const char* POSITION) {
  digitalWrite(DOOR_RELAY,HIGH); // Turn on relay
  delay(500); // Wait .5 seconds
  digitalWrite(DOOR_RELAY,LOW); // Turn off relay
  client.publish(TOPIC, POSITION); // Notify MQTT broker door should be closed
  delay(WAIT); // Wait for door to act
}

// WebSerial server setup
AsyncWebServer server(80);
// WebSerial callback message
void webSerialMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
}

// Print line to both Serial and WebSerial
template<class TYPE>
void DualSerial(TYPE MSG) {
  WebSerial.println(MSG);
  Serial.println(MSG);
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Fetch current door states
  DOOR_1_STATE = digitalRead(DOOR_1_PIN);
  DOOR_2_STATE = digitalRead(DOOR_2_PIN);

  // Print topic and message to serial
  DualSerial("Topic: ");
  DualSerial(topic);
  DualSerial("Message: ");

  // Convert message from char* to string
  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];
  }
  DualSerial(message);

  // Decision tree for received topic messages
  if (strcmp(topic, "gumby_garage/door_1_toggle")==0) {
    // If message is to close, and the door is currently open
    if(message == "close" && DOOR_1_STATE == 1) {
      ToggleDoor(RELAY_1_PIN, 15000, DOOR_1_POS_TOPIC, CLOSE);
      LAST_DOOR_1_STATE = 2; // Reset door state so void Loop can fetch new state
    }
    // If message is to open, and the door is currently closed
    if(message == "open" && DOOR_1_STATE != 1) {
      ToggleDoor(RELAY_1_PIN, 2000, DOOR_1_POS_TOPIC, OPEN);
      LAST_DOOR_1_STATE = 2; // Reset door state so void Loop can fetch new state
    } 
  } else if (strcmp(topic, "gumby_garage/door_2_toggle")==0) {
    // If message is to close, and the door is currently open
    if(message == "close" && DOOR_2_STATE == 1) {
      ToggleDoor(RELAY_2_PIN, 15000, DOOR_2_POS_TOPIC, CLOSE);
      LAST_DOOR_2_STATE = 2; // Reset door state so void Loop can fetch new state
    }
    // If message is to open, and the door is currently closed
    if(message == "open" && DOOR_2_STATE != 1) {
      ToggleDoor(RELAY_2_PIN, 2000, DOOR_2_POS_TOPIC, OPEN);
      LAST_DOOR_2_STATE = 2; // Reset door state so void Loop can fetch new state
    }
  } else if (strcmp(topic, "gumby_garage/refresh_status")==0) {
    DualSerial("Sending door state refresh...");
    LAST_DOOR_1_STATE = 2; // Reset door state so void Loop can fetch new state
    LAST_DOOR_2_STATE = 2; // Reset door state so void Loop can fetch new state
  } else {
    // Listening to topic, but can't something went wrong
    DualSerial("Error on topic: '");
    DualSerial(topic);
    DualSerial("' with message: ");
    DualSerial(message);

  }
 
  DualSerial("-----------------------");
 
}

// Reconnect to client
void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    DualSerial("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID,HA_USER,HA_PASS)) {
      DualSerial("connected");
      DualSerial("Publishing to the following topics: ");
      DualSerial(DOOR_1_POS_TOPIC);
      DualSerial(DOOR_2_POS_TOPIC);

    } else {
      DualSerial(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  // MQTT subscription topics
  client.subscribe(REFRESH_LISTENER);
  client.subscribe(DOOR_1_RELAY_TOPIC);
  client.subscribe(DOOR_2_RELAY_TOPIC);
  DualSerial("-----------------------");
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  // Start serial monitoring
  Serial.begin(115200);
  // Connect to wifi
  initWiFi();
  // WebSerial startup
  // Accessible at "<IP Address>/webserial" in browser
  WebSerial.begin(&server);
  WebSerial.msgCallback(webSerialMsg);
  server.begin();
  // Print IP to WebSerial
  WebSerial.println(WiFi.localIP());
  // Print out the RSSI (in dB)
  DualSerial(WiFi.RSSI());
  // Set pin modes
  pinMode(DOOR_1_PIN, INPUT_PULLUP);
  pinMode(DOOR_2_PIN, INPUT_PULLUP);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  // Start MQTT publisher
  client.setServer(broker, 1883);
  // Start MQTT subscriber
  client.setCallback(callback);
}

void loop() {
  // Reconnect if connection is lost
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Increment loop counter
  LOOP_COUNTER++;

  // Read current door states
  DOOR_1_STATE = digitalRead(DOOR_1_PIN);
  DOOR_2_STATE = digitalRead(DOOR_2_PIN);

  // Monitor magnet switch for Door 1
  if (DOOR_1_STATE != LAST_DOOR_1_STATE) {
    if (DOOR_1_STATE == 1) {
      digitalWrite(LED_1_PIN, HIGH); // LED On
      DualSerial("Door 1: open"); // Print status to serial monitor
      client.publish(DOOR_1_POS_TOPIC, "100"); // MQTT Door 1 open
    } else {
      digitalWrite(LED_1_PIN, LOW); // LED Off
      DualSerial("Door 1: closed"); // Print status to serial monitor
      client.publish(DOOR_1_POS_TOPIC, "0"); // MQTT Door 1 closed
    }
  }
   
  // Monitor magnet switch for Door 2
  if (DOOR_2_STATE != LAST_DOOR_2_STATE) {
    if (DOOR_2_STATE == 1) {
      digitalWrite(LED_2_PIN, HIGH);  // LED On
      DualSerial("Door 2: open"); // Print status to serial monitor
      client.publish(DOOR_2_POS_TOPIC, "100"); // MQTT Door 2 open
    } else {
      digitalWrite(LED_2_PIN, LOW); // LED Off
      DualSerial("Door 2: closed"); // Print status to serial monitor
      client.publish(DOOR_2_POS_TOPIC, "0"); // MQTT Door 2 closed
    }
  }

  // Check loop counter for MQTT checkin interval
  // This is useful for sending updates in case Home Assistant has restarted
  // Sometimes Home Assistant will put a device in an unknown state on HA startup, and will not update it's state until it's received a new message
  if (LOOP_COUNTER >= 30) {
    LAST_DOOR_1_STATE = 2; // Force door state fetch on next loop
    LAST_DOOR_2_STATE = 2; // Force door state fetch on next loop
    LOOP_COUNTER = 0; // Reset loop counter
  } else {
    LAST_DOOR_1_STATE = DOOR_1_STATE; // Update previous door state to current door state
    LAST_DOOR_2_STATE = DOOR_2_STATE; // Update previous door state to current door state
  }
  
  delay(500); // Wait .5 seconds before restarting the loop
}