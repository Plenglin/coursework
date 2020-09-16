#include <Bounce2.h> // Debounce switch to mitigate spurious readings due to mechanical vibrations


#define LED_OPEN 9
#define LED_SHORT 7

const float Vin = 5.0;  //should start with Vin = 5.0
const float R1 = 10000; //should start with R1 = 10K
const int CIRCUIT = 0; //using analog input A0
const int BUTTON = 12; //Button to ground on pin 12
const int READ_DELAY_mS = 1; //delay between successive reads
const float VOLTS_PER_COUNT = Vin/1023.0; //4.89 mV / ADC count, assuming 5V reference


Bounce Button = Bounce(); //Button object from Bounce library
float Vout; //voltage out of voltage divier, read by ADC
char VoutString[100]; //string representation of Vout
float R2; //"unknown" resistance
int ADCvalue; // value read from ADC

void setup() {
  // serial monitor used at 115200 bps
  Serial.begin(115200);
  // trigger button
  pinMode(BUTTON, INPUT_PULLUP);
  // using the Bounce2 library to debounce the button
  Button.attach(BUTTON,INPUT_PULLUP);
  Button.interval(25);
  // starting salutation
  Serial.println("Arduino Ohm Meter Started.\nPush button to initiate reading.\n");
}

void loop() {
  // Is the button pushed?
  Button.update();
  bool ButtonPushed = !Button.read(); // invert logic to true = pushed
  
  // if the button is pushed, generate a reading
  if (ButtonPushed) {
    //read the ADC four times to get a stable reading
    for (int i=0; i < 4; i++) {
      ADCvalue = analogRead(CIRCUIT);
      delay(READ_DELAY_mS);
    }
    //calculate the voltage that was read
    Vout = (float)ADCvalue * VOLTS_PER_COUNT;
    // calculate the resistance
    R2 = 1.00286 * (Vout*R1)/(Vin-Vout);
    // print the results to the serial monitor
    Serial.print("ADC Value: ");
    Serial.print(ADCvalue);
    Serial.print("   Vout: ");
    dtostrf(Vout, 1,4, VoutString);
    Serial.print(VoutString);
    Serial.print("V");
    Serial.print(" Measured Resistance: ");
    Serial.print(R2);
    Serial.println(" ohms");
    // wait for the button to be released before proceeding
  }

  int continuityMeasurement = analogRead(CIRCUIT);
  bool openState = 0;
  bool shortState = 0;
  if (continuityMeasurement == 1023) {
    openState = 1;
  } else if (continuityMeasurement == 0) {
    shortState = 1;
  }
  digitalWrite(LED_OPEN, openState);
  digitalWrite(LED_SHORT, shortState);
  
  while (ButtonPushed) {
    Button.update();
    ButtonPushed = !Button.read();
  }
}
