#include <OSCMessage.h>

/*
Make an OSC message and send it over serial
 */

#ifdef BOARD_HAS_USB_SERIAL
#include <SLIPEncodedUSBSerial.h>
SLIPEncodedUSBSerial SLIPSerial( thisBoardsSerialUSB );
#else
#include <SLIPEncodedSerial.h>
 SLIPEncodedSerial SLIPSerial(Serial1);
#endif

int pinLast[12] = {0};
int analogLast[5] = {0};
int pinCurrent;
int analogCurrent;

void setup() {
  //begin SLIPSerial just like Serial
  SLIPSerial.begin(9600);   // set this as high as you can reliably run on your platform
#if ARDUINO >= 100
  while(!Serial)
    ; //Leonardo "feature"
#endif
pinMode(0, INPUT);
pinMode(1, INPUT);
pinMode(2, INPUT);
pinMode(3, INPUT);
pinMode(4, INPUT);
pinMode(5, INPUT);
pinMode(6, INPUT);
pinMode(7, INPUT);
pinMode(8, INPUT);
pinMode(9, INPUT);
pinMode(10, INPUT);
pinMode(11, INPUT);

}

void loop(){
  int analogUpdate = 0;
  int i;
  
  OSCMessage msg("/knobs");
  for (i=0; i<6; i++ ) { 
    analogCurrent = analogRead(i);
    //if (analogCurrent > 100) { analogCurrent = 100; }

    if (abs(analogLast[i] - analogCurrent) > 15) {
      analogUpdate = 1;
      analogLast[i] = analogCurrent;
    }
  }

  if (analogUpdate == 1) {
    analogUpdate = 0;
    for (i=0; i<6; i++ ) { 
      msg.add((int32_t) analogLast[i]);
    }
    
    SLIPSerial.beginPacket();  
    msg.send(SLIPSerial); // send the bytes to the SLIP stream
    SLIPSerial.endPacket(); // mark the end of the OSC Packet
    msg.empty(); // free space occupied by message
    delay(20);  
  }

    
  //the message wants an OSC address as first argument
  for (i=0; i<12; i++ ) { 

    if (digitalRead(i) == HIGH) { 
      pinCurrent = 1;
    } else {
      pinCurrent = 0;
    }

    if (pinLast[i] != pinCurrent) {
      OSCMessage msg("/key");
      if (pinCurrent == 1) {
        msg.add((int32_t) i);
        msg.add((int32_t) 100);
      } else {
        msg.add((int32_t) i);
        msg.add((int32_t) 0);   
      }

      SLIPSerial.beginPacket();  
      msg.send(SLIPSerial); // send the bytes to the SLIP stream
      SLIPSerial.endPacket(); // mark the end of the OSC Packet
      msg.empty(); // free space occupied by message
      delay(20);
    }
    pinLast[i] = pinCurrent;
    
  }
  //for (i = 0; i < 12; i++) {
  //  Serial.println(pinLast[i]);
  //}
}
