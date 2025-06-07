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

    /**
     * Allows the initial angle, target angle and mid angle to all be set to position 1 (Mid).
     */
    void setInitialAngles(uint8_t initialAngle);

    void setSendEventCallbackFunction(void (*sendEvent)(uint16_t eventIndexToSend)) { this->sendEvent = sendEvent; }

    bool eventIndexMatchesThisServo(uint16_t index);

    void eventReceived(uint16_t index);

    void process();

    void print();

    /**
     * Need to arrange for the servo easing callback functions to 
     * call here and then use the sendEvent() call back function to
     * pass them back to main.cpp where the event will be sent.
     */

     /**
      * Should we sent a leaving event if the servos moves from pos 1 (mid) to
      * pos 1 (mid)?? DITTO for other positions? Should be reached instead??
      */

  private:
    uint8_t servoNumber;
    uint8_t pin;
    uint16_t eventToggle;

    std::vector<Position_LCC> positions;

    // Call back function to send events.
    void (*sendEvent) (uint16_t eventIndexToSend);

    uint16_t getLeavingEventForCurrentAngle();

    Servo servo;
    ServoEasing servoEasing;
};

#endif
