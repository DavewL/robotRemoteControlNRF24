#include <RF24_config.h>
#include <printf.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;
unsigned long loopCounter = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

void setup() {
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

  byte axes[4];
  
  radio.stopListening();                    // First, stop listening so we can talk.

  if (loopCounter%50 == 0){
    ///read joystick position message
    axes[0] = map(analogRead(A0), 0, 1023, 0, 255);
    axes[1] = map(analogRead(A1), 0, 1023, 0, 255);
    axes[2] = map(analogRead(A2), 0, 1023, 0, 255);
    axes[3] = map(analogRead(A3), 0, 1023, 0, 255);
    
    if (!radio.write( &axes, 32)){
      Serial.println(F("failed"));
    }
    radio.startListening();
  }
  
  unsigned long started_waiting_at = micros();
  boolean timeout = false;
  
  while (!radio.available()){
    if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
          timeout = true;
          break;
    } 
  }
  if ( timeout ){                                             // Describe the results
    Serial.println(F("Failed, response timed out."));
  }else{
    unsigned long response;                                 // Grab the response, compare, and send to debugging spew
    radio.read(&response, sizeof(unsigned long));
  }
  
  loopCounter++;

}

