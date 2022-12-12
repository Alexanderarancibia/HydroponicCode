// Example of drawing a graphical "switch" and using
// the touch screen to change it's state.

// This sketch does not use the libraries button drawing
// and handling functions.

// Based on Adafruit_GFX library onoffbutton example.

// Touch handling for XPT2046 based screens is handled by
// the TFT_eSPI library.

// Calibration data is stored in SPIFFS so we need to include it
#include "FS.h"

#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

// This is the file name used to store the touch coordinate
// calibration data. Change the name to start a new calibration.
#define CALIBRATION_FILE "/TouchCalData3"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

bool SwitchOn = false;

// Comment out to stop drawing black spots
//#define BLACK_SPOT

// Switch position and size
#define FRAME_X 20
#define FRAME_Y 40
#define FRAME_W 150
#define FRAME_H 50

// Red zone size
#define REDBUTTON_X FRAME_X
#define REDBUTTON_Y FRAME_Y
#define REDBUTTON_W (FRAME_W/2)
#define REDBUTTON_H FRAME_H

// Green zone size
#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
#define GREENBUTTON_Y FRAME_Y
#define GREENBUTTON_W (FRAME_W/2)
#define GREENBUTTON_H FRAME_H


#define SENSOR1 4
#define SENSOR2 5
#define SENSOR3 18
#define SENSOR4 19
#define NIVELTANQUE 16

#define VALVULA1 25
#define VALVULA2 26
#define VALVULA3 17
#define VALVULA4 33
#define MOTOR 32

uint16_t contador = 0;
bool NIVEL1 = false;
bool NIVEL2 = false;
bool NIVEL3 = false;
bool NIVEL4 = false;


void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

void drawFrame()
{
  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, TFT_BLACK);
}

// Draw a red button
void redBtn()
{
  tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, TFT_RED);
  tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, TFT_DARKGREY);
  drawFrame();
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ON", GREENBUTTON_X + (GREENBUTTON_W / 2), GREENBUTTON_Y + (GREENBUTTON_H / 2));
  SwitchOn = false;
}

// Draw a green button
void greenBtn()
{
  tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, TFT_GREEN);
  tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, TFT_DARKGREY);
  drawFrame();
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("OFF", REDBUTTON_X + (REDBUTTON_W / 2) + 1, REDBUTTON_Y + (REDBUTTON_H / 2));
  SwitchOn = true;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void setup(void)
{
  Serial.begin(9600);
  tft.init();

  // Set the rotation before we calibrate
  tft.setRotation(1);

  // call screen calibration
  touch_calibrate();

  // clear screen
  tft.fillScreen(TFT_BLUE);

  // Draw button (this example does not use library Button class)
  redBtn();
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);
  pinMode(SENSOR3, INPUT);
  pinMode(SENSOR4, INPUT);
  pinMode(NIVELTANQUE, INPUT);
  
  pinMode(VALVULA1, OUTPUT);
  pinMode(VALVULA2, OUTPUT);
  pinMode(VALVULA3, OUTPUT);
  pinMode(VALVULA4, OUTPUT);
  pinMode(MOTOR, OUTPUT);

  digitalWrite(VALVULA1,HIGH);
  digitalWrite(VALVULA2,HIGH);
  digitalWrite(VALVULA3,HIGH);
  digitalWrite(VALVULA4,HIGH);
  digitalWrite(MOTOR,HIGH);
  
  tft.drawString("Nivel 1: Alto", 100, 120);
  
  tft.drawString("Nivel 2: Alto", 100, 150);
  tft.drawString("Nivel 3: Alto", 100, 180);
  tft.drawString("Nivel 4: Alto", 100, 210);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void niveles_agua(){
  if(digitalRead(SENSOR1) == NIVEL1){}
  else{
    if (digitalRead(SENSOR1) == true){
      NIVEL1 = true;
      tft.fillRect(0, 110, 300, 20, TFT_BLUE);
      tft.drawString("Nivel 1: Bajo", 100, 120);
    }
    else{
      NIVEL1 = false;
      tft.fillRect(0, 110, 300, 20, TFT_BLUE);
      tft.drawString("Nivel 1: Alto", 100, 120);
      }
    }

  if(digitalRead(SENSOR2) == NIVEL2){}
  else{
    if (digitalRead(SENSOR2) == true){
      NIVEL2 = true;
      tft.fillRect(0, 140, 300, 20, TFT_BLUE);
      tft.drawString("Nivel 2: Bajo", 100, 150);
    }
    else{
      NIVEL2 = false;
      tft.fillRect(0, 140, 300, 20, TFT_BLUE);
      tft.drawString("Nivel 2: Alto", 100, 150);
      }
    }
  if(digitalRead(SENSOR3) == NIVEL3){}
  else{
    if (digitalRead(SENSOR3) == true){
      NIVEL3 = true;
      tft.fillRect(0, 170, 300, 20, TFT_BLUE);
      tft.drawString("Nivel 3: Bajo", 100, 180);
    }
    else{
      NIVEL3 = false;
      tft.fillRect(0, 170, 300, 20, TFT_BLUE);
      tft.drawString("Nivel 3: Alto", 100, 180);
      }
    }

  if(digitalRead(SENSOR4) == NIVEL4){}
  else{
    if (digitalRead(SENSOR4) == true){
      NIVEL4 = true;
      tft.fillRect(0, 200, 300, 20, TFT_BLUE);
      tft.drawString("Nivel 4: Bajo", 100, 210);
    }
    else{
      NIVEL4 = false;
      tft.fillRect(0, 200, 300, 20, TFT_BLUE);
      tft.drawString("Nivel 4: Alto", 100, 210);
      }
    }
  }


void valvulas(){
  if (SwitchOn == false){
    digitalWrite(VALVULA1,HIGH);
    digitalWrite(VALVULA2,HIGH);
    digitalWrite(VALVULA3,HIGH);
    digitalWrite(VALVULA4,HIGH);
    digitalWrite(MOTOR,HIGH);
    }
  else{
      if (digitalRead(SENSOR1) == true){
        digitalWrite(VALVULA1,LOW);
        delay(50);
        digitalWrite(VALVULA2,HIGH);
        digitalWrite(VALVULA3,HIGH);
        digitalWrite(VALVULA4,HIGH);
        if (digitalRead(NIVELTANQUE) == true){
          digitalWrite(MOTOR,HIGH);}
        else{digitalWrite(MOTOR,LOW);}
        Serial.println("Valvula 1 Activada");
        }
      else{
        digitalWrite(VALVULA1,HIGH);
        if (digitalRead(SENSOR2) == true){
          digitalWrite(VALVULA2,LOW);
          delay(50);
          digitalWrite(VALVULA1,HIGH);
          digitalWrite(VALVULA3,HIGH);
          digitalWrite(VALVULA4,HIGH);
          if (digitalRead(NIVELTANQUE) == true){
            digitalWrite(MOTOR,HIGH);}
          else{digitalWrite(MOTOR,LOW);}
          Serial.println("Valvula 2 Activada"); 
        }
        else{
          digitalWrite(VALVULA2,HIGH);
          if (digitalRead(SENSOR3) == true){
            digitalWrite(VALVULA3,LOW);
            delay(50);
            digitalWrite(VALVULA1,HIGH);
            digitalWrite(VALVULA2,HIGH);
            digitalWrite(VALVULA4,HIGH);
            if (digitalRead(NIVELTANQUE) == true){
              digitalWrite(MOTOR,HIGH);}
            else{digitalWrite(MOTOR,LOW);}
            Serial.println("Valvula 3 Activada"); 
          }
          else{
            digitalWrite(VALVULA3,HIGH);
            if (digitalRead(SENSOR4) == true){
              digitalWrite(VALVULA4,LOW);
              delay(50);
              digitalWrite(VALVULA2,HIGH);
              digitalWrite(VALVULA3,HIGH);
              digitalWrite(VALVULA1,HIGH);
              if (digitalRead(NIVELTANQUE) == true){
                digitalWrite(MOTOR,HIGH);}
              else{digitalWrite(MOTOR,LOW);}
              Serial.println("Valvula 4 Activada"); 
            }
            else{
              digitalWrite(VALVULA4,HIGH);
              digitalWrite(VALVULA2,HIGH);
              digitalWrite(VALVULA3,HIGH);
              digitalWrite(VALVULA1,HIGH);
              digitalWrite(MOTOR,HIGH);
             }
             }
         }
         }
    }
    
  
  }

void loop(){
  uint16_t x, y;
  contador = contador +1;
  if (contador == 100){
    contador = 0;
    valvulas();
    Serial.println(digitalRead(NIVELTANQUE));}
  
    
    
  niveles_agua();
  // See if there's any touch data for us
  if (tft.getTouch(&x, &y))
  {
    // Draw a block spot to show where touch was calculated to be
    #ifdef BLACK_SPOT
      tft.fillCircle(x, y, 2, TFT_BLACK);
    #endif
    
    if (SwitchOn)
    {
      if ((x > REDBUTTON_X) && (x < (REDBUTTON_X + REDBUTTON_W))) {
        if ((y > REDBUTTON_Y) && (y <= (REDBUTTON_Y + REDBUTTON_H))) {
          Serial.println("Red btn hit");
          redBtn();
        }
      }
    }
    else //Record is off (SwitchOn == false)
    {
      if ((x > GREENBUTTON_X) && (x < (GREENBUTTON_X + GREENBUTTON_W))) {
        if ((y > GREENBUTTON_Y) && (y <= (GREENBUTTON_Y + GREENBUTTON_H))) {
          Serial.println("Green btn hit");
          greenBtn();
        }
      }
    }

    Serial.println(SwitchOn);

  }
}
//------------------------------------------------------------------------------------------
