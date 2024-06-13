#include <gps/gps.h>
#include <LoRa_E220.h>
class Lora {
 public:
  void init();
  void send(Location location);
  Location read();

 private:
  int timeout = 200;
};