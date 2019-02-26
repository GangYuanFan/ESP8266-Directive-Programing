#include <ESP8266WiFi.h>
#define DO_INVERSE 1
#define WIFI_TIMEOUT 1500

WiFiServer server(80);
const int httpPort = "<your server port>";
////////////////////////////////////////////////////////////////////////////////
const char* ssid     = "<your SSID>";
const char* password = "<your wifi password>";
const char* host = "<your server host IP>";
uint8_t Plant_Light_Status = 0x00;
///////////////////////////////////////////////////////////////////////////////
unsigned long t1,t2;

void setup() {
  // Initialize sensor
  pinMode(2, OUTPUT);
  #if DO_INVERSE
    digitalWrite(2, HIGH);
  #else
    digitalWrite(2, LOW);
  #endif
  // We start by connecting to a WiFi network
  Serial.begin(115200);
 WiFi.mode(WIFI_STA);   // station mode
  WiFi.begin(ssid, password);   // connect to AP
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.begin();
  t1=millis();
}

void loop() {
  Serial.println(WiFi.localIP());
  WiFiClient client = server.available();
  client.setTimeout(WIFI_TIMEOUT);
  if (client.connect(host, httpPort))     // build one http connection
  {
    client.print(String("GET ") + "/read" + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" + 
             "Connection: close\r\n\r\n");      // Send HTTP Request
    unsigned long previous1 = millis(); 
    String req = client.readString();           // Receive Response
    Serial.print("read String time:");
    Serial.print((unsigned long)(millis() - previous1));
    Serial.println("\r\n");
    char arr[req.length() + 1];
    req.toCharArray(arr, req.length() + 1);
    Serial.println(req);
    Serial.println(arr[req.length() - 6]);
    Serial.println("\n");
    client.flush();
    t1=t2;
    Plant_Light_Status = uint8_t(arr[req.length() - 6]) - 48;
    #if DO_INVERSE
      digitalWrite(2, ~Plant_Light_Status & 0x01);
    #else
      digitalWrite(2, Plant_Light_Status);
    #endif
  }
  else
  {
    Serial.println("client connect failed......");
  }
}
