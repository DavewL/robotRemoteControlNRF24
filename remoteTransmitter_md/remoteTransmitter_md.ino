#include <RF24_config.h>
#include <printf.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;
unsigned long loopCounter = 0;
bool flash = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

void setup() {

  pinMode(4, OUTPUT);
  digitalWrite(4, 0);
  //Serial.begin(9600);
  radio.begin();

  //RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  
  // Start the radio listening for data
  radio.startListening();

}

/********************** Main Loop *********************/
void loop() {

  byte axes[2];

  //digitalWrite(4, 0);
  
  radio.stopListening();                    // First, stop listening so we can talk.

  if (loopCounter%50 == 0){
    ///read joystick position message
//    axes[0] = map(analogRead(A0), 0, 1023, 0, 255);
//    axes[1] = map(analogRead(A1), 0, 1023, 0, 255);
    axes[0] = map(analogRead(A2), 0, 1023, 0, 255);
    axes[1] = map(analogRead(A3), 0, 1023, 0, 255);
    
    if (!radio.write( &axes, (2*sizeof(byte)))){
      //Serial.println(F("failed"));
    }
    radio.startListening();

    //Serial.println(analogRead(A2));
    //Serial.println("test");
    
  }
  
  unsigned long started_waiting_at = micros();
  boolean timeout = false;
  
  while (!radio.available()){
    if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
          timeout = true;
          break;
    }
    else{
      //flash = 0; 
    }
  }
  if ( timeout ){                                             // Describe the results
    flash = !flash;
     digitalWrite(4, flash);
    //Serial.println(F("Failed, response timed out."));
    //Serial.print("Y-Axis: ");
    //Serial.println(analogRead(A2));
    //Serial.print("X-Axis: ");
    //Serial.println(analogRead(A3));
  
  }else{
    byte response[2];                                 // Grab the response, compare, and send to debugging spew
    radio.read(&response, (2*sizeof(byte)));
    //digitalWrite(4, 0);
  }
  
  loopCounter++;

}

