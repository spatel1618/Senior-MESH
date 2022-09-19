/*
ESP8266 Analog MUX
Switches between 3 analog signals using the 74HC4066AP IC.

ESP GPIO 16 (D0) (CTRL1) -> IC pin 13
ESP GPIO 5 (D1) (CTRL2)  -> IC pin 5
ESP GPIO 4 (D2) (CTRL3)  -> IC pin 6

ESP (A0) (analogPin) -> IC pin 9
*/

//Outputs
#define CTRL1 16
#define CTRL2 5
#define CTRL3 4 

//Inputs
#define analogPin A0

int adcVal1 = 0;
int adcVal2 = 0;
int adcVal3 = 0;

void setup() {
  pinMode(CTRL1, OUTPUT);
  pinMode(CTRL2, OUTPUT);
  pinMode(CTRL3, OUTPUT);
  allLow();

  pinMode(analogPin, INPUT);
}

void loop() {
  digitalWrite(CTRL1, HIGH); // Pass Analog 1 value through MUX
  delay(1000); // Wait for a second
  adcVal1 =  analogRead(analogPin); // READ ANALOG 1 VALUE
  digitalWrite(CTRL1, LOW); // Stop Analog 1
  delay(1000); // Wait for a second

  digitalWrite(CTRL2, HIGH); // Pass Analog 2 value through MUX
  delay(1000); // Wait for a second
  adcVal2 =  analogRead(analogPin); // READ ANALOG 1 VALUE
  digitalWrite(CTRL2, LOW); // Stop Analog 2
  delay(1000); // Wait for a second

  digitalWrite(CTRL3, HIGH); // Pass Analog 3 value through MUX
  delay(1000); // Wait for a second
  adcVal3 =  analogRead(analogPin); // READ ANALOG 1 VALUE
  digitalWrite(CTRL3, LOW); // Stop Analog 3
  delay(1000); // Wait for a second
  
  allLow();
}

void allLow(){
    digitalWrite(CTRL1, LOW);
    digitalWrite(CTRL2, LOW);
    digitalWrite(CTRL3, LOW);
}
