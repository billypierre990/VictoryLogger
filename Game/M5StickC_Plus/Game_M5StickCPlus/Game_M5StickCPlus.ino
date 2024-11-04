#include <M5StickCPlus2.h>
#include <ThingSpeak.h>

#include <WiFi.h>
#include "secrets.h"

char ssid[] = WIFI_SSID;   // your network SSID (name) 
char pass[] = WIFI_PASSWORD;   // your network password
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char* myWriteAPIKey = SECRET_WRITE_APIKEY;

const int scorePin = G26; // Use G26 as the score pin

unsigned long lastScoreTime = 0;
int previousState = LOW; // Variable to store the previous state of the pin
int scoreLeft = 0;
int scoreRight = 0;


void setup() {
  M5.begin(); // Initialize the M5StickC Plus
  pinMode(scorePin, INPUT); // Set scorePin as an input
  M5.Lcd.setTextSize(4); // Set text size for better visibility
  M5.Lcd.fillScreen(BLACK); // Clear the screen at startup
  Serial.begin(115200); // Initialize Serial communication for debugging

      while (!Serial) {
        ; // wait for serial port to connect. Needed for Leonardo native USB port only
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);  // Connect to WiFi
    ThingSpeak.begin(client);  // Initialize ThingSpeak

    StickCP2.Display.setRotation(1);  // Initial rotation
    StickCP2.Display.setTextDatum(middle_center);
    StickCP2.Display.setFont(&fonts::FreeSansBold9pt7b);
}
  
  // // Display initial scores
  // displayScore();
// }

void loop() {
  //********************************************************* Check Wifi Connection *********************************************************
  if (WiFi.status() != WL_CONNECTED) {
    StickCP2.Display.fillScreen(BLACK);
    StickCP2.Display.setTextColor(WHITE);
    StickCP2.Display.setTextDatum(middle_center);
    M5.Lcd.setTextSize(1); // Set text size for better visibility
    StickCP2.Display.drawString("Connecting to WiFi...", StickCP2.Display.width() / 2, StickCP2.Display.height() / 2);
    
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    
    // Attempt to connect to WiFi
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);  // Delay to avoid rapid connection attempts
    }
    
    // Display "Connected" message when connected
    StickCP2.Display.fillScreen(BLACK); // Clear the previous message
    StickCP2.Display.setTextColor(GREEN);
    StickCP2.Display.drawString("WiFi Connected", StickCP2.Display.width() / 2, StickCP2.Display.height() / 2);
    
    delay(2000);  // Show the message for 2 seconds
    StickCP2.Display.fillScreen(BLACK); // Clear screen for next use
    Serial.println("\nConnected.");
  }

  // Read the current state of the score pin
  int currentState = digitalRead(scorePin);
  Serial.print("Current State: ");
  Serial.println(currentState); // Print the current state of the score pin for debugging

  // Check for rising edge on the score pin (HIGH signal)
  if (currentState == HIGH && previousState == LOW) {
    // Record the time when the signal goes HIGH
    unsigned long startTime = millis();

    // Wait until the signal goes LOW again
    while (digitalRead(scorePin) == HIGH) {
      // Optionally, you can add a timeout here to avoid infinite loops
    }

    // Calculate the duration the pin was HIGH
    unsigned long duration = millis() - startTime;

    Serial.println(duration);
    // Check the duration to determine which team scored
    if (duration > 10) { // Right team scores
      Serial.println("Right Team Scored");
      scoreRight++;
      sendScoreToThingSpeak(scoreLeft, scoreRight); // Send scores to ThingSpeak
    } else if (duration < 10) { // Left team scores
      Serial.println("Left Team Scored");
      scoreLeft++;
      sendScoreToThingSpeak(scoreLeft, scoreRight); // Send scores to ThingSpeak
    }

    displayScore(); // Update the display after changing the scores

    delay(1000); // Delay to allow for visibility of the message
  }

  previousState = currentState; // Update previous state for next loop
}

// Function to send scores to ThingSpeak
void sendScoreToThingSpeak(int leftScore, int rightScore) {
  // Prepare data to send to ThingSpeak
  ThingSpeak.setField(1, leftScore);  // Assuming field 1 is for left score
  ThingSpeak.setField(2, rightScore); // Assuming field 2 is for right score

  // Write to ThingSpeak
  int result = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  if (result == 200) {
    Serial.println("Score updated successfully");
  } else {
    Serial.print("Error updating score. Code: ");
    Serial.println(result);
  }
}

// Function to display the scores on the screen
void displayScore() {
  M5.Lcd.setTextSize(2); // Set text size for better visibility
  StickCP2.Display.setRotation(0);
  M5.Lcd.fillScreen(BLACK); // Clear screen
  M5.Lcd.setTextColor(WHITE); // Set text color for the scores

  M5.Lcd.setCursor(2, 30); // Move cursor down for the left score
  M5.Lcd.print("Left          ");
  M5.Lcd.setTextColor(BLUE); // Set text color for the scores
  M5.Lcd.print(scoreLeft);

  M5.Lcd.setTextColor(WHITE); // Set text color for the scores
  M5.Lcd.setCursor(2, 130); // Move cursor down for the right score
  M5.Lcd.print("Right        ");
  M5.Lcd.setTextColor(RED); // Set text color for the scores
  M5.Lcd.print(scoreRight);
}