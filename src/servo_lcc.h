#ifndef SERVO_LCC_H
#define SERVO_LCC_H

#include <Arduino.h>
#include <vector>
#include "servo_easing.h"

/**
 * Class Position_LCC represents one position in a servo.
 */
class Position_LCC {
  public:
    Position_LCC(uint8_t positionNumber,
                const char *positionDescription,
                uint8_t positionAngle,
                uint16_t eventMove,
                uint16_t eventLeaving,
                uint16_t eventReached);

    uint8_t getPositionNumber() { return this->positionNumber; }
    const char *getPositionDescription() { return this->positionDescription; }
    uint8_t getPositionAngle() { return this->positionAngle; }
    uint16_t getEventMove() { return this->eventMove; }
    uint16_t getEventLeaving() { return this->eventLeaving; }
    uint16_t getEventReached() { return this->eventReached; }

  private:
    uint8_t positionNumber;
    const char *positionDescription;
    uint8_t positionAngle;
    uint16_t eventMove;
    uint16_t eventLeaving;
    uint16_t eventReached;

};

/**
 * Class Servo_LCC represents one servo.
 */
class Servo_LCC {
  public:
    Servo_LCC(uint8_t servoNumber, uint8_t pin);

    void addPosition(uint8_t positionNumber,
                          const char *positionDescription,
                          uint8_t positionAngle,
                          uint16_t eventMove,
                          uint16_t eventLeaving,
                          uint16_t eventReached);
    
    void setEventToggle(uint16_t eventToggle) { this->eventToggle = eventToggle; }

    bool eventIndexMatchesThisServo(uint16_t index);

    void eventReceived(uint16_t index);

    void process();

    void print();

  private:
    uint8_t servoNumber;
    uint8_t pin;
    uint16_t eventToggle;

    std::vector<Position_LCC> positions;

    Servo servo;
    ServoEasing servoEasing;


};

#endif
