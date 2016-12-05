#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>

const char* ssid = "SSID of your wifi network";
const char* password = "*******";

// ip du serveur sarah
const char* host = "192.168.240.33";


// Definition des PIN pour le sonar ultrason
#define TRIGGER_PIN  14
#define ECHO_PIN     12

#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup() {

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  //Serial.begin (9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("CAB MEDICAL Dr CADIC");
  display.println("Acquisition signal...");
  display.println("Mode RADAR: ON");
  display.display();

// INIT du sonar
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
 
// connection au reseau WIFI
  display.println(ssid);
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  display.println("WiFi connected");
  display.println(WiFi.localIP());
  // Affichage de la distance
  display.println("Scanning ...");
  display.display();

}

int value = 0;

void loop() {
  // Code pour rechercher la rupture de distance
  long duration, distance;
  digitalWrite(TRIGGER_PIN, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1;
  //Serial.print(distance);
  //Serial.println(" cm");
  delay(1000);

  // Affichage de la distance
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Distance (cm)");

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(30,30);
  display.println(distance);
  display.display();

  // Si distance < 50 alors on lance la synthèse vocale
  if (distance < 50) {
    display.clearDisplay();
    display.display();
    display.setTextSize(1);
    display.setCursor(30,20);
    display.println("Detection!");
    display.display();

  // ON lance la synthese vocale

  // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 8888;
      if (!client.connect(host, httpPort)) {
           display.clearDisplay();
           display.display();   
           display.println("connection failed");
           display.display();
         return;
      }
  
    // We now create a URI for the request
   String url = "/?tts=bonjour.%20bienvenue%20au%20cabinet%20médical.%20la%20salle%20d%27attente%20est%20derrière%20vous.%20Merci%20d%27y%20patienter";
  
    //Serial.print("Requesting URL: ");
    //Serial.println(url);
  
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
     unsigned long timeout = millis();
     
      while (client.available() == 0) {
       if (millis() - timeout > 5000) {
       display.println(">>> Client Timeout !");
       display.display();
       client.stop();
       return;
              }
      } 
  
    // Read all the lines of the reply from server and print them to Serial
      while(client.available()){
      String line = client.readStringUntil('\r');
        //Serial.print(line);
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(10);                       // wait for a second
    
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(10);                       // wait for a second
  }
  
      //Serial.println();
      //Serial.println("closing connection");
      delay (10000);

  }


}
