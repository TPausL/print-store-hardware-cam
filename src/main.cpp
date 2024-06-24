#include <Arduino.h>
#include <Wire.h>
#include <M5TimerCAM.h>

void sendImage()
{
}

RTC_DATA_ATTR bool first_start = true;
void setup()
{
  Serial2.begin(115200, SERIAL_8N1, 4, 13);
  Serial.begin(115200);
  TimerCAM.begin();
  TimerCAM.Camera.begin();
  TimerCAM.Camera.sensor->set_pixformat(TimerCAM.Camera.sensor, PIXFORMAT_JPEG);
  TimerCAM.Camera.sensor->set_framesize(TimerCAM.Camera.sensor, FRAMESIZE_240X240);
  TimerCAM.Camera.sensor->set_quality(TimerCAM.Camera.sensor, 10);
  TimerCAM.Camera.sensor->set_vflip(TimerCAM.Camera.sensor, 1);
  TimerCAM.Camera.sensor->set_hmirror(TimerCAM.Camera.sensor, 0);
}

void loop()
{
  if (Serial2.available() > 0)
  {
    int req_code;
    req_code = Serial2.read();
    switch (req_code)
    {
    case 0x01:
      Serial.println("got size request");
      if (TimerCAM.Camera.get())
      {
        Serial.println(TimerCAM.Camera.fb->len);
        uint16_t size = TimerCAM.Camera.fb->len;
        Serial.println(size);
        Serial2.print(String(size));
        Serial.write((uint8_t)size);
      }
      else
      {
        Serial2.write(5);
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
      Serial2.write(7);
      break;
    }
  }
}
