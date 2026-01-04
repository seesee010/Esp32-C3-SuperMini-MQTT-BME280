/*
  seesee010
  2026-01

  BME + MQTT

  Details:
  Esp32 C3 Super Mini + BME280 - mqtt -> Home Assistant

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.

  Dependencies:
  - BME280I2C.h by Tyler Glenn (GPL-3.0)
  - AViShaMQTT.h by Ajang (GPL-3.0)
  - MQTT.h by Joel Gaehwiler (MIT License)
  - Wire.h (Arduino Core)

==========

  BME + MQTT

  Details:
  Esp32 C3 Super Mini + BME280 - mqtt -> Home Assistant

  Connection:
  Sensor -> Board

  Pinout:
  VCC -> 3.3V
  Gnd -> Gnd
  SCL -> 9
  SDA -> 8

==========

*/

// secret config
#if __has_include("config.h")
  #include "config.h"
  const bool is_using_config = true;

#else
  #include "config.example.h"
  const bool is_using_config = false;

#endif

#define baudrate 115200

#define pin_sda 8
#define pin_scl 9

// using the BME280I2C.h by Tyler Glenn
// gpl 3.0 license
#include <BME280I2C.h>

// AViShaMQTT.h by Ajang
// gpl 3.0 license
#include <AViShaMQTT.h>

#include <Wire.h>

const char *topicTemp = "homeassistant/outside/0/temp";
const char *topicPres = "homeassistant/outside/0/pres";
const char *topicHum = "homeassistant/outside/0/hum";

AViShaMQTT mqtt(ssid, password, mqtt_server, 1883, mqtt_user, mqtt_pass);

BME280I2C bme;

float temp(NAN), hum(NAN), pres(NAN);

void getData(Stream *data) {

// from BME_280_I2C_Test

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);  

   bme.read(pres, temp, hum, tempUnit, presUnit);
// ##########

  delay(500);
}

void sendData(float temp, float pres, float hum) {

  // send -> HA Server
  mqtt.publish(topicTemp, String(temp));
  mqtt.publish(topicPres, String(pres));
  mqtt.publish(topicHum, String(hum));
}

void setup() {
  Serial.begin(baudrate);
  delay(1000);

  Serial.println("\n=== START ===\n");

  Wire.begin(pin_sda, pin_scl);
  if (!bme.begin()) {
    Serial.println("BME280 is not connected!");
    while(1) delay(1000);
  }
  Serial.println("BME280 is oK!");

  Serial.println("Config.h in use?");
  Serial.print(is_using_config);

  mqtt.begin();
}

void loop() {
  mqtt.loop();
  
  // Connection Check
  if (!mqtt.isConnected()) {
    Serial.println("MQTT is DISCONNECTED!");
    delay(1000);
    return;
  }
  
  Serial.println("--- read sensor ---");
  
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);
  bme.read(pres, temp, hum, tempUnit, presUnit);
  
  sendData(temp, pres / 100000.0, hum);

  Serial.print("T:"); Serial.print(temp);
  Serial.print(" H:"); Serial.print(hum);
  Serial.print(" P:"); Serial.println(pres / 100000.0);
  
  Serial.println("Publishing...");

  
  Serial.println("Done!\n");
  delay(3500);
}