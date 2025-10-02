# HomeSpanDoorLock
This project implements a smart door lock system using an ESP32C3 microcontroller and the HomeSpan library. It allows you to control and monitor the lock state, endstops, and doorbell via HomeKit.

## Pin Mapping

| Name              | Pin | Description                |
|-------------------|-----|----------------------------|
| IN0               | 0   | Motor input/control        |
| IN1               | 1   | Motor input/control        |
| IN2               | 2   | Motor input/control        |
| IN3               | 3   | Motor input/control        |
| UNLOCKED_ENDSTOP  | 20  | Endstop for unlocked state |
| LOCKED_ENDSTOP    | 21  | Endstop for locked state   |
| DOORBELL_SWITCH   | 10  | Doorbell button input      |
