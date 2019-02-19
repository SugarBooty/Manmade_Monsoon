#include <EEPROM.h>

int RST = 4;  //4017 reset pin 15
int CLK = 5;  //4017 clock pin 14
int EN = 6;   //4017 enable/clock inhibit pin 13
int PUMP = 7; //Pump control output. Active high.
int UP = 2;   //Up bttn input pullup
int DWN = 3;  //Down bttn input pullup

int WaitDuration = 120; //How long apart (in minutes) to water
int WaterMultiplier = 1; //Multiplier for watering times. Adjust for amount of water needed
bool PumpActive = 1; // Is the pump active? 
int PumpDuration = 60;//in seconds, 60 seconds is aproximately 100ml. 500ml/day is the aproximate use for a plant

unsigned long PreviousMillis = 0;
int pin; // For sending single bytes

int DISP = EEPROM.read(0); // read from EEPROM to get saved state
int preval; //previous bar graph value

volatile byte state = LOW;

void setup() {
  pinMode(RST, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DWN, INPUT_PULLUP);

  digitalWrite(RST, LOW);
  digitalWrite(CLK, LOW);
  digitalWrite(EN, LOW);

attachInterrupt(digitalPinToInterrupt(UP), TimeUp, LOW);
attachInterrupt(digitalPinToInterrupt(DWN), TimeDown, LOW);
  
  Serial.begin(1200); 
}

void loop() {
  unsigned long CurrentMillis = millis();

  if (PumpActive == 1) {
    if ((bitRead(PORTD,PUMP)) == LOW) {
    digitalWrite(PUMP, HIGH); 
  }
    
    if (CurrentMillis - PreviousMillis >= PumpDuration) {
      PreviousMillis = CurrentMillis;
      PumpActive = 0;
      digitalWrite(PUMP, LOW);
    }
    
  } else if (PumpActive == 0) {
      if ((bitRead(PORTD,PUMP)) == HIGH) {
        digitalWrite(PUMP, LOW); 
      }
    
      if (CurrentMillis - PreviousMillis >= WaitDuration) {
        PreviousMillis = CurrentMillis;
        PumpActive = 1;
        digitalWrite(PUMP, HIGH); 
      }
    }
}

//Button Interupt
void TimeUp() {
  delay(1);
  DISP++;
  EEPROM.write(0, DISP);
  Bar(DISP);
}

//Button Interupt
void TimeDown() {
  delay(1);
  DISP--;
  EEPROM.write(0, DISP);
  Bar(DISP);
}

//Controls the bar graph
void Bar(int val) {
  //if the value is greater than the previous value send a byte through the clock
  if (val > preval) {
    sendByte(CLK);
  //if the value is less than the previous value, reset and resend the values
  }else if(val < preval) {
    sendByte(RST);
    for(int x = val ; x!=val ; x++) {
      sendByte(CLK);
    }
  }
  preval = val;
}

void sendByte(pin) {
  digitalWrite(pin, HIGH);
  digitalWrite(pin, LOW);
}
