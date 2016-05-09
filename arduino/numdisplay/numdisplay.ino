/**
 * All Copyrights reserved. © alvin.cpp@gmail.com 2016
 * 
 * Arduino with a number display.
 * Display shows a blinking number given from input via serial port.
 * It reads integer string, parses it and then display
 * 
 * Part of the program includes code from Spark Fun Electronics, please see below.
 */

int g_counter = 0;
void setup() {              
  Serial.begin(115200);  
  displayPinsSetup();
}

int lastv = 0;
long nexttime = 0;
void loop() {
  if(millis() > nexttime){ 
    int duration = lastv == g_counter ? 1000 : 2000;
    stayLightNumber(g_counter, duration);
    lastv = g_counter;
    nexttime = millis() + 1000;
  }
  
}

class SimpleNumber{
  private:
  String content;
  public:
  SimpleNumber(){
    Reset();
  }
  void addChar(char c){
    content += c;
  }
  int cLen(){
    return content.length();
  }
  int getInt(){
    return content.toInt();
  }
  void Reset(){
    content = "";
  }
  
};

void serialEvent(){
  SimpleNumber* SN = new SimpleNumber();
  while(Serial.available()){
    char inchar = Serial.read();
    if(inchar == '\n'){
      if(SN->cLen() > 0)
        g_counter = SN->getInt();
      else{
        SN->Reset();
        continue;
      }
    }else if(inchar == '+'){
      g_counter ++;
    }else if (isDigit(inchar)){
      SN->addChar(inchar);
      continue;
    }
    Serial.print(g_counter);
    Serial.println();
    SN->Reset();
  }
}


// this keeps default display brightness but last for ms2stay milliseconds
void stayLightNumber(int toDisplay, int ms2stay){
  long beginTime = millis();
  do{
    displayNumber(toDisplay);
  }while((millis() - beginTime) < (ms2stay));
}

/*
 6-13-2011
 Spark Fun Electronics 2011
 Nathan Seidle
 
 This code is public domain but you buy me a beer if you use this and we meet 
 someday (Beerware license).
 
 4 digit 7 segment display:
 http://www.sparkfun.com/products/9483
 Datasheet: 
 http://www.sparkfun.com/datasheets/Components/LED/7-Segment/YSD-439AR6B-35.pdf

 This is an example of how to drive a 7 segment LED display from an ATmega
 without the use of current limiting resistors. This technique is very common 
 but requires some knowledge of electronics - you do run the risk of dumping 
 too much current through the segments and burning out parts of the display. 
 If you use the stock code you should be ok, but be careful editing the 
 brightness values.
 
 This code should work with all colors (red, blue, yellow, green) but the 
 brightness will vary from one color to the next because the forward voltage 
 drop of each color is different. This code was written and calibrated for the 
 red color.

 This code will work with most Arduinos but you may want to re-route some of 
 the pins.

 7 segments
 4 digits
 1 colon
 =
 12 pins required for full control 
 
 */

int digit1 = 13; //PWM Display pin 1
int digit2 = 10; //PWM Display pin 2
int digit3 = 9; //PWM Display pin 6
int digit4 = 7; //PWM Display pin 8

//Pin mapping from Arduino to the ATmega DIP28 if you need it
//http://www.arduino.cc/en/Hacking/PinMapping
int segA = 12; //Display pin 14
int segB = 8; //Display pin 16
int segC = A0; //Display pin 13
int segD = 3; //Display pin 3
int segE = 2; //Display pin 5
int segF = 11; //Display pin 11
int segG = 6; //Display pin 15


#define DISPLAY_BRIGHTNESS  50

#define DIGIT_ON  LOW
#define DIGIT_OFF  HIGH

#define SEGMENT_ON  HIGH
#define SEGMENT_OFF LOW

void displayPinsSetup(){
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);

  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);
  
  pinMode(4, OUTPUT);
}

//Given a number, we display 10:22
//After running through the 4 numbers, the display is left turned off

//Display brightness
//Each digit is on for a certain amount of microseconds
//Then it is off until we have reached a total of 20ms for the function call
//Let's assume each digit is on for 1000us
//Each digit is on for 1ms, there are 4 digits, so the display is off for 16ms.
//That's a ratio of 1ms to 16ms or 6.25% on time (PWM).
//Let's define a variable called brightness that varies from:
//5000 blindingly bright (15.7mA current draw per digit)
//2000 shockingly bright (11.4mA current draw per digit)
//1000 pretty bright (5.9mA)
//500 normal (3mA)
//200 dim but readable (1.4mA)
//50 dim but readable (0.56mA)
//5 dim but readable (0.31mA)
//1 dim but readable in dark (0.28mA)

void displayNumber(int toDisplay) {

  long beginTime = millis();

  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, DIGIT_ON);
      break;
    case 2:
      digitalWrite(digit2, DIGIT_ON);
      break;
    case 3:
      digitalWrite(digit3, DIGIT_ON);
      break;
    case 4:
      digitalWrite(digit4, DIGIT_ON);
      break;
    }

    //Turn on the right segments for this digit
    lightNumber(toDisplay % 10);
    toDisplay /= 10;

    delayMicroseconds(DISPLAY_BRIGHTNESS); 
    //Display digit for fraction of a second (1us to 5000us, 500 is pretty good)

    //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, DIGIT_OFF);
    digitalWrite(digit2, DIGIT_OFF);
    digitalWrite(digit3, DIGIT_OFF);
    digitalWrite(digit4, DIGIT_OFF);
  }

  while( (millis() - beginTime) < 10) ; 
  //Wait for 20ms to pass before we paint the display again
}

//Given a number, turns on those segments
//If number == 10, then turn off number
void lightNumber(int numberToDisplay) {


  switch (numberToDisplay){

  case 0:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 1:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 2:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 3:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 4:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 5:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 6:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 7:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 8:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 9:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 10:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;
  }
}

