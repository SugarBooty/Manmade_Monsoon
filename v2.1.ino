#include <EEPROM.h>

/*setup:
 *  the bars value is checked to make sure its valid (less than 9)
 *  set the pinmodes for all the I/O
 *  display the current bars value on the display for 300ms
 *  
 *loop:
 *  sets currentmillis to the current millis
 *  Checks if the EEPROM is waiting writing, try to write if it is
 *  check for button presses (debounced in function)
 *  checks to see if the pump should be on.If yes -->
 *    calls the pregress bar function (it loops through and calls this whenever the pump is on)
 *    checks if the pin for the pump control is off, turns it on if it is off
 *    checks if its time to move on. If yes -->
 *      turn off the pump
 *      enable user input
 *      resets the iteration value to 1 (for next time)
 *      show the bars value on the display
 *  checks to see if it should wait. If yes -->
 *    checks if the pin for the pump control is on, turns it off if it is on
 *    checks to see if its time to move on. If yes -->
 *      turn on the pump
 *      disable user input
 *      resets the display to 1
 *      
 *progress bar
 *  generates the amount of time to stay on each step so 10 steps can be taken while the pump is on
 *  calculates the time the pump has been on
 *    When it reaches the first step, add another step to that time to wait
 *    increment iteration by one to show that the display went up by one
*/

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
long bars = EEPROM.read(0); //reads the EEPROM to recall saved state
long lastBars = bars; // used to store the last known EEPROM value
bool pumpActive = 1;  // controls the pump
bool userInput = 0;   // used to enable/disable user input
int iteration = 1;    // used for the progress bar. Each iteration is making it increase by one bar
unsigned long steps = 0; //how long each bar is displayed before going to the next one on the progress bar

//config
const unsigned long waitDuration = (120 * 60000); // How long to wait. format: (minutes * 60000)
const unsigned long pumpDuration = (10 * 1000);   //How long the pump is on per bar. format: (seconds * 1000)

void setup() {
  if (bars > 9) { // if the bars value read from EEPROM is above 9, set it to 5. This happens on new arduinos where this program hasnt been run
    bars = 5;
    lastBars = 5;
    EEPROM.write(0, bars);
  }

  pinMode(RST, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DWN, INPUT_PULLUP);


// sets all the 4017 I/O low
  digitalWrite(RST, LOW);
  digitalWrite(CLK, LOW);
  digitalWrite(EN, LOW);

  displayWrite(bars);
  delay(300);
}

void loop() {
  currentMillis = millis();

//checks to see if the EEPROM is waiting to be written to, if the last saves value is not equal to the currentvalue stored in memory it is written after the delay
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

//turns on pump when pumpActive is true
  if (pumpActive) {
    progressBar; // this is called to loop the progress bar function while the pump is active
    if (bitRead(PORTD, PUMP) == LOW) {  //checks the pump pin status directly to see if its off
      digitalWrite(PUMP, HIGH);         //turns it on, because the pump is active
      digitalWrite(LED, HIGH);          //indicator for testing
    }
//turns off the pump when enough time has passed
    if ((currentMillis - previousMillis) >= (pumpDuration * bars)) {  // if (time elapsed >= time to wait)
      previousMillis = currentMillis;
      pumpActive = 0; // set the pup variable to 0, this disables the pump
      userInput = 1;  // enables user input
      iteration = 1;  // used for the progress bar, each iteration is basically incrementing it by one
      displayWrite(bars); //puts the bars val on the display
    }
  }

//turns off pump when pumpActive is 0
  if (!(pumpActive)) {
    if (bitRead(PORTD, PUMP) == HIGH) { // checks the pump pin to see if its on
      digitalWrite(PUMP, LOW);
      digitalWrite(LED, LOW);
    }
//turns on the pump after the waitDuration is elapsed
    if (currentMillis - previousMillis >= waitDuration) {
      previousMillis = currentMillis;
      pumpActive = 1; // set the pup variable to 1, this enables the pump
      userInput = 0;  // disables user input
      bloop(RST);     // sends a byte to the 4017 resetting it to 1
    }
  }
}

// triggered when up button is pressed
void upButton() {
  if (bars < 9) {  // only increment if it will be less than 9
    bars = bars + 1;
    displayWrite(bars); //displays the new bars var when the button is pressed
    updateEEPROM(); //self explanatory
  }
}

//triggered when down button is pressed
void downButton() {
  if (bars > 0) {  // only decrement if it will be at least 0
    bars = bars - 1;
    displayWrite(bars); //displays the new bars var when the button is pressed
    updateEEPROM(); //self explanatory
  }
}

//saves new bar values to the EEPROM after a delay (so it doesnt glitch and burn it out)
void updateEEPROM() {
  if (currentMillis - previousMillisEEPROM >= writeEEPROMDelay) {  // waits for the eeprom delay until writing a new value
    previousMillisEEPROM = currentMillis;
    EEPROM.write(0, bars);
    lastBars = bars; // writes the last stored bars value to memory
  }
}

//runs to update the progress bar for watering
void progressBar() {
  steps = ((bars * pumpDuration) / 10); // takes the total time (bars * pumpDuration) and divides it by 10 to get the time between incrementing the bar graph
  if ((currentMillis - previousMillis) >= (steps * iteration)) {
    iteration++; // multiplied by the time per step, this makes the duration it checks for longer by one bar. I cannot change the currentMillis or previousMillis because they are used for the pump and wait timing
    displayWrite((iteration - 1)); // displays the current iteration - 1 because iteration needs to start at 1 so the math two lines above works correctly. If it were 0 the time would be 0.
  }
}

//displays to the bar graph
void displayWrite(int amount) {
  if (amount <= 9) { // checks to see if the the number its trying to write is valid
    bloop(RST);      // sends a byte to the 4017 reset pin
    int effect = 10; // used for the display effects, this is the initial delay between segments counting up as fast as possible
    for (int x = 1; x <= amount; x++) {
      delay(effect); // ""
      bloop(CLK);    // bloops the clock of the 4017 causing it to count up
      effect = effect + 5; // makes the time between segments counting up last longer the higher the number on the display is. It makes it look like its slowing down before it stops.
    }
  }
}

//bloops an  output. Bloop is a technical term.
void bloop(int pin) {
  digitalWrite(pin, HIGH);
  digitalWrite(pin, LOW);
}
