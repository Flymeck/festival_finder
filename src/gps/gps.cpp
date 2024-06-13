#include "gps.h"
#include <TinyGPSPlus.h>

#include <Arduino.h>

#define RXD1 2
#define TXD1 4

const float EARH_RADIUS = 6371.0;
TinyGPSPlus gps;

void GPS_class::displayInfo() {
  Serial.println(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    Serial.print(" Speed: ");
    Serial.print(gps.speed.kmph());
    Serial.print(" Date/Time: ");
    Serial.print(gps.date.value());
    Serial.print(" Satellites: ");
    Serial.print(gps.satellites.value());
    Serial.print(" Altitude: ");
    Serial.print(gps.altitude.meters());
    Serial.println(" ");
  } else {
    Serial.println(F("INVALID"));
  }
}
double degreesToRadians(double degrees) { return degrees * M_PI / 180.0; }

int GPS_class::distance(float lat, float lon) {
  float lat1 = gps.location.lat();
  float lon1 = gps.location.lng();
  float lat2 = lat;
  float lon2 = lon;
  return calculateDistance(lat1, lon1, lat2, lon2);
}

int GPS_class::calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  lat1 = degreesToRadians(lat1);
  lon1 = degreesToRadians(lon1);
  lat2 = degreesToRadians(lat2);
  lon2 = degreesToRadians(lon2);

  double dlat = lat2 - lat1;
  double dlon = lon2 - lon1;
  double a = std::sin(dlat / 2.0) * std::sin(dlat / 2.0) +
             std::cos(lat1) * std::cos(lat2) * std::sin(dlon / 2.0) *
                 std::sin(dlon / 2.0);
  double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));

  // Distance in kilometers
  double distance = EARH_RADIUS * c * 1000.0;  // Convert to meters

  return distance;
}

int GPS_class::calculateAngle(float lat1, float lon1, float lat2, float lon2) {
  lat1 = degreesToRadians(lat1);
  lon1 = degreesToRadians(lon1);
  lat2 = degreesToRadians(lat2);
  lon2 = degreesToRadians(lon2);

  // Calculate the differences
  double dLon = lon2 - lon1;

  // Calculate the initial bearing using the Haversine formula
  double y = sin(dLon) * cos(lat2);
  double x = cos(lat1) * sin(lat2) - (sin(lat1) * cos(lat2) * cos(dLon));
  double initialBearing = atan2(y, x);

  // Convert the initial bearing from radians to degrees
  initialBearing = fmod((initialBearing + 2.0 * M_PI), (2.0 * M_PI));
  initialBearing = initialBearing * (180.0 / M_PI);
  // Adjust the angle to be relative to the north (clockwise from positive
  // X-axis)
  initialBearing = 90.0 - initialBearing;

  // Ensure the result is in the range [0, 360)
  if (initialBearing < 0.0) {
    initialBearing += 360.0;
  }

  return initialBearing;
}

Location getEmptyLocation() {
  Location location;
  location.lat = 0;
  location.lon = 0;
  return location;
}

Location GPS_class::getLocation() {
  while (Serial1.available() > 0) {
     Serial.println("test 1");
    if (gps.encode(Serial1.read())) {
      Serial.println("test 2");
      displayInfo();
      if (gps.location.isValid()) {
        Location location;
        location.lat = gps.location.lat();
        location.lon = gps.location.lng();
        return location;
      } else {
        return getEmptyLocation();
      }
    }
  }
  return getEmptyLocation();
}

void GPS_class::init() {
  Serial1.begin(9600,SERIAL_8N1,RXD1,TXD1);
  Serial.println(F("GPS module is ready"));
}

void GPS_class::updateSerial() {
  delay(500);
  while (Serial.available()) {
    Serial1.write(
        Serial.read());  // Forward what Serial received to Software Serial Port
  }
  while (Serial1.available()) {
    Serial.write(
        Serial1
            .read());  // Forward what Software Serial received to Serial Port
  }
}