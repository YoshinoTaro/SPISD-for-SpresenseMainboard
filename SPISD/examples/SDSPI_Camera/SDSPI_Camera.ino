/**
 * SPISD read write test
 * Copyright 2021 Yoshino Taro
 * License: GNU General Public License V3
 * (Because Arduino SD library is licensed with this.)
 */
#include <SPI.h>
#include <SPISD.h>
#include <Camera.h>

SpiSDClass SD(SPI5);

const int intPin = 0;
static bool bButtonPressed = false;
static int gCounter = 0;

void changeState () {
  bButtonPressed = true;
}

void setup() {
  Serial.begin(115200);
  if (!SD.begin(SPI_FULL_SPEED)) {
    Serial.println("SD.begin() failed");
    return;
  }

  theCamera.begin();
  theCamera.setStillPictureImageFormat(
    CAM_IMGSIZE_QUADVGA_H ,CAM_IMGSIZE_QUADVGA_V
   ,CAM_IMAGE_PIX_FMT_JPG);

  attachInterrupt(digitalPinToInterrupt(intPin) ,changeState ,FALLING);
}

void loop() {
  if (!bButtonPressed) return;  
  Serial.println("button pressed");
  digitalWrite(LED0, HIGH);
  CamImage img = theCamera.takePicture();
  if (img.isAvailable()) {
      char filename[16] = {0};
      sprintf(filename, "PICT%03d.JPG", gCounter);
      SpiFile myFile = SD.open(filename,FILE_WRITE);
      myFile.write(img.getImgBuff(), img.getImgSize());
      myFile.close();
      ++gCounter;
  }
  bButtonPressed = false;
  digitalWrite(LED0, LOW);
}
