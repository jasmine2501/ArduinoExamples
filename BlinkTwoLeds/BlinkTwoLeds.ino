

// constants to set pin numbers
const int greenLedPin =  11; // the number of the green LED pin
const int redLedPin = 12;    // the number of the green LED pin

// Variables will change
int greenLedState = HIGH; //ledState for green LED
int redLedState = HIGH;   //ledState for red LED
long previousMillisRed = 0;    //last time Red LED was updated
long previousMillisGreen = 0;  //last time Green LED was updated

// must be long to prevent overflow
long greenLedInterval = 900; //interval to blink greed LED (milliseconds)
long redLedInterval = 200;    //interval to blink greed LED (milliseconds)
unsigned long currentMillis = 0;

void setup() {
  // set the pins to output mode
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  currentMillis = millis();
}

void loop()
{
  // capture the current time
  currentMillis = millis();
  manageRedLed();
  manageGreenLed();
}

void manageRedLed() {  //check if it's time to change the Red LED yet
  if(currentMillis - previousMillisRed > redLedInterval) {
    //store the time of this change
    previousMillisRed = currentMillis;
    redLedState = (redLedState == HIGH) ? LOW : HIGH;
    digitalWrite(redLedPin, redLedState);
  }
}

void manageGreenLed() {
  //check if it's time to change the green LED yet 
  if(currentMillis - previousMillisGreen > greenLedInterval) {
    //store the time of this change
    previousMillisGreen = currentMillis;
    greenLedState = (greenLedState == HIGH) ? LOW : HIGH;
    digitalWrite(greenLedPin, greenLedState);
  }
}

