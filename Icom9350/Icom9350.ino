/*
    Icom IC-7000/706 to Codan 9350 Adaptor Software 
  
    Copyright (C) 2016 Mark Jessop <vk5qi@rfhead.net>

    Changelog:
      - v1.0.1: Fix the delays to work with a 8MHz crystal. Should also now work with
                any clock speed Arduino's delay functions support.

    Some Notes:
      - I used Arduino 1.6.4 to compile this code initially.
      - I'm using board definitions from here: http://highlowtech.org/?p=1695
      - New VK5ZM boards have 8MHz crystals, so the 8MHz (external) Clock option
        from the attiny board selection is appropriate.
      - For the correct fuse bits to be written, you first need to 'Burn Bootloader'
        within Arduino (though it doesn't actually burn a bootloader), then upload
        the code using an ISP (i.e. Ctrl + Shift + U)

    Interface PCB through to 9350 Pin Connections:
    - NGTs and Envoys use the 6-way 'microphone' connector.
    - Older codan rigs (9323, 8528) use a DB15 connector.
    - The 9350 itself uses MIL-Spec Amphenol connector

    Function        9350 Pin        6-way Pin       DB15 Pin        Interface PCB Pin
    ---------------------------------------------------------------------------------
    +12V            A (Red)         4               12+13           4
    Earth           B (Black)       6               14+15           6
    Tune            C (Blue)        1               4               1
    Scan            D (Green)       2               5               2
    Tune Status     E (Yellow)      3               11              3
    Not Connected   F

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/


// Pin definitions for VK5ZM designed ATTiny44 interface PCB.

#define ICOM_START    8 // Attiny44 Pin 5 - Pulled high 3.3K resistor.
                        // IC-7000 will pull this pin LOW to start tuning.
#define ICOM_KEY      7 // ATtiny44 Pin 6 - Output, via Transistor network. 

#define CODAN_TUNE    3 // ATTiny44 Pin 10 - Pulled high 3.3K resistor
#define CODAN_TUNEIO  1 // ATTiny44 Pin 12 - Pulled high 3.3K
#define CODAN_SCAN    2 // ATTiny44 Pin 11 - Output via transistor network. Inverted output.

#define LED           0 // ATTiny44 Pin 13 - LED. Pull low to activate.


void setup(){
    // Setup output pins.
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    pinMode(CODAN_SCAN, OUTPUT);
    pinMode(ICOM_KEY, OUTPUT);
    digitalWrite(CODAN_SCAN, LOW);
    digitalWrite(ICOM_KEY, LOW);

    // Setup Input Pins
    pinMode(ICOM_KEY, INPUT);
    pinMode(CODAN_TUNEIO, INPUT);
    pinMode(CODAN_TUNE, INPUT);

}

void loop(){
    // Wait until ICOM_START is pulled low.
    while(digitalRead(ICOM_START)){}

    // Delay for 200ms. This allows us to check if the Icom radio has requested a tune, or a tuner reset.
    delay(200); 

    // Check state of ICOM_START line.
    // If high, the Icom has requested a tuner reset. We could put the Codan 9350 into scan mode now.
    // If low, we have a tune request.
    if(digitalRead(ICOM_START)){
        //scan_mode();  // Currently don't do anything on tuner request, as going into scan mode
                        // causes the 9350 motor to reset, which takes about 10 seconds.
    }else{
        tune();
    }

    // Delay for a short time, so we don't re-tune accidentally.
    delay(400);
}

void tune(){
    // Turn off Indicator LED.
    digitalWrite(LED, HIGH);

    // Make sure we are not in scan mode.
    digitalWrite(CODAN_SCAN, LOW);

    // Assert ICOM_KEY line, indicating to the Icom we are in tune mode.
    digitalWrite(ICOM_KEY, HIGH);

    // Wait a bit for the Icom to start producing RF.
    delay(350); // Approx 350ms

    // Now we start to talk to the Codan tuner.

    // Assert 9350 tune line low.
    pinMode(CODAN_TUNE, OUTPUT);
    digitalWrite(CODAN_TUNE, LOW);

    // 9350 will now hold this line low until it finishes tuning.
    // Wait a little bit (300ms-ish) before releasing the line
    delay(300);

    // Release the line.
    pinMode(CODAN_TUNE, INPUT);

    delay(30);// Wait 30ms before trying to read the CODAN_TUNE line.

    // Wait until CODAN_TUNE goes high.
    while(digitalRead(CODAN_TUNE)==0){
        // Flash the LED while we wait.
        digitalWrite(LED, LOW); // On
        delay(100);
        digitalWrite(LED, HIGH); // Off
        delay(100);
    }

    // When line goes high, read CODAN_TUNEIO line immediately.
    // This line indicates if the tune has failed or succeeded
    if(digitalRead(CODAN_TUNEIO)){
      // Tune OK. Release ICOM Key line.
      digitalWrite(ICOM_KEY, LOW);
      // Turn LED On and leave on.
      digitalWrite(LED, LOW);
    }else{
      // Tune failed. Run through tune failed sequence.
      tune_failed();
    }
}

void tune_failed(){
    // We can tell the Icom that the tune has failed by releasing ICOM_KEY line for 20ms, 
    // holding it low for 200ms, then releasing it again.
    digitalWrite(ICOM_KEY, LOW);
    delayMicroseconds(20000); // 20ms
    digitalWrite(ICOM_KEY, HIGH);
    delay(200); // 200ms
    digitalWrite(ICOM_KEY, LOW);

    // Blink LED, then turn LED off.
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
}

void scan_mode(){
    // Assert Codan SCAN line (pull low)
    digitalWrite(CODAN_SCAN, HIGH);

    // Fast-blink LED.
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
}
