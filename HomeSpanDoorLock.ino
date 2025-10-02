#include "HomeSpan.h"
#include "Doorbell.h"
#include "Lock.h"

#define IN0              0
#define IN1              1
#define IN2              2
#define IN3              3
#define UNLOCKED_ENDSTOP 20
#define LOCKED_ENDSTOP   21
#define DOORBELL_SWITCH  10


#define DOORBELL_SWITCH  10


void setup()
{
  Serial.begin(115200);
  homeSpan.setLogLevel(1); 

  homeSpan.setPairingCode("11122333");
  homeSpan.setQRID("111-22-333");
  homeSpan.setWifiCredentials("HeliPort","4a00907dcb"); // change credentials

  homeSpan.begin(Category::Other,"DeadBolt"); 
 
  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Doorbell");
    new DoorBell(DOORBELL_SWITCH);

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Lock");
    new DoorLock(IN0, IN1, IN2, IN3, UNLOCKED_ENDSTOP, LOCKED_ENDSTOP);
      
} // end of setup()

//////////////////////////////////////

void loop()
{
  homeSpan.poll();
  
}
