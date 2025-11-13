/*

Components that we're implementing
1. IR Sensor + Remote
2. HC-SR04 Ultrasonic Distance Sensor
3. RGB LED
4. Active Buzzer
5. Servo

The servo (5) is to be controlled by the IR sensor + remote (1). 
The ultrasonic distance sensor is attached to the servo and scans the table. 
Passive Buzzer beeping increases in frequency and RGB LED changes in color as distance readings change. 
- Closer = higher frequency sound, red
- Medium = medium frequency sound, green
- Far = low frequency sound, blue

For shits and giggles, make it sound like a Geiger counter ("radioactive").

Pin #s - Function
-----------------
2        - IR receiver

3 (PWM)  - passive buzzer

5 (PWM)  - red for RGB
10 (PWM) - green for RGB
11 (PWM) - blue for RGB

7        - HC-SR04 echo pin
8        - HC-SR04 trigger pin

9 (PWM)  - servo

*/

/* PART 1: IMPORT NECESSARY LIBRARIES & DECLARE/INITIALIZE VARIABLES */
#include "IRremote.h" // ensure that the version is 2.6.0
#include "Servo.h"
#include "math.h"

// IR Receiver
const int receiver = 2; 
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

// For the distance sensor
const int echoPin = 7;
const int trigPin = 8;
long duration, distance;

// For the servo
const int servoPin = 9;
Servo servo_3;
int angle = 0;

// RGB LED
const int redPin = 5;
const int greenPin = 10;
const int bluePin = 11;

// For the passive buzzer
const int buzzerPin = 3;
int frequency; // scales linearly with the distance
int k = 1; // proportionality constant

void translateIR() 
{
  switch(results.value) // takes action based on value. 
  {
    case 0xFFA25D: Serial.println("POWER"); break;
    case 0xFFE21D: Serial.println("FUNC/STOP"); break;
    case 0xFF629D: Serial.println("VOL+"); break;
    case 0xFF22DD: Serial.println("FAST BACK"); angle -= 30;  break; // subtract 15 deg. from the angle
    case 0xFF02FD: Serial.println("PAUSE");    break;
    case 0xFFC23D: Serial.println("FAST FORWARD"); angle += 30; break; // add 15 deg. to the angle
    case 0xFFE01F: Serial.println("DOWN");    break;
    case 0xFFA857: Serial.println("VOL-");    break;
    case 0xFF906F: Serial.println("UP");    break;
    case 0xFF9867: Serial.println("EQ");    break;
    case 0xFFB04F: Serial.println("ST/REPT");    break;
    case 0xFF6897: Serial.println("0");    break;
    case 0xFF30CF: Serial.println("1");    break;
    case 0xFF18E7: Serial.println("2");    break;
    case 0xFF7A85: Serial.println("3");    break;
    case 0xFF10EF: Serial.println("4");    break;
    case 0xFF38C7: Serial.println("5");    break;
    case 0xFF5AA5: Serial.println("6");    break;
    case 0xFF42BD: Serial.println("7");    break;
    case 0xFF4AB5: Serial.println("8");    break;
    case 0xFF52AD: Serial.println("9");    break;
    case 0xFFFFFFFF: Serial.println("REPEAT"); break;  

    default: 
      Serial.println(" other button : ");
      Serial.println(results.value);
  }
  delay(200);
} 

long readDistance(int trig, int echo) {
  // Sending pulses
  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  // Reading the reaction
  pinMode(echo, INPUT);
  duration = pulseIn(echo, HIGH);
  return duration * 0.01723;
}

void setup()
{
  // Begin serial monitor.
  Serial.begin(9600);

  // Initialize the IR receiver.
  Serial.println("IR Receiver Button Decode"); 
  irrecv.enableIRIn(); // Start the receiver

  // Initialize the servo. 
  servo_3.attach(servoPin, 500, 2500);
  pinMode(receiver, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  pinMode(buzzerPin, OUTPUT);
} 

void loop()
{
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    translateIR(); // translate IR signal
    irrecv.resume(); // receive the next value
  }  

  angle = constrain(angle, 0, 180); // limits the angle from 0 to 180 deg. 
  servo_3.write(angle); // writes the angle to servo
  // Serial.println(angle);


  // Sets the color of the LED according to the distance reading
  if (readDistance(trigPin, echoPin) <= 10) {
    analogWrite(redPin, 255);
    analogWrite(bluePin, 0);
    analogWrite(greenPin, 0);
  } else if (readDistance(trigPin, echoPin) <= 25) {
    analogWrite(greenPin, 255);
    analogWrite(bluePin, 0);
    analogWrite(redPin, 0);
  } else {
    analogWrite(bluePin, 255);
    analogWrite(greenPin, 0);
    analogWrite(redPin, 0);
  }
  
  // Sets the period of the sound to directly proprtional to the distance reading
  frequency = (int) (k * readDistance(trigPin, echoPin) + 1);
  
  digitalWrite(buzzerPin, HIGH);
  delay(frequency);
  digitalWrite(buzzerPin, LOW);
  delay(frequency);
  
}