/*
   OSC_Keyboard
   gets inputs from knobs and buttons and sends them via osc over usb serial
   also controls oled screen and midi inputs

*/

#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include <Adafruit_GFX.h>
#include <OSCMessage.h>

//Setup serial over usb or not
#ifdef BOARD_HAS_USB_SERIAL
#include <SLIPEncodedUSBSerial.h>
SLIPEncodedUSBSerial SLIPSerial( thisBoardsSerialUSB );
#else
#include <SLIPEncodedSerial.h>
SLIPEncodedSerial SLIPSerial(Serial1);
#endif

// OLED display TWI address
#define OLED_ADDR   0x3C

// reset pin not used on 4-pin OLED module
Adafruit_SSD1306 display(-1);  // -1 = no reset pin

int pinLast[12] = {0};
int analogLast[5] = {0};
int pinCurrent;
int analogCurrent;

#define KNOB1 7
#define KNOB2 6
#define KNOB3 2
#define KNOB4 1
#define KNOB5 3
#define BGCOLOR 0

#define NUM_KNOBS 6
const uint8_t KNOB_PINS[NUM_KNOBS] = {KNOB2, KNOB3, BGCOLOR, KNOB1, KNOB4, KNOB5};

#define NEXT_MODE 3
#define PREV_MODE 5
#define TRIG_BUTTON 10
#define PREV_SCENE 6
#define NEXT_SCENE 7
#define SAVE_DELETE_SCENE 8
#define SET_OSD 2
#define AUTO_CLEAR 1
#define SCREEN_GRAB 9
#define EXTRA1 4
#define EXTRA2 11
#define POWER 0

#define NUM_BUTTONS 12
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {POWER, PREV_MODE, NEXT_MODE, SET_OSD, PREV_SCENE, NEXT_SCENE, SAVE_DELETE_SCENE, SCREEN_GRAB, AUTO_CLEAR, TRIG_BUTTON, EXTRA1, EXTRA2};

//setup debounce for button inputs
Bounce * buttons = new Bounce[NUM_BUTTONS];

void setup() {
  int i;

  for (i = 0; i < NUM_BUTTONS; i++)  {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  //begin SLIPSerial just like Serial
  SLIPSerial.begin(112000);   // set this as high as you can reliably run on your platform
#if ARDUINO >= 100
  while (!Serial)
    ; //Leonardo "feature"
#endif

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach( BUTTON_PINS[i] , INPUT );       //setup the bounce instance for the current button
    buttons[i].interval(25);              // interval in ms
  }

  //init the display and show the default boot screen
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);

  display.clearDisplay();
  display.display();  // display a line of text
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.print("ectera");

  // update display with all of the above graphics
  display.display();
  delay(2000);

}

void loop() {
  int analogUpdate = 0;
  int i;

  //send any knob updates over osc
  OSCMessage msg("/knobs");
  //-1 to disable the 5th knob for now
  for (i = 0; i < NUM_KNOBS-1; i++ ) {
    analogCurrent = analogRead(KNOB_PINS[i]);

    if (abs(analogLast[i] - analogCurrent) > 7) {
      analogUpdate = 1;
      analogLast[i] = analogCurrent;
    }
  }

  if (analogUpdate == 1 ) {
    analogUpdate = 0;
    for (i = 0; i < 6; i++ ) {
      msg.add((int32_t) analogLast[i]);
    }

    SLIPSerial.beginPacket();
    msg.send(SLIPSerial); // send the bytes to the SLIP stream
    SLIPSerial.endPacket(); // mark the end of the OSC Packet
    msg.empty(); // free space occupied by message
    delay(20);
  }


  //send any button presses over osc
  for (i = 0; i < NUM_BUTTONS; i++)  {

    // Update the Bounce instance :
    buttons[i].update();

    if (buttons[i].read() == HIGH) {
      pinCurrent = 0;
    } else {
      pinCurrent = 1;
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

  //draw the display with the last status
  updateDisplay();

}

void updateDisplay(void) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(1, 1);
  display.print("ectera..  :)");
  display.setCursor(100, 1);
  //display.setTextSize(2);
  //display.print(":)");

  //draw the circles and fill them if the button is pressed
  if (pinLast[3] == 1) {
    display.fillCircle(103, 10, 5, WHITE);
  } else {
    display.drawCircle(103, 10, 5, WHITE);
  }
  if (pinLast[8] == 1) {
    display.fillCircle(103, 23, 5, WHITE);
  } else {
    display.drawCircle(103, 23, 5, WHITE);
  }

  if (pinLast[1] == 1) {
    display.fillCircle(10, 50, 5, WHITE);
  } else {
    display.drawCircle(10, 50, 5, WHITE);
  }
  if (pinLast[10] == 1) {
    display.fillCircle(20, 40, 5, WHITE);
  } else {
    display.drawCircle(20, 40, 5, WHITE);
  }
  if (pinLast[2] == 1) {
    display.fillCircle(30, 50, 5, WHITE);
  } else {
    display.drawCircle(30, 50, 5, WHITE);
  }

  if (pinLast[4] == 1) {
    display.fillCircle(50, 50, 5, WHITE);
  } else {
    display.drawCircle(50, 50, 5, WHITE);
  }
  if (pinLast[6] == 1) {
    display.fillCircle(60, 40, 5, WHITE);
  } else {
    display.drawCircle(60, 40, 5, WHITE);
  }
  if (pinLast[5] == 1) {
    display.fillCircle(70, 50, 5, WHITE);
  } else {
    display.drawCircle(70, 50, 5, WHITE);
  }

  if (pinLast[7] == 1) {
    display.fillCircle(90, 50, 5, WHITE);
  } else {
    display.drawCircle(90, 50, 5, WHITE);
  }
  if (pinLast[11] == 1) {
    display.fillCircle(100, 40, 5, WHITE);
  } else {
    display.drawCircle(100, 40, 5, WHITE);
  }
  if (pinLast[9] == 1) {
    display.fillCircle(110, 50, 5, WHITE);
  } else {
    display.drawCircle(110, 50, 5, WHITE);
  }

  display.drawRect(28, 19, 10, 12, WHITE);
  display.drawRect(39, 19, 10, 12, WHITE);
  display.drawRect(50, 19, 10, 12, WHITE);
  display.drawRect(61, 19, 10, 12, WHITE);
  display.drawRect(72, 19, 10, 12, WHITE);
  display.drawRect(83, 19, 10, 12, WHITE);

  display.fillRect(28, 20, 10, (10 - round(analogLast[5] / 100)), WHITE);
  display.fillRect(39, 20, 10, (10 - round(analogLast[3] / 100)), WHITE);
  display.fillRect(50, 20, 10, (10 - round(analogLast[0] / 100)), WHITE);
  display.fillRect(61, 20, 10, (10 - round(analogLast[1] / 100)), WHITE);
  display.fillRect(72, 20, 10, (10 - round(analogLast[4] / 100)), WHITE);
  display.fillRect(83, 20, 10, (10 - round(analogLast[2] / 100)), WHITE);

  display.display();
}
