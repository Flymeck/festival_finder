#include "lora.h"
#include <LoRa_E220.h>

#define ss 32
#define rst 12
#define dio0 2
#define localAddress 0x01

Location parseLocation(String loc);
String locationToString(Location location);
LoRa_E220 e220ttl(&Serial2, 5, 19, 18);

void Lora::init() {
  e220ttl.begin();
  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(18, "init Radio");
  Serial.println(rs.getResponseDescription());
}

Location getEmptyLocationLora() {
  Location location;
  location.lat = 0;
  location.lon = 0;
  return location;
}

unsigned long loraReadStart = 0;
Location Lora::read() {
  if (e220ttl.available()>1) {
    // read the String message
    ResponseContainer rc = e220ttl.receiveMessage();
    // Is something goes wrong print error
    if (rc.status.code!=1){
      Serial.println(rc.status.getResponseDescription());
    }else{
      // Print the data received
      Serial.println(rc.data);
      return parseLocation(rc.data);
    }
  }
  return getEmptyLocationLora();
}

Location parseLocation(String loc) {
  Location location;
  int semicolonIndex = loc.indexOf(";");
  String lat = loc.substring(0, semicolonIndex);
  String lon = loc.substring(semicolonIndex + 1);
  location.lat = lat.toFloat();
  location.lon = lon.toFloat();
  return location;
}

String locationToString(Location location) {
  String lat = String(location.lat, 6);
  String lon = String(location.lon, 6);
  return lat + ";" + lon;
}

void Lora::send(Location location) {
  String locString = locationToString(location);
  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(18, locationToString(location));
  Serial.print("Sending packet");
  Serial.println(rs.getResponseDescription());
}