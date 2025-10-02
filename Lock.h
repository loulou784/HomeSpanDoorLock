#include <Stepper.h>

#define TARGET_STATUS_LOCKED    1
#define TARGET_STATUS_UNLOCKED  0

#define CURRENT_STATUS_UNLOCKED 0
#define CURRENT_STATUS_LOCKED   1
#define CURRENT_STATUS_JAMMED   2
#define CURRENT_STATUS_UNKNOWN  3

#define STATE_UNLOCKING 0
#define STATE_LOCKING   1
#define STATE_IDLE      3
#define STATE_TIMEOUT   4

#define kTIMEOUT_MS 30000
#define kSTEP 10

struct DoorLock : Service::LockMechanism {  // Door LockMechanism

  SpanCharacteristic *current;  // reference to the Current Door State Characteristic
  SpanCharacteristic *target;   // reference to the Target Door State Characteristic
  Stepper *stepper;

  int in0_pin;
  int in1_pin;
  int in2_pin;
  int in3_pin;
  int unlock_endstop_pin;
  int lock_endstop_pin;

  int lockState = STATE_IDLE;
  int startMillis = 0;

  DoorLock(int in0, int in1, int in2, int in3, int unlock_endstop, int lock_endstop)
    : Service::LockMechanism() {  // constructor() method
    in0_pin = in0;
    in1_pin = in1;
    in2_pin = in2;
    in3_pin = in3;
    unlock_endstop_pin = unlock_endstop;
    lock_endstop_pin = lock_endstop;
    
    // Init Endstop pins
    pinMode(unlock_endstop_pin, INPUT_PULLUP);
    pinMode(lock_endstop_pin, INPUT_PULLUP);

    // Init stepper code
    stepper = new Stepper(2048, in0_pin, in1_pin, in2_pin, in3_pin);
    stepper->setSpeed(15);

    // TODO: Set lock to a known state, UNLOCKED?
    //int startMillis = millis();
    //while((millis() - startMillis < 5000) && digitalRead(unlock_endstop_pin) == LOW) {
    //  stepper->step(10);
    //}

    if(digitalRead(unlock_endstop_pin) == LOW) {
      // The door is in the unlocked position
      current = new Characteristic::LockCurrentState(CURRENT_STATUS_UNLOCKED);  
      target = new Characteristic::LockTargetState(TARGET_STATUS_UNLOCKED);    
    } else if(digitalRead(lock_endstop_pin) == LOW) {
      // The door is in the locked position
      current = new Characteristic::LockCurrentState(CURRENT_STATUS_LOCKED);  
      target = new Characteristic::LockTargetState(TARGET_STATUS_LOCKED);    
    } else {
      current = new Characteristic::LockCurrentState(CURRENT_STATUS_UNKNOWN);  
      target = new Characteristic::LockTargetState(TARGET_STATUS_UNLOCKED);    
    }
  }                                                     // end constructor

  boolean update() {
    if (target->getNewVal() == TARGET_STATUS_UNLOCKED) {
      startMillis = millis();
      lockState = STATE_UNLOCKING;
      Serial.println("Unlocking requested");
    } else if (target->getNewVal() == TARGET_STATUS_LOCKED) {
      startMillis = millis();
      lockState = STATE_LOCKING;
      Serial.println("Locking requested");
    }
    return (true);  // return true
  }

  void loop() {
    switch(lockState) {
      case STATE_IDLE:
        lockState = STATE_IDLE;
        startMillis = 0;
        break;
      case STATE_LOCKING:
        if(millis() - startMillis <= kTIMEOUT_MS) {
          if(digitalRead(lock_endstop_pin) != LOW) {
            stepper->step(-kSTEP);
          } else {
            // Success in reaching action
            lockState = STATE_IDLE;
            current->setVal(CURRENT_STATUS_LOCKED);
            Serial.println("Door locked sucessfully");
          }
        } else {
          // We timed out
          lockState = STATE_TIMEOUT;
          Serial.println("Door action timeout");
        }
        break;
      case STATE_UNLOCKING:
        if(millis() - startMillis <= kTIMEOUT_MS) {
          if(digitalRead(unlock_endstop_pin) != LOW) {
            stepper->step(kSTEP);
          } else {
            // Success in reaching action
            lockState = STATE_IDLE;
            current->setVal(CURRENT_STATUS_UNLOCKED);
            Serial.println("Door unlocked sucessfully");
          }
        } else {
          // We timed out
          lockState = STATE_TIMEOUT;
          Serial.println("Door action timeout");
        }
        break;
      case STATE_TIMEOUT:
        lockState = STATE_IDLE;
        current->setVal(CURRENT_STATUS_UNKNOWN);
        Serial.println("Door status unknown");
        break;
    }
  }
};
