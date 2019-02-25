#include <EEPROM.h>

//pins
int RST = 4;  //4017 reset pin 15
int CLK = 5;  //4017 clock pin 14
int EN = 6;   //4017 enable/clock inhibit pin 13
int PUMP = 7; //Pump control output. Active high.
int UP = 9;   //Up bttn input pullup
int DWN = 8;  //Down bttn input pullup
int LED = 13; //built in LED

//time vars
unsigned long previousMillis = 0;
unsigned long previousMillisDebounce = 0;
unsigned long previousMillisEEPROM = 0;
unsigned long currentMillis = 0;
int debounceDelay = 300;
long writeEEPROMDelay = 1000;


//misc
int bars = EEPROM.read(0); //reads the EEPROM to recall saved state
int lastBars = bars;
bool pumpActive = 1;
bool userInput = 1;
int iteration = 1;


//config
long waitDuration = (120 * 60000); // How long in minutes to wait
int pumpDuration = (10000); //in seconds, the length of time per bar the pump runs

unsigned long steps = 0;



void setup() {
  if (bars > 9) {
    bars = 5;
    lastBars = bars;
    EEPROM.write(0, bars);
  }

  pinMode(RST, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DWN, INPUT_PULLUP);

  digitalWrite(RST, LOW);
  digitalWrite(CLK, LOW);
  digitalWrite(EN, LOW);

  displayWrite(bars);
  delay(300);

}

void loop() {
  currentMillis = millis();

//checks to see if the EEPROM is waiting to be written to
  if (lastBars != bars) {
    updateEEPROM();
  }

//reads button states with debounce
  if ((currentMillis - previousMillisDebounce >= debounceDelay) & userInput) {
    previousMillisDebounce = currentMillis;
    
    if (digitalRead(UP) == LOW) {
      upButton();
    } 
    if (digitalRead(DWN) == LOW) {
      downButton();
    }
  }

//turns on pump
  if (pumpActive) {
    progressBar();
    if (bitRead(PORTD, PUMP) == LOW) {  //checks the pump pin to see if its off
      digitalWrite(PUMP, HIGH);
      digitalWrite(LED, HIGH);
      
    }
    if ((currentMillis - previousMillis) >= (pumpDuration * bars)) {
      previousMillis = currentMillis;
      pumpActive = 0;
      userInput = 1;  // enables user input
      
      displayWrite(bars);
    }
  }

//turns off pump
  if (!(pumpActive)) {
    if (bitRead(PORTD, PUMP) == HIGH) { // checks the pump pin to see if its on
      digitalWrite(PUMP, LOW);
      digitalWrite(LED, LOW);
    }
    if ((currentMillis - previousMillis) >= waitDuration) {
      previousMillis = currentMillis;
      pumpActive = 1;
      userInput = 0;  // disables user input
      iteration = 1;
      progressBar();
    }
  }
}

// triggered when up button is pressed
void upButton() {
  if (bars < 9) {
    bars = bars + 1;
    displayWrite(bars);
    updateEEPROM();
  }
}

//triggered when down button is pressed
void downButton() {
  if (bars > 0) {
    bars = bars - 1;
    displayWrite(bars);
    updateEEPROM();
  }
}

//saves new bar values to the EEPROM after a delay (so it doesnt glitch and burn it out)
void updateEEPROM() {
  if (currentMillis - previousMillisEEPROM >= writeEEPROMDelay) {
    previousMillisEEPROM = currentMillis;
    EEPROM.write(0, bars);
    lastBars = bars;
  }
}

//runs to update the progress bar for watering
void progressBar() {
  steps = ((bars * pumpDuration) / 10);
  if ((currentMillis - previousMillis) >= (steps * iteration)) {
    iteration++;
    displayWrite((iteration - 1));
  }
}

//displays to the bar graph
void displayWrite(int amount) {
  if (amount <= 9) {
    bloop(RST);
    int effect = 10;
    for (int x = 1; x <= amount; x++) {
      delay(effect);
      bloop(CLK);
      effect = effect + 5;
    }
  }
}

//bloops an  output
void bloop(int pin) {
  digitalWrite(pin, HIGH);
  digitalWrite(pin, LOW);
}

