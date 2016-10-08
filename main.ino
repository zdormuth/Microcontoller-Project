/*
EE 2361 Final Project
Cat sets motion detector off
Calls function with variables of light sensing and time
If conditions met, door will unlock and cat will go out
If cat goes out, push button signifies cat leaving, door will lock
*/

// initializing use of individually addressable LED
#include "neopixel/neopixel.h"
#define PIXEL_PIN D3
#define PIXEL_COUNT 1
#define PIXEL_TYPE WS2811

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// initialize analog inputs
int tempPin = A0;
int lightPin = A1;

//digital pin connected to PIR sensor output
int pirPin = D2;

// initialize LED lights
int greenLedPin = D0; // output pin on 
int redLedPin = D1;
int ledPhoton = D7;
int ledLocked = D3;

// initialize Push Button
int ButtonPIN = D4;
bool ButtonOut = false;

int data0; // temp sensor
int data1; // light sensor

// function to check light and temp requirements
bool checker();

// function to unlock door
void unlockDoor();

// **MOTION DETECTED**
int PixelColorPurple = strip.Color(128, 0, 128); // initialize color to purple
// **INTERMEDIATE STAGE - DOOR IS LOCKED BUT MAY OPEN SOON**
int PixelColorYellow = strip.Color(60, 50, 5); // initialize color to yellow
// **INTERMIDATE STAGE - DOOR IS UNLOCKED BUT MAY CLOSE SOON**
int PixelColorBlue = strip.Color(6, 22, 249); // initialize color to blue
// **LOCKED DOOR**
int PixelColorRed = strip.Color(80, 0, 4); // initialize color to red 
// **UNLOCKED DOOR**
int PixelColorGreen = strip.Color(6, 250, 6); // initialize color to green

void setup() {
    Serial.begin(9600);
    
    // initialize pins
    pinMode(pirPin, INPUT);
    pinMode(greenLedPin, OUTPUT); // output pin on photon
    pinMode(redLedPin, OUTPUT);
    pinMode(ledPhoton, OUTPUT);
    pinMode(ButtonPIN, INPUT_PULLDOWN);
    pinMode(ledLocked, OUTPUT);
    
    // colorful LED
    strip.begin();
    
    digitalWrite(pirPin, LOW); // make pirPin low
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    digitalWrite(ledPhoton, LOW);
    digitalWrite(ledLocked, LOW);
    
    // motion sensor calibration
    delay(10000);
}

void loop() {
    strip.setPixelColor(0, PixelColorRed); // locked door
    strip.show();
    delay(3000);
    
    // used to check readings on temp and light sensor while programming circuit
    /*data0 = analogRead(tempPin);
    data1 = analogRead(lightPin);
    Serial.println(data0);
    Serial.println(data1);*/

    //Checking if cat wants out
    if (digitalRead(pirPin) == HIGH)
    {
        strip.setPixelColor(0, PixelColorPurple); // motion is detected, checking if true
        strip.show();
        delay(10000);
        // motion still detected
        if (digitalRead(pirPin) == HIGH)
        {
            // notify with leds
            digitalWrite(greenLedPin, HIGH);
            delay(500);
            digitalWrite(greenLedPin, LOW);
            
            strip.setPixelColor(0, PixelColorYellow); // door may open soon
            strip.show();

            // cat wants out - used with Particle dashboard
            Particle.publish("Cat wants to go outside!");
            delay(1500);

            // check if cat can go out
            if (checker())
            {
                // unlock door and lock door after cat leaves
                unlockDoor();
            }
            else
            {
                strip.setPixelColor(0, PixelColorRed); // locked door
                strip.show();
                Particle.publish("Conditions not met - cat stays inside!");
                // door remains locked
            }
        }
        else
        {
            // false positive
            strip.setPixelColor(0, PixelColorRed); // locked door
            strip.show();
            // false alarm - error checking because sometimes motion dector detects a ghost... :o
            //falsePositive();
        }
    }
    // no motion detected
    else if (digitalRead(pirPin) == LOW)
    {
        digitalWrite(redLedPin, HIGH);
        delay(500);
        digitalWrite(redLedPin, LOW);
    }
    // set variable back to initial value
    digitalWrite(ledPhoton, HIGH);
    delay(15000);
    digitalWrite(ledPhoton, LOW);
}

//checker function
bool checker()
{
    data0 = analogRead(tempPin); // reads data from temp sensor
    data1 = analogRead(lightPin); // reads data from light sensor
    // checks to make sure temp is above 40 and below 85 degrees Farenheit
    // checks to make sure it is light enough outside (1100 complete darkness)
    if ((data0 > 670 && data0 < 992) && (data1 < 4100 && data1 > 300)) // not too bright -> might be sun exploding
    {
        return true;
    }
    else
    {
        return false;
    }
}

void unlockDoor()
{
    bool locked;
    // let cat out
    strip.setPixelColor(0, PixelColorGreen); // unlock door
    strip.show();
    locked = false;
    
    delay(2000);
    ButtonOut = digitalRead(ButtonPIN);
    
    int t = 0;
    // Simulates cat going through door
    while (!locked)
    {
        strip.setPixelColor(0, PixelColorBlue); // door may close soon
        strip.show();
        delay(1500);
        // simulates cat leaving through door
        if (ButtonOut == true)
        {
            // wait for cat to go through door
            delay(3000);
            locked = true;
            strip.setPixelColor(0, PixelColorRed); // lock door
            strip.show();
            break;
        }
        delay(500);
        // automatically locks door if cat doesn't go out
        if (t > 10)
        {
            locked = true;
            strip.setPixelColor(0, PixelColorRed); // lock door
            strip.show();
        }
        t++;
    }
}
