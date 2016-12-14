

/*
 * 
TCL IOT script
Tim Waizenegger
ESP8266 für arduino, pusub mqtt client

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "hive_iot";
const char* password = "xxx";
const char* mq_org = "xxx";
const char* mq_type = "ESP8266";
const char* mq_id = "speaker_switch";
const char* mq_user = "use-token-auth";
const char* mq_authtoken = "xxx";
const char* mq_clientId = "d:xxx:ESP8266:speaker_switch"; //"d:"+mq_org+":"+mq_type+":"+mq_id;
const char* mq_serverUrl = "xxx.messaging.internetofthings.ibmcloud.com";

const int pin_a = 0;
const int pin_b = 2;

WiFiClient espClient;
PubSubClient client(espClient);


/*
 * *************************************************************************************
 * WLAN stuff
 * *************************************************************************************
 */
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(500);
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mq_clientId, mq_user, mq_authtoken)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("iot-2/cmd/switch/fmt/json");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}

/*
 * *************************************************************************************
 * MQTT handler
 * *************************************************************************************
 */

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = String("");
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(msg);
  
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }

  if (root.containsKey("channel_a")) {
    const char* state_a = root["channel_a"];
    Serial.println(state_a);
    if('1' == state_a[0]) {
      digitalWrite(pin_a, HIGH);
      delay(300);
    } else if ('0' == state_a[0]) {
      digitalWrite(pin_a, LOW);
      delay(300);
    }
  }

  if (root.containsKey("channel_b")) {
    const char* state_b = root["channel_b"];
    Serial.println(state_b);
    if('1' == state_b[0]) {
      digitalWrite(pin_b, HIGH);
      delay(300);
    } else if ('0' == state_b[0]) {
      digitalWrite(pin_b, LOW);
      delay(300);
    }
  }

}


/*
 * *************************************************************************************
 * ARDUINO API
 * *************************************************************************************
 */

void setup() {
  pinMode(pin_a, OUTPUT);
  pinMode(pin_b, OUTPUT);
  digitalWrite(pin_a,1);
  digitalWrite(pin_b,1);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mq_serverUrl, 1883);
  client.setCallback(callback);
}



void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
