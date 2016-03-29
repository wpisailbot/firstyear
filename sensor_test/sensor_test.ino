#include "Arduino.h"
#include "PPM.h"
#include <Wire.h>

int compassAddress = 0x80; // From datasheet compass address is 0x42
// shift the address 1 bit right, the Wire library only needs the 7
// most significant bits for the address
int reading = 0;

static const char pow_pin = 14;
PPM pins(2);
void setup() {
  Serial.begin(9600);

  // Compass sensor setup.
  Wire.begin();       // join i2c bus (address optional for master)
  Serial.println("Hello World");
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(12, HIGH); // Power supply for wind direction sensor
  digitalWrite(13, LOW); // Power supply for wind direction sensor
  digitalWrite(3, HIGH); // Power supply for GPS
  digitalWrite(4, LOW); // Power supply for GPS
}

void loop() {
  bool loop_run = true;
  if (Serial.available()) {
    while (loop_run) {
      while (!Serial.available()) continue;
      char chr = Serial.read();
      Serial.print(char(chr));
      if (chr == '\n' && !Serial.available()) loop_run = false;
    }
  }

  Serial.print("PPM: ");
  Serial.print(pins.getChannel(0));
  Serial.print(" ");
  Serial.print(pins.getChannel(1));
  Serial.print(" ");
  Serial.print(pins.getChannel(2));
  Serial.print(" ");
  Serial.print(pins.getChannel(3));
  Serial.print(" ");
  Serial.print(pins.getChannel(4));
  Serial.print(" ");
  Serial.println(pins.getChannel(5));

  // Do wind sensor stuff.
  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(compassAddress);  // transmit to device
  // the address specified in the datasheet is 66 (0x42)
  // but i2c adressing uses the high 7 bits so it's 33
  Wire.write('A');        // command sensor to measure angle  (A)
  Wire.endTransmission(); // stop transmitting

  // step 2: wait for readings to happen
  delay(10); // datasheet suggests at least 6000 microseconds

  // step 3: request reading from sensor
  Wire.requestFrom(compassAddress, 2); // request 2 bytes from slave device #33

  Serial.print("Analog Ports:");
  for (int i = 0; i < 6; ++i) {
    Serial.print(" ");
    Serial.print(analogRead(i));
  }
  Serial.println();

  // step 4: receive reading from sensor
  Serial.print("Wind: ");
  if (2 <= Wire.available()) // if two bytes were received
  {
    reading = Wire.read();  // receive high byte (overwrites previous reading)
    reading = reading << 8; // shift high byte to be high 8 bits
    reading += Wire.read(); // receive low byte as lower 8 bits
    reading /= 10;
    Serial.println(reading); // print the reading
  } else {
    Serial.println("Data not available");
  }

  delay(200);

}
