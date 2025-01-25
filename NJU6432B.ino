/**********************************************************************/
/*This code is not clean and OFFCOURSE will be possible do it better! */ 
/*this is only a group of functions to be used as independent code,  */ 
/*and by this way will be possible to any person can do the changes   */ 
/*of code and see what happen.                                         */
/*The code don't depend of any external library or extenal functions  */
/*complicated.                                                         */
/*I'm let the values to sent as binnary, this allow swap bit by bit */
/* to is possible test segment by segment without convert to HEX    */
/**********************************************************************/
//
#define VFD_in 8  // This is the pin number 8 on Arduino UNO  // You can use a resistor of 1Kohms to protect this line!
#define VFD_clk 9 // This is the pin number 9 on Arduino UNO  // You can use a resistor of 1Kohms to protect this line!
#define VFD_ce 10 // This is the pin number 10 on Arduino UNO // You can use a resistor of 1Kohms to protect this line!


unsigned int shiftBit=0;
unsigned int nBitOnBlock=0; // Used to count number of bits and split to 8 bits... (number of byte)
unsigned int nByteOnBlock=0; 
unsigned int sequencyByte=0x00;
byte Aa,Ab,Ac,Ad,Ae,Af,Ag;
byte blockBit =0x00;

#define BUTTON_PIN 2 //Att check wich pins accept interrupts... Uno is 2 & 3
volatile byte buttonReleased = false;

// variables will change:
bool forward = false;
bool backward = false;
bool isRequest = true;
bool allOn=false;
bool cycle=false;
/*

//ATT: On the Uno and other ATMEGA based boards, unsigned ints (unsigned integers) are the same as ints in that they store a 2 byte value.
//Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647.
/*************************************************/
void setup() {
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_in, OUTPUT);
  pinMode(VFD_ce, OUTPUT);
 
  //initialize the LED pin as an output:
    pinMode(13, OUTPUT);
  //initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  //Next line is the attach of interruption to pin 2
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),
                    buttonReleasedInterrupt,
                    FALLING);
  //
  Serial.begin(115200); // only to debug  
  Serial.println("You reach a Reset Hardware!");               
}
void send_char(unsigned char a){
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  // the validation of data happen when clk go from LOW to HIGH.
  // This lines is because the clk have one advance in data, see datasheet of sn74HC595
  // case don't have this signal instead of "." will se "g"
  digitalWrite(VFD_ce, LOW); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  digitalWrite(VFD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
  for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
  digitalWrite(VFD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(5);
    //
    digitalWrite(VFD_ce, HIGH); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  }
}
/*********************************************************************/
// I h've created 3 functions to send bit's, one with strobe, other without strobe and one with first byte with strobe followed by remaing bits.
void send_char_without(unsigned char a){
 unsigned char data = 170; //value to transmit, binary 10101010 to test
 unsigned char mask = 1; //our bitmask
  data=a;
      for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
      digitalWrite(VFD_clk, LOW);
      delayMicroseconds(1);
        if (data & mask){ // if bitwise AND resolves to true
          digitalWrite(VFD_in, HIGH);
          //Serial.print(1); //Only to debug
        }
        else{ //if bitwise and resolves to false
          digitalWrite(VFD_in, LOW);
          //Serial.print(0); //Only to debug
        }
        digitalWrite(VFD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
        delayMicroseconds(1);
      }
  digitalWrite(VFD_clk, LOW);
  //Serial.println(); //Only to debug
}
/*********************************************************************/
void send_char_8bit_stb(unsigned char a){
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
 int i = -1;
  data=a;
  // This lines is because the clk have one advance in data, see datasheet of sn74HC595
  // case don't have this signal instead of "." will se "g"
  for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
   i++;
   digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// 
    delayMicroseconds(1);
    if (i==7){
    //Serial.println(i);
    digitalWrite(VFD_ce, HIGH);
    delayMicroseconds(1);
    }  
  }
}
/*******************************************************************/
void allON(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(1);
  // On the 75824 of Sony the message have first 8 bits more 64. Max 4 times to performe 256 bits( correspond send 4 times 8+64 bits
  send_char_without(0B11111110);  send_char_without(0B11111111); //  8:1   -16:9// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
  send_char_without(0B11111111);  send_char_without(0B11111111); // 24:17  -32:25// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
  send_char_without(0B11111111);  send_char_without(0B11111111); // 40:33  -48:41// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
  send_char_without(0B00101111);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  delayMicroseconds(1); 
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(60); //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); // 
  // On the 7582 of Sanyo the message 
  send_char_without(0B11111111);  send_char_without(0B11111111); // 61:54  -69:62// 
  send_char_without(0B11111111);  send_char_without(0B11111111); // 77:70  -85:78// 
  send_char_without(0B11111111);  send_char_without(0B11111111); // 93:86 -101:94//  
  send_char_without(0B10001111);  // 109:102  // 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void allOFF(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B00000000); //  8:1   -16:9//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 24:17  -32:25//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 40:33  -48:41//  
  send_char_without(0B00100000);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 61:54  -69:62//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 77:70  -85:78//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 93:86 -101:94//  
  send_char_without(0B10000000);  //109:102   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
/**************************************************************************/
void msgHello(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B01000000); //  8:1   -16:9//  
  send_char_without(0B00001111);  send_char_without(0B10000000); // 24:17  -32:25//  
  send_char_without(0B00000011);  send_char_without(0B00000000); // 40:33  -48:41//  
  send_char_without(0B00000000);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B01000000);  send_char_without(0B01110111); // 64:57  -72:65//  
  send_char_without(0B00110111);  send_char_without(0B00000011); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void number0(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B10000000); //  8:1   -16:9//  
  send_char_without(0B10111011);  send_char_without(0B10111011); // 24:17  -32:25//  
  send_char_without(0B10111011);  send_char_without(0B01111011); // 40:33  -48:41//  
  send_char_without(0B00010111);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B01110111);  send_char_without(0B01110111); // 64:57  -72:65//  
  send_char_without(0B01110111);  send_char_without(0B00000011); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void number1(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B00000000); //  8:1   -16:9//  
  send_char_without(0B00110011);  send_char_without(0B00110011); // 24:17  -32:25//  
  send_char_without(0B00110011);  send_char_without(0B00110011); // 40:33  -48:41//  
  send_char_without(0B00100000);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 64:57  -72:65//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void number2(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B11000000); //  8:1   -16:9//  
  send_char_without(0B11101110);  send_char_without(0B11101110); // 24:17  -32:25//  
  send_char_without(0B11101110);  send_char_without(0B01101110); // 40:33  -48:41//  
  send_char_without(0B00100101);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B01010101);  send_char_without(0B01010101); // 64:57  -72:65//  
  send_char_without(0B01010101);  send_char_without(0B00000001); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void number3(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B11000000); //  8:1   -16:9//  
  send_char_without(0B11111111);  send_char_without(0B11111111); // 24:17  -32:25//  
  send_char_without(0B11111111);  send_char_without(0B01111111); // 40:33  -48:41//  
  send_char_without(0B00000100);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B01000100);  send_char_without(0B01000100); // 64:57  -72:65//  
  send_char_without(0B01000100);  send_char_without(0B00000000); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void number4(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B01000000); //  8:1   -16:9//  
  send_char_without(0B01110111);  send_char_without(0B01110111); // 24:17  -32:25//  
  send_char_without(0B01110111);  send_char_without(0B00110111); // 40:33  -48:41//  
  send_char_without(0B00100010);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B00100010);  send_char_without(0B00100010); // 64:57  -72:65//  
  send_char_without(0B00100010);  send_char_without(0B00000010); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void number5(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B11000000); //  8:1   -16:9//  
  send_char_without(0B11011101);  send_char_without(0B11011101); // 24:17  -32:25//  
  send_char_without(0B11011101);  send_char_without(0B01011101); // 40:33  -48:41//  
  send_char_without(0B00100110);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B01100110);  send_char_without(0B01100110); // 64:57  -72:65//  
  send_char_without(0B01100110);  send_char_without(0B00000010); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void number6(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B11000000); //  8:1   -16:9//  
  send_char_without(0B11011101);  send_char_without(0B11011101); // 24:17  -32:25//  
  send_char_without(0B11011101);  send_char_without(0B01011101); // 40:33  -48:41//  
  send_char_without(0B00100111);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B01110111);  send_char_without(0B01110111); // 64:57  -72:65//  
  send_char_without(0B01110111);  send_char_without(0B00000011); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void number7(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B10000000); //  8:1   -16:9//  
  send_char_without(0B10111011);  send_char_without(0B10111011); // 24:17  -32:25//  
  send_char_without(0B10111011);  send_char_without(0B00111011); // 40:33  -48:41//  
  send_char_without(0B00100000);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 64:57  -72:65//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void number8(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B11000000); //  8:1   -16:9//  
  send_char_without(0B11111111);  send_char_without(0B11111111); // 24:17  -32:25//  
  send_char_without(0B11111111);  send_char_without(0B01111111); // 40:33  -48:41//  
  send_char_without(0B00000111);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B01110111);  send_char_without(0B01110111); // 64:57  -72:65//  
  send_char_without(0B01110111);  send_char_without(0B00000011); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void number9(){
  delayMicroseconds(5);
  digitalWrite(VFD_ce, HIGH); //
  delayMicroseconds(2);
  // On the 7582 
  send_char_without(0B00000000);  send_char_without(0B11000000); //  8:1   -16:9//  
  send_char_without(0B11111111);  send_char_without(0B11111111); // 24:17  -32:25//  
  send_char_without(0B11111111);  send_char_without(0B01111111); // 40:33  -48:41//  
  send_char_without(0B00100110);  //56,55,54,53:49   //Bit: 54=Dp, 55=Dq,56=Group 0 or 1
  //Note: The Dp is to set at 1/2 or 1/1, means 53 or 106 segments
  //Note: the Dq is to define the pins to Segments or Analog pins
  //Last  bit is to define first group or second! 
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
  delayMicroseconds(6);  //ON datasheet they recomend this time (T4) at minimum of 4uSec
  digitalWrite(VFD_ce, HIGH); //
  // On the 7582 of Sanyo the message 
  send_char_without(0B01100110);  send_char_without(0B01100110); // 64:57  -72:65//  
  send_char_without(0B01100110);  send_char_without(0B00000010); // 80:73  -88:81//  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 96:89 -104:97//  
  send_char_without(0B10000000);  //112:105   //  
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW); // 
}
void searchOfSegments(){
  //your main code here, to run repeatedly:
  uint8_t group = 0x00;
  uint8_t nBit =0x00;
  uint8_t nMask = 0b00000001;
  unsigned int block =0;
  unsigned int nSeg=0x00;  
  Serial.println();
  Serial.println("We start the test of segments!");
    for(block = 0; block < 2; block++){
        for( group = 0; group < 7; group++){   // Do until n bits 7*8 bits
        //for(int nBit=0; nBit<8; nBit++){
                      for (nMask = 0b00000001; nMask>0; nMask <<= 1){
                        Aa=0x00; Ab=0x00; Ac=0x00; Ad=0x00; Ae=0x00; Af=0x00; Ag=0x00; 
                              switch (group){
                                case 0: Aa=nMask; break;
                                case 1: Ab=nMask; break;
                                case 2: Ac=nMask; break;
                                case 3: Ad=nMask; break;
                                case 4: Ae=nMask; break;
                                case 5: Af=nMask; break;
                                case 6: Ag=nMask; break;
                              }    
                      
                      if((nSeg >=0) && (nSeg<57)){
                        blockBit=0;
                        }
                        if((nSeg >=57) && (nSeg<112)){
                        blockBit=1;
                        }  
                        nSeg++;
                        //This start the control of button to allow continue teste! 
                                  while(1){
                                        if(!buttonReleased){
                                          delay(200);
                                        }
                                        else{
                                          delay(15);
                                          buttonReleased = false;
                                          break;
                                          }
                                    }
                        delayMicroseconds(5);
                        segments();

                        Serial.print(nSeg, DEC); Serial.print(", group: "); Serial.print(group, DEC);Serial.print(", BlockBit: "); Serial.print(blockBit, HEX);Serial.print(", nMask: "); Serial.print(nMask, BIN);Serial.print("   \t");
                        // Serial.print(Ag, HEX);Serial.print(", ");Serial.print(Af, HEX);Serial.print(", ");Serial.print(Ae, HEX);Serial.print(", ");Serial.print(Ad, HEX);Serial.print(", ");
                        // Serial.print(Ac, HEX);Serial.print(", ");Serial.print(Ab, HEX);Serial.print(", ");Serial.print(Aa, HEX); Serial.print("; ");
                        
                        Serial.println();
                        delayMicroseconds (2);  
                      }         
        } 
   }       
}
void segments(){
          digitalWrite(VFD_ce, HIGH); //
          delayMicroseconds(2);
          // 
          send_char_without(Aa);  send_char_without(Ab);  //   8:1     -16:9// 
          send_char_without(Ac);  send_char_without(Ad);  //  24:17    -32:25//
          send_char_without(Ae);  send_char_without(Af);  //  40:33    -48:41// 
          //send_char_without(Ag);  //  52:49   //                    
          send_char_without((0B00000000 | Ag) & 0x1F); 
          delayMicroseconds(2);
          digitalWrite(VFD_ce, LOW); //
          delayMicroseconds(2);
          digitalWrite(VFD_ce, HIGH); //
          delayMicroseconds(2);
          // 
          send_char_without(Aa);  send_char_without(Ab);  //   8:1     -16:9// 
          send_char_without(Ac);  send_char_without(Ad);  //  24:17    -32:25//
          send_char_without(Ae);  send_char_without(Af);  //  40:33    -48:41// 
          //send_char_without(Ag);  //  52:49   //  
          send_char_without(0B10000000| Ag);
          delayMicroseconds(1);
          digitalWrite(VFD_ce, LOW); //
          delayMicroseconds(1);
}
void loop() {
    for(int i=0; i<4; i++){
      allON();
      delay(500);
      allOFF();
      delay(500);
    }
    msgHello();
    delay(1000);
    allON();
    delay(1000);
    number0();
    delay(500);
    number1();
    delay(500);
    number2();
    delay(500);
    number3();
    delay(500);
    number4();
    delay(500);
    number5();
    delay(500);
    number6();
    delay(500);
    number7();
    delay(500);
    number8();
    delay(500);
    number9();
    delay(500);
   
   //searchOfSegments(); //Uncomment this line if you want run to find segments 
}
void buttonReleasedInterrupt() {
  buttonReleased = true; // This is the line of interrupt button to advance one step on the search of segments!
}
