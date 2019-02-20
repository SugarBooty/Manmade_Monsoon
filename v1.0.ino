#include <EEPROM.h>

int RST = 4;  //4017 reset pin 15
int CLK = 5;  //4017 clock pin 14
int EN = 6;   //4017 enable/clock inhibit pin 13
int PUMP = 7; //Pump control output. Active high.
int UP = 9;   //Up bttn input pullup
int DWN = 8;  //Down bttn input pullup

long WaitDuration = (120 * 60000); //How long apart (in minutes) to water
int WaterMultiplier = 1; //Multiplier for watering times. Adjust for amount of water needed
bool PumpActive = 1; // Is the pump active? 
int PumpDuration = (60 * 1000);//in seconds per bar, 60 seconds is aproximately 100ml. 500ml/day is the aproximate use for a plant

unsigned long PreviousMillis = 0;
unsigned long PrevMillisDisp = 0;
int pin; // For sending single bytes

int DebounceDelay = 300;

int DISP = EEPROM.read(0); // read from EEPROM to get saved state
int preval = DISP; //previous bar graph value
int val = DISP;



volatile byte state = LOW;

void setup() {

  pinMode(13, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DWN, INPUT_PULLUP);

  digitalWrite(RST, LOW);
  digitalWrite(CLK, LOW);
  digitalWrite(EN, LOW);

//attachInterrupt(digitalPinToInterrupt(UP), TimeUp, LOW);
//attachInterrupt(digitalPinToInterrupt(DWN), TimeDown, LOW);
  
  Serial.begin(9600); 

sendByte(RST);
        int effect = 30;
        for (int x = 1 ; x <= DISP ; x++) {
          delay(effect);
          effect = effect - 3;
          sendByte(CLK);
        }
  
  int state = HIGH;
}

void loop() {

  
  unsigned long CurrentMillis = millis();

  

  if (digitalRead(UP) == LOW) {
    TimeUp();
  }

  if (digitalRead(DWN) == LOW) {
    TimeDown();
  }

  if (PumpActive == 1) {
    if ((bitRead(PORTD,PUMP)) == LOW) {
    digitalWrite(PUMP, HIGH); 
    Serial.print("Pump on \n");
  }
    
    if (CurrentMillis - PreviousMillis >= PumpDuration * DISP) {
      PreviousMillis = CurrentMillis;
      PumpActive = 0;
      digitalWrite(PUMP, LOW);
      Serial.print("Pump Off -- wait \n");
    }
    
  } else if (PumpActive == 0) {
      if ((bitRead(PORTD,PUMP)) == HIGH) {
        digitalWrite(PUMP, LOW);
        Serial.print("Pump Off -- wait \n");
      }
    
      if (CurrentMillis - PreviousMillis >= WaitDuration) {
        PreviousMillis = CurrentMillis;
        PumpActive = 1;
        digitalWrite(PUMP, HIGH); 
        Serial.print("Pump On \n");
      }
    }
}

//Button Interupt
void TimeUp() {
  unsigned long CurrentMillisDisp = millis();
  if (CurrentMillisDisp - PrevMillisDisp >= DebounceDelay) {
    PrevMillisDisp = CurrentMillisDisp;

    if ((DISP + 1 >=0) & (DISP + 1 <= 9)) {
      DISP++;
      sendByte(RST);
        int effect = 30;
        for (int x = 1 ; x <= DISP ; x++) {
          delay(effect);
          effect = effect - 3;
          sendByte(CLK);
        }
    }
    
    EEPROM.write(0, DISP);
    Serial.print("DISP: ");
    Serial.println(DISP);
    state = !state;
    digitalWrite(13, state);
  }
  
}

//Button Interupt
void TimeDown() {
  unsigned long CurrentMillisDisp = millis();
  if (CurrentMillisDisp - PrevMillisDisp >= DebounceDelay) {
    PrevMillisDisp = CurrentMillisDisp;
    state = !state;
    digitalWrite(13, state);
    int effect = 30;
    if ((DISP - 1 >=0) & (DISP - 1 <= 9)) {
      DISP = DISP - 1;
      sendByte(RST);
        for (int x = 1 ; x <= DISP ; x++) {
          delay(effect);
          effect = effect - 3;
          sendByte(CLK);
        }
    }
    EEPROM.write(0, DISP);
    Serial.print("DISP: ");
    Serial.println(DISP);
  }
}

void sendByte(int pin) {
  digitalWrite(pin, HIGH);
  digitalWrite(pin, LOW);
  Serial.print(pin);
  Serial.print("bit \n");
}
