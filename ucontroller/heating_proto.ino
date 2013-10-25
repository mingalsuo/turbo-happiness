/**
 *  A simple monitor that just reports measured temperatures
 *  
 *  Mostly just a prototype at this stage and code reflects that.
 *  
 *  Functionality :         does what it needs
 *  Implementation :        leaves things to be desired
 *  Comments :              none
 *  Usefulness to others :  limited
 *  Code quality :          crap
 *  Quality as example :    frightening
 *  
 * 
 *  Copyright (C) 2013 Robert Ahlskog
 * 
 *  Distributed under the GNU GPL v3 license
 *  http://www.gnu.org/licenses/ 
 */

#include <Arduino.h>
#include <Servo.h>

Servo myservo;
int setValue = 0;
int outflow_raw[10];
int inflow_raw[10];
int guess[] = {0, 24, 36, 43, 48, 51, 54, 56, 57, 59, 60};

void getTemps(int n);
int calcBestGuess(int x);
void moveServo();
int inflow();
int outflow();

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println("Booting");
  for (int i = 0; i < 500; i++)
    Serial.print(".");
  Serial.println(" OK");
  Serial.print("Setting up IO");  
  pinMode(12, OUTPUT);
  myservo.attach(9);
  Serial.println("... OK");

  Serial.print("Loading initial data");
  for (int i = 0; i < 10; i++)
  {
    getTemps(i);
    Serial.print(".");
    delay(500);
  }
  Serial.println(" OK");
  
  int bg = calcBestGuess(outflow());
  setValue = 5*bg;
  moveServo();
}

void loop()
{
  delay(10000);
  for (int i = 0; i < 10; i++)
  {
    getTemps(i);
    delay(500);
  }
  Serial.print("IN=");
  Serial.print(inflow());
  Serial.print(", OUT=");
  Serial.print(outflow());
  Serial.print(", MIX=");
  Serial.println(setValue);
}

int calcBestGuess(int x)
{
  int i = 0;
  for (; i<11 ;i++)
  {
    if (x > guess[i]) continue;
    if (x == guess[i]) return i;
    int distA = guess[i] - x;
    int distB = x - guess[i-1];
    if (distA < distB)
      return i;
    else
      return i - 1;
  }
  return i;
}

int inflow()
{
  int x = 0;
  for (int i=0;i<10;i++)
    x += inflow_raw[i];
  return x / 10;
}
int outflow()
{
  int x = 0;
  for (int i=0;i<10;i++)
    x += outflow_raw[i];
  return x / 10;
}

void getTemps(int n)
{
  outflow_raw[n] = map(analogRead(0), 0, 1023, 0, 500);
  inflow_raw[n]  = map(analogRead(1), 0, 1023, 0, 500);
}

void upBy(int n)
{
  setValue += n;
  setValue = constrain(setValue, 0, 100);
  moveServo();
}

void downBy(int n)
{
  setValue -= n;
  setValue = constrain(setValue, 0, 100);
  moveServo();
}

void moveServo()
{
  setValue = constrain(setValue, 0, 100);
  int real = map(setValue, 0, 100, 150, 45);

  myservo.write(real);
  delay(250);
  digitalWrite(12, HIGH);
  delay(750);
  digitalWrite(12, LOW);
}
