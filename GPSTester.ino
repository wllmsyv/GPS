/* 
 * Testbech
 *  Board: Atmega2650
 *  Breakout: PA6H
 *  Using Serial2 pins 16 & 17
 */
 
#include "GPS.h"

GPS gps;
gps_location location;

void setup() {
  Serial.begin(115200);
  gps.init();
}



void loop() {
  if(gps.new_location_ready()){
    gps.get_new_location(&location);
    Serial.print("Lat: ");
    Serial.print(location.latitude);
    Serial.print(", Lon: ");
    Serial.print(location.longitude);
    Serial.print(", Time: ");
    Serial.print(location.ping_time);
    Serial.print(", Spd: ");
    Serial.print(location.speed_knots);
    Serial.print(", Heading: ");
    Serial.print(location.heading);
    Serial.print(", Date: ");
    Serial.print(location.ping_date);
    Serial.println();
    Serial.println(location.msg);
  }
}



void serialEvent2(){
  while(Serial2.available() > 0) {
    gps.add_char((char)Serial2.read());     
  }
}


