#ifndef SERVO_LCC_H
#define SERVO_LCC_H

#include <Arduino.h>

/**
 * Class Servo represents one servo.
 */
class Servo_LCC {
  public:
    Servo_LCC(uint8_t pin);

    void setEvents();






  private:
    uint8_t pin;
    // uint16_t eventIndex
    // uint16_t eventIndex
    // uint16_t eventIndex
    // uint16_t eventIndex

    // 3 positions with 3 events each + 1 for the servo toggle event.




};



#endif