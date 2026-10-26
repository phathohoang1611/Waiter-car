#include <Wire.h>
#include <Keypad.h>

// Motor pin definitions
const int IN1 = 3;
const int IN2 = 4;
const int motorAspeed = 5;
const int IN3 = 7;
const int IN4 = 8;
const int motorBspeed = 6;

// Keypad setup
char receivedKey = 0;
int targetTable = 0;  // Target table number from keypad
bool rotate;
// Sensor definitions
int R_S = 0;
int L_S = 0;
int RM_S = 0;
int LM_S = 0;

// Variables to control counting
bool start = false; 
bool success = false;
unsigned long table = 0;  // Variable to store received table from slave

// Motor control functions
void forward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(motorBspeed, 180);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(motorAspeed, 200);
}

void turnLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(motorAspeed, 180);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(motorBspeed, 200);
}

void slightTurnRight() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(motorBspeed, 200);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(motorAspeed, 180);
}

void slightTurnLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(motorAspeed, 200);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(motorBspeed, 180);
}

void Stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void rotate_left() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(motorAspeed, 215);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(motorBspeed, 230);
}
void rotate_right() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(motorBspeed, 215);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(motorAspeed, 230);
}

// Setup function
void setup() {
  Serial.begin(9600);

  pinMode(L_S, INPUT);
  pinMode(RM_S, INPUT);
  pinMode(LM_S, INPUT);
  pinMode(R_S, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(motorAspeed, OUTPUT);
  pinMode(motorBspeed, OUTPUT);

  Wire.begin();  // Start I2C as master
  delay(2000);
}

// Main loop
void loop() {
  // Read sensor values
  R_S = digitalRead(10);
  RM_S = digitalRead(2);
  LM_S = digitalRead(9);
  L_S = digitalRead(12);

  // Request keypad input from the slave
  Wire.requestFrom(1, 1);
  if (Wire.available()) {
    receivedKey = Wire.read();
    if (receivedKey == 'C') {
      // Clear all conditions
      Wire.beginTransmission(2);  // Communicate with Slave 1
      Wire.write('C');            // Send reset command
      Wire.endTransmission();
      targetTable = 0;
      table = 0;
      start = false;
      success = false;
      Serial.println("All conditions cleared.");
      Stop();  // Stop the car if 'C' is pressed
    } else if (receivedKey >= '1' && receivedKey <= '6') {
      targetTable = receivedKey - '0';  // Convert char to int
      Serial.print("Target table set to: ");
      Serial.println(targetTable);
      start = true;  // Start table detection
    }
    receivedKey = 0;  // Clear the key
  } 

  // Request table data from slave (address 2)
  Wire.requestFrom(2, 1);
  if (Wire.available()) {
    delay(300); // Reduce CPU load
    table = Wire.read();  // Read the byte sent by the slave (table number)

    // Print the received data to Serial Monitor
    Serial.print("Received table from slave: ");
    Serial.println(table);
  }
  if(start == true){
    if(targetTable != 0){
      if(rotate == false){
        if (R_S == LOW && RM_S == HIGH && LM_S == HIGH && L_S == LOW) {
        forward();
        analogWrite(motorAspeed, 58);
        analogWrite(motorBspeed, 58);
        } 
        if (R_S == LOW && RM_S == HIGH && LM_S == LOW && L_S == LOW) {
        slightTurnRight();
        } 
        if (R_S == LOW && RM_S == LOW && LM_S == HIGH && L_S == LOW) {
       slightTurnLeft();
        }
      }  
    if(targetTable == 4 && rotate == false){
      if (R_S == LOW && RM_S == LOW && LM_S == LOW && L_S == LOW) {
       rotate = true;
      }
      if (R_S == HIGH && RM_S == HIGH && LM_S == HIGH && L_S == HIGH) {
       rotate = true;
      } 
    }
    }
  // Handle the received table numbeR

  // Car motion logic
  if (table == targetTable) {
    if (R_S == HIGH && RM_S == HIGH && LM_S == LOW && L_S == LOW) {
      turnRight();
    } 
    if (R_S == HIGH && RM_S == HIGH && LM_S == HIGH && L_S == LOW) {
      turnRight();
    } 
    if ((R_S == LOW && RM_S == LOW && LM_S == HIGH && L_S == HIGH)) {
      turnLeft();
    }
    if ((R_S == LOW && RM_S == HIGH && LM_S == HIGH && L_S == HIGH)) {
      turnLeft();
    } 
    if (R_S == HIGH && RM_S == HIGH && LM_S == HIGH && L_S == HIGH) {
      Stop();
      rotate = true;
  }
  }
  if(targetTable == 4 && rotate == true){
    if (R_S == LOW && RM_S == HIGH && LM_S == HIGH && L_S == LOW) {
      Stop();
      rotate = false;
    } 
    else if (R_S == LOW && RM_S == HIGH && LM_S == LOW && L_S == LOW) {
      Stop();
      rotate = false;
    } 
    else if (R_S == LOW && RM_S == LOW && LM_S == HIGH && L_S == LOW) {
      Stop();
      rotate = false;
  }
  else{
    if(table == 1|| table ==3){
      rotate_right();
      
    }
    if(table == 2){
       rotate_left();
    }
  }
  }
  }
}
