#include <Arduino.h>
#include <Wire.h>
#include <M5TimerCAM.h>

void setup()
{
  Serial2.begin(115200, SERIAL_8N1, 4, 13); // Connection to Main Device
  Serial.begin(115200);                     // Debug

  TimerCAM.begin();
  TimerCAM.Camera.begin();
  /**
   *
   * Settings for camera
   * Changes in the actual "config" struct need to be done in the library Camera_Class.cpp file
   * .grab_mode     = CAMERA_GRAB_LATEST,
   */
  TimerCAM.Camera.sensor->set_pixformat(TimerCAM.Camera.sensor, PIXFORMAT_JPEG);
  TimerCAM.Camera.sensor->set_framesize(TimerCAM.Camera.sensor, FRAMESIZE_240X240);
  TimerCAM.Camera.sensor->set_quality(TimerCAM.Camera.sensor, 10);
  TimerCAM.Camera.sensor->set_vflip(TimerCAM.Camera.sensor, 1);
  TimerCAM.Camera.sensor->set_hmirror(TimerCAM.Camera.sensor, 0);
}

void loop()
{
  // Continuously check for requests from the main device (in main loop because device does not need to handle other task, so polling is fine)
  /**
   * Protocol:
   * 0x01: Request size of image
   * 0x02: Request actual image (buffer data)
   * 0x03: Request reset of camera (frees image buffer if something went wrong)
   */
  if (Serial2.available() > 0)
  {
    int req_code;
    req_code = Serial2.read();
    switch (req_code)
    {
    case 0x01: //
      Serial.println("got size request");
      if (TimerCAM.Camera.get())
      {
        Serial.print("size: ");
        Serial.println(TimerCAM.Camera.fb->len);
        uint16_t size = TimerCAM.Camera.fb->len;

        Serial2.print(String(size));
        Serial.write((uint8_t)size);
      }
      else
      {
        Serial.println("failed to get size");
        Serial2.write(-1);
      }
      break;
    case 0x02:
      Serial.println("got image request");
      Serial2.write(TimerCAM.Camera.fb->buf, TimerCAM.Camera.fb->len);
      TimerCAM.Camera.free();
      break;
    case 0x03:
      Serial.println("got reset request");
      TimerCAM.Camera.free();
      break;
    default:
      Serial2.write(-2);
      break;
    }
  }
}
