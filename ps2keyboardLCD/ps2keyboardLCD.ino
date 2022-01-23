#include <SoftwareSerial.h>

// LCD Documentation: https://www.jameco.com/Jameco/Products/ProdDS/387911.pdf

// Keyboard constants
#define CLOCKPIN 3
#define DATAPIN 9
#define MAXSTRSIZE 256

// LCD constants
#define RXPIN 11
#define TXPIN 10
#define DELAY_TIME 10

SoftwareSerial interface(RXPIN, TXPIN);

unsigned int rawData = 0;
unsigned int bitsCollected = 0;
char scanCodes[] = {(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,'Q','1',(char)0,(char)0,(char)0,'Z','S','A','W','2',(char)0,(char)0,'C','X','D','E','4','3',(char)0,(char)0,' ','V','F','T','R','5',(char)0,(char)0,'N','B','H','G','Y','6',(char)0,(char)0,(char)0,'M','J','U','7','8',(char)0,(char)0,(char)0,'K','I','O','0','9',(char)0,(char)0,(char)0,(char)0,'L',(char)0,'P',(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0,(char)0};
char string[MAXSTRSIZE];
int charsPrinted = 0;
bool canPrintChar = false;

void setup() {
  string[0] = (char)0;
  pinMode(CLOCKPIN, INPUT);
  pinMode(DATAPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLOCKPIN), readDataBit, FALLING); // attach an interrupt to the clock pin (the clock signal goes low at a data bit)
  Serial.begin(9600); // initialize hardware serial interface

  interface.begin(9600); // initialize software serial interface for LCD
  delay(1000);
  interface.write(24); // initialize LCD
}

void loop() {
  delay(1000);
  Serial.println(string);
  interface.write(12); // clears the entire display
  delay(DELAY_TIME); // the documentation wants you to wait 5ms after clearing, so I'm waiting for 10
  for (int i = 0; i < MAXSTRSIZE; i++) {
    if (string[i]) {
      interface.write(string[i]);
    }
    else break;
  }
  bitsCollected = 0; // resets the bit buffer periodically so that there aren't bits in there when there shouldn't be
  rawData = 0;
}
void readDataBit()
{
  rawData <<= 1; // adds new bit to bit buffer
  rawData += digitalRead(DATAPIN);
  bitsCollected++;
  if (bitsCollected == 11)
  {
    unsigned int scanCode = 0;
    while (bitsCollected > 0) // prints the bits in the same order as shown on the LEDs in the video, and in the same order that you should interpret them in.
    {
      //Serial.print(rawData % 2); // uncomment to print the raw bits coming from the keyboard as they are in the video
      scanCode += (rawData % 2) * (unsigned int)(1 << (bitsCollected - 1));
      rawData >>= 1; // bitshift right
      bitsCollected--;
    }
    
    Serial.println(scanCodes[(scanCode % 0b1000000000) >> 1]); // debugging statements
    Serial.println((scanCode % 0b1000000000) >> 1);
    
    if (scanCodes[(scanCode % 0b1000000000) >> 1]) {
      canPrintChar = !canPrintChar; // the character is sent twice by the keyboard, so this switches back and forth to say when the character can be printed
      for (int i = 0; i < MAXSTRSIZE - 1; i++) 
      {
        if (string[i] == (char)0) // if end of string is found
        {
          if (canPrintChar) {
            string[i] = scanCodes[(scanCode % 0b1000000000) >> 1]; // remove the parity and error checking bits
            string[i+1] = (char)0;
          }
          break;
        }
      }
    }
  }
}
// I wrote this program using information from Ben Eater's video on the PS/2 keyboard interface
// https://www.youtube.com/watch?v=7aXbh9VUB3U
