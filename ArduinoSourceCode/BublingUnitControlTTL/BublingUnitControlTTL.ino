//Code for controling 

#include <string.h>
#include <stdlib.h>
#include "Arduino_LED_Matrix.h"
#include <EEPROM.h>

#define BAUDRATE 9600          // Baudrate, on some systems the value 9600 may be used
#define MAX_MESSAGE_LENGTH 64  // Max allowed length of input
#define ARG_NUMBER 10          // Max allowed number of args, higher baudrates allow more args
#define RESETRATE 10000        //  Set port reset rate in ms. (i.e. each approx. RESETRATE ms, the port will be closed and reopened)

static char gcaRxStr[MAX_MESSAGE_LENGTH + 1];  // Received message string
static char* gcaAux;                           // Auxiliary String for strtok() string splitting
static uint8_t u8aArgs[ARG_NUMBER];            // Arguments as Array
static uint8_t u8pwm = 0;                      // PWM duty cycle variable
static uint8_t u8RxStrPos = 0;                 // Position at Received string, equals to number of rec. bytes
static uint8_t u8ArgPos = 0;                   // Number of Arguments after strtok() splitting
static uint32_t u32reset = 0;                  // Resetcounter for port closing and reopening
const int TTL1 = 8;                            // TTL connection no.1.
const int TTL2 = 7;                            // TTL connection no.2.
const int TTL3 = 6;                              // TTL connection no.3.

//Definintions of animations on Arduino UNO rev.4 LED panel.
byte f1[8][12] = {

  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }
};


byte f1a[8][12] = {

  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }
};

byte f1b[8][12] = {

  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }
};


byte f1c[8][12] = {

  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }
};

byte f1d[8][12] = {

  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }
};
byte f1e[8][12] = {

  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }
};


byte f1f[8][12] = {

  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }
};
byte f1g[8][12] = {

  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1 },
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }
};

byte f1h[8][12] = {

  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1 },
  { 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }
};

byte f2[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0 }
};

byte f21[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1 }
};
byte f22[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1 }
};
byte f23[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1 }
};
byte f24[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1 }
};
byte f25[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1 }
};
byte f26[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1 },
  { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1 }
};
byte f27[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1 },
  { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1 }
};
byte f28[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1 },
  { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1 }
};
byte f3[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0 }
};

byte end[8][12] = {
  { 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0 },
  { 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 },
  { 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0 },
  { 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0 },
  { 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0 },
  { 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0 },
  { 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 },
  { 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0 }
};
// R4 Specific code
ArduinoLEDMatrix matrix;
uint32_t status[3];
int fmode = 0;
void setup() {

  pinMode(TTL1, OUTPUT);
  pinMode(TTL2, OUTPUT);

  pinMode(TTL3, OUTPUT);
  Serial.begin(BAUDRATE);
  // Serial.begin(115200);

  matrix.begin();
  while (!Serial) continue;  // This evaluates to always true on Uno R3
   delay(10);
   if (EEPROM.read(0x0000) != 0xFF) {
    uint16_t u16address = 0x0000;
    while (EEPROM.read(u16address) != 0xFF) {
      Serial.print(char(EEPROM.read(u16address)));
      u16address++;
    }
    Serial.println();
  }
  Serial.print("Starting...");
  Serial.println();
}

void loop() {
  // READ UART INPUT
  char cInByte;

  if (Serial.available() > 0) {
    delay(1);
    u8RxStrPos = 0;
    u8ArgPos = 0;
    memset(gcaRxStr, 0, MAX_MESSAGE_LENGTH + 1);  // Reset the character argument string
    while (Serial.available() > 0) {
      cInByte = Serial.read();
      if (cInByte != '\n') {
        gcaRxStr[u8RxStrPos] = cInByte;
        u8RxStrPos++;
      }
      if (cInByte == '\n') {
        gcaRxStr[u8RxStrPos] = '\0';
        u8RxStrPos = 0;
      }
    }  // End of message receive code
 
    gcaAux = strtok(gcaRxStr, " ");  // First argument
    u8aArgs[u8ArgPos] = atoi(gcaAux);
    u8ArgPos++;
    while (gcaAux != NULL)  
    {
     
      gcaAux = strtok(NULL, " ");
      u8aArgs[u8ArgPos] = atoi(gcaAux);
      u8ArgPos++;
    }
    Serial.print("Received: ");
    for (int i = 0; i < u8ArgPos - 1; i++) {
      Serial.print(i);
      Serial.print(":");
      Serial.print(u8aArgs[i]);
      Serial.print(", ");
    }   
    Serial.println("");
    delay(3);


    /// COMMANDS
    switch (u8aArgs[0])   
    {
      case (1):
        {
          //function1
          matrix.renderBitmap(f1, 8, 12);
          Serial.println("Function1 started");

          digitalWrite(TTL1, HIGH);
          digitalWrite(TTL2, LOW);
          digitalWrite(TTL3, LOW);

          delay(10); 
          matrix.renderBitmap(f1a, 8, 12);

          digitalWrite(TTL2, HIGH);
          delay(500);  
          matrix.renderBitmap(f1b, 8, 12);

          digitalWrite(TTL3, HIGH);
          delay(400);  
          matrix.renderBitmap(f1c, 8, 12);
          digitalWrite(TTL2, LOW);

          delay(2000); 
          matrix.renderBitmap(f1d, 8, 12);

          delay(2000);  
          matrix.renderBitmap(f1e, 8, 12);
          delay(2000);  
          matrix.renderBitmap(f1f, 8, 12);

          delay(2000); 
          matrix.renderBitmap(f1g, 8, 12);

          delay(1500);  
          matrix.renderBitmap(f1h, 8, 12);
          delay(500);
          Serial.println("Function1 ended");
          matrix.renderBitmap(end, 8, 12);

          break;
        }
      case (2):
        {
          //Bubling time 2s.
          matrix.renderBitmap(f2, 8, 12);

          Serial.println("Function2 started");
          digitalWrite(TTL1, LOW);   //
          digitalWrite(TTL2, HIGH);  //

          delay(300);   
          matrix.renderBitmap(f21, 8, 12);

          delay(150);   
          matrix.renderBitmap(f22, 8, 12);

          delay(150);  
          matrix.renderBitmap(f23, 8, 12);
     
          delay(150);  
          matrix.renderBitmap(f24, 8, 12);

          delay(150);  
          matrix.renderBitmap(f25, 8, 12);

          delay(150);  
          matrix.renderBitmap(f26, 8, 12);

          delay(150); 
          delay(500); 
          matrix.renderBitmap(f27, 8, 12);

          delay(100);  
          matrix.renderBitmap(f28, 8, 12);

          delay(200);               
          digitalWrite(TTL1, HIGH);  //
          digitalWrite(TTL2, LOW);   //
          Serial.println("Function2 ended");
          matrix.renderBitmap(end, 8, 12);

          break;
        }
      case (3):
        {
          matrix.renderBitmap(f3, 8, 12);
          Serial.println("Function3 started");
          digitalWrite(TTL3, LOW); 
          digitalWrite(TTL1, LOW); 
          Serial.println("Function3 ended");
          delay(100);
          matrix.renderBitmap(end, 8, 12);

          break;
        }
    }

 
   }
}
