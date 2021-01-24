/**
 * SPISD read write test
 * Copyright 2021 Yoshino Taro
 * License: GNU General Public License V3
 * (Because Arduino SD library is licensed with this.)
 */
#include <SPI.h>
#include <SPISD.h>

SpiSDClass SD(SPI5);
SpiFile myFile;

void setup() {
  Serial.begin(115200);
  if (!SD.begin(SPI_FULL_SPEED)) {
    Serial.println("SD.begin() failed");
    return;
  }

  SD.mkdir("/test");
  myFile = SD.open("/test/test.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to test.txt...");
    for (int i = 0; i < 100; ++i) {
       myFile.println("testing 1, 2, 3, 4, 5, 6, 7, 8, 9, 0");
    }
    myFile.close();
    Serial.print("done.");
  } else {
    Serial.println("error opening test.txt");
  }

  myFile = SD.open("/test/test.txt");
  if (myFile) {
    Serial.println("/test/test.txt:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("error opening test.txt");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
