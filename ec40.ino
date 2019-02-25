//onebutton libary
#include <OneButton.h>

//current sensor data
int mVperAmp = 100;
int RawValue = 0;
int ACSoffset = 2500;
double Voltage = 0;
double Amps = 0;

//arduino pin assignment
const int analogIn = A0;
const int button_pin = 7;
const int relay_pin = 8;

OneButton button(button_pin, false); //false, because pull down resistor is used

int amp = 0;
bool motor = LOW;
bool button_cond = LOW;

int buttonLevel = digitalRead(button_pin);

//here you can adjust the values for the motor shutdown
const double amp_idle = 0.5;
const unsigned long idletime = 2000;
const int  amp_stall = 6;
const unsigned long stalltime = 200;

unsigned long lastcalltime = 0;
unsigned long elapsedtime = 0;
unsigned long stalltimer = 0;
unsigned long idletimer = 0;

void setup() {
  Serial.begin(9600);
  pinMode(relay_pin, OUTPUT);
  pinMode(button_pin, INPUT);
  digitalWrite(relay_pin, LOW);
  button.attachClick(buttonclick);
}

void clearAndHome() // https://gist.github.com/THEtheChad/5539958
{
  Serial.write(27);
  Serial.print("[2J"); // clear screen
  Serial.write(27); // ESC
  Serial.print("[H"); // cursor to home
}

//reads and converts analog signals to motor current
void ampread()
{
  RawValue = analogRead(analogIn);
  Voltage = (RawValue / 1024.0) * 5000; // gets you mV
  Amps = ((Voltage - ACSoffset) / mVperAmp);
  Amps = abs(Amps); //prevents negativ values
}

//turns motor on when push button switch is pressed
void buttonclick() {
  digitalWrite(relay_pin, HIGH);
  motor = HIGH;
}

void loop()
{
  ampread();

  button.tick();

  if (Amps > amp_stall && motor == HIGH) //counts up time if the motor stalls
  {
    elapsedtime = millis() - lastcalltime;
    stalltimer += elapsedtime;
  }
  else
  {
    stalltimer = 0;
  }

  if (Amps < amp_idle && motor == HIGH) //counts up if there is no load (no beans)
  {
    elapsedtime = millis() - lastcalltime;
    idletimer += elapsedtime;
  }
  else
  {
    idletimer = 0;
  }

  lastcalltime = millis();

  //shuts off motor if idle or stall time is reached
  if (stalltimer >= stalltime || idletimer >= idletime)
  {
    digitalWrite(relay_pin, LOW);
    motor = LOW;
  }

  // clearAndHome(); //use this for "debugging" with Putty, in terminal new values will "replace" old values
  // Serial.println(Amps);
  // Serial.println(stalltimer);
  // Serial.println(idletimer);


}
