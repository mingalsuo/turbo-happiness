/**
 *  A simple monitor that just reports measured temperatures
 *  
 *  Mostly just a prototype at this stage and code reflects that.
 *  
 *  Functionality :         does less than what it should
 *  Implementation :        leaves things to be desired
 *  Comments :              not really that useful
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

/* Function prototypes to make the compiler happy */
void getTemps(int n);
int calcBestGuess(int x);
void moveServo();
int inflow();
int outflow();


/* Oh dear god, global variables!
 * What is this? Some kind of crippled environment without a heap and limited stack? Oh, right... */
Servo myservo;
int setValue = 0;
int outflow_raw[10];
int inflow_raw[10];
int guess[] = {0, 24, 36, 43, 48, 51, 54, 56, 57, 59, 60};


/* Some basic setup that needs to be done for life not to suck */
void setup() {
  // Delay a while to give the BT module a chance to init 
  // then setup the serial to 115200 so we have some common ground
  // and just shove some data up in there so it knows we are alive
  delay(1000);
  Serial.begin(115200);
  delay(250);
  Serial.println("Booting monitor-control");

  // Setup the IOs we will use
  pinMode(12, OUTPUT);
  myservo.attach(9);

  // This is fugly, why is it even here? Please clean it up before my eyes melt
  for (int i = 0; i < 10; i++)
  {
    getTemps(i);
    delay(500);
  }
  int bg = calcBestGuess(outflow());
  setValue = 5*bg;
  moveServo();
}

/* What should our main loop do? Well, probably empty spin waiting for something to do */
void loop()
{
  // probably not this
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

/* This was a really clever piece of code that does something */
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

/* Oh this is all wrong */
int inflow()
{
  int x = 0;
  for (int i=0;i<10;i++)
    x += inflow_raw[i];
  return x / 10;
}
/* even worse */
int outflow()
{
  int x = 0;
  for (int i=0;i<10;i++)
    x += outflow_raw[i];
  return x / 10;
}
/* Why was this a good idea again? */
void getTemps(int n)
{
  outflow_raw[n] = map(analogRead(0), 0, 1023, 0, 500);
  inflow_raw[n]  = map(analogRead(1), 0, 1023, 0, 500);
}

/* A mostly pointless method */
void upBy(int n)
{
  setValue += n;
  setValue = constrain(setValue, 0, 100);
  moveServo();
}
/* About as pointles as the one above */
void downBy(int n)
{
  setValue -= n;
  setValue = constrain(setValue, 0, 100);
  moveServo();
}
/* Finally something that does something, if only the cable was connected */
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
