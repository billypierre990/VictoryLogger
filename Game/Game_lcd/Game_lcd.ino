#include <LiquidCrystal.h>

const int scorePin = 9; // Pin to send score values

// Right Joystick
const int rightX = A1; // X-axis (not used for paddle movement)
const int rightY = A0; // Y-axis (controls right paddle)
const int rightSwitch = 8;

// Left Joystick
const int leftX = A2; // X-axis (not used for paddle movement)
const int leftY = A3; // Y-axis (controls left paddle)
const int leftSwitch = 1;

// Buzzer
const int buzzPin = 13;

// Display
const int rs = 12;
const int en = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Game variables
int paddleLeft = 0;
int paddleRight = 0;
int ballX = 8;
int ballY = 1;
int ballDirX = 1;
int ballDirY = 1;
int scoreLeft = 0;
int scoreRight = 0;

// Threshold for joystick movement from center
const int threshold = 200;
const int center = 506;

void sendScore(int score, bool isRightTeam) {
  // Send score for the right team
  if (isRightTeam) {
    for (int i = 0; i < score; i++) {
      digitalWrite(scorePin, HIGH); // Send a pulse
      delay(15); // Keep HIGH for 100 ms
      digitalWrite(scorePin, LOW); // Set LOW
      delay(22); // Wait for 200 ms before the next pulse
    }
    Serial.print("Sent Right Score: ");
    Serial.println(score); // Print sent score for debugging
  }
  // Send score for the left team
  else {
    for (int i = 0; i < score; i++) {
      digitalWrite(scorePin, HIGH); // Send a pulse
      delay(10); // Keep HIGH for 100 ms
      digitalWrite(scorePin, LOW); // Set LOW
      delay(22); // Wait for 200 ms before the next pulse
    }
    Serial.print("Sent Left Score: ");
    Serial.println(score); // Print sent score for debugging
  }
}


void setup() {
  pinMode(buzzPin, OUTPUT);
  pinMode(scorePin, OUTPUT); // Set pin 6 as output
  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
  lcd.clear();
  Serial.begin(115200);
  
  // Display initial score
  displayScore();
}

void loop() {
  // Read joystick Y positions to adjust paddles
  int leftYValue = analogRead(leftX);
  int rightYValue = analogRead(rightX);

  // Adjust left paddle position based on Y movement
  if (leftYValue < center - threshold) {
    paddleLeft = max(0, paddleLeft - 1);  // Move up, ensure within bounds
  } else if (leftYValue > center + threshold) {
    paddleLeft = min(1, paddleLeft + 1);  // Move down, ensure within bounds
  }

  // Adjust right paddle position based on Y movement
  if (rightYValue < center - threshold) {
    paddleRight = max(0, paddleRight - 1);  // Move up, ensure within bounds
  } else if (rightYValue > center + threshold) {
    paddleRight = min(1, paddleRight + 1);  // Move down, ensure within bounds
  }

  // Move the ball
  ballX += ballDirX;
  ballY += ballDirY;

  // Log ballX value
  Serial.print("ballX: ");
  Serial.println(ballX);

  // Ball collision with top and bottom walls
  if (ballY <= 0 || ballY >= 1) {
    ballDirY *= -1;  // Reverse vertical direction
  }

  // Ball collision with the left paddle only
  if (ballX == 0 && ballY == paddleLeft) { // Left paddle
    ballDirX *= -1;
  }

  // Check for missed ball (left or right side)
  if (ballX < 0) { // Left side miss
    scoreRight++; // Increment right score on miss
    Serial.print("Score Updated! Right Score: ");
    Serial.println(scoreRight);
    tone(buzzPin, 1000, 200); // Sound buzzer
  sendScore(scoreRight, true); // Send right team score

    resetBall();
  } else if (ballX > 15) { // Right side miss
    // scoreLeft++; // Increment left score on miss
    Serial.print("Score Updated! Left Score: ");
    Serial.println(scoreLeft);
    tone(buzzPin, 1000, 200); // Sound buzzer
  sendScore(scoreLeft, false); // Send left team score

    resetBall();
  }

  // Clear the LCD and display paddles, ball, and scores
  lcd.clear();
  lcd.setCursor(0, paddleLeft); // Left paddle
  lcd.write(byte(255));
  lcd.setCursor(15, paddleRight); // Right paddle
  lcd.write(byte(255));
  lcd.setCursor(ballX, ballY); // Ball
  lcd.write("O");

  // Update score display
  displayScore();

  delay(200); // Game speed
}

void resetBall() {
  ballX = 8;
  ballY = 1;
  ballDirX = (ballDirX == 1) ? -1 : 1; // Alternate ball direction on reset
}

void displayScore() {
  // Display score on the top row
  lcd.setCursor(1, 0);
  lcd.print("L:");
  lcd.setCursor(3, 0);
  lcd.print(scoreLeft);


  lcd.setCursor(12, 0);
  lcd.print("R:");
  lcd.setCursor(14, 0);
  lcd.print(scoreRight);
}
