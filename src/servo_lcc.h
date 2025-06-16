#ifndef SERVO_LCC_H
#define SERVO_LCC_H

#include <Arduino.h>
#include "LCC_Node_Component_Base.h"
#include "servo_easing.h"
#include <vector>

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
class Servo_LCC : public LCC_Node_Component_Base {
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

    // void setSendEventCallbackFunction(void (*sendEvent)(uint16_t eventIndexToSend)) { this->sendEvent = sendEvent; }

    // bool eventIndexMatchesThisServo(uint16_t index);
    bool eventIndexMatches(uint16_t index) override;

    bool eventIndexMatchesCurrentState(uint16_t index);

    void eventReceived(uint16_t index);

    /**
     * Called when the ServoEasing object has moved its servo to any position.
     * It determines the correct event and sends it.
     */
    void handleReachedAngle(uint8_t currentAngle, AngleDirection direction);

    /***
     * Called when sending initial events.
     * Used to initialise JMRI when JMRI starts after the node has started.
     */
    void sendEventsForCurrentState();

    void process();

    void print();

    /**
     * servoEasing has been made public to allow the following;-
     *  1. In ServoEasing::update() when the servo reaches any of the 3 positions
     *     the code calls the callback function reachedAngle() which
     *     calls reachedAngleCallbackFunction() in main.cpp.
     *  2. reachedAngleCallbackFunction() calls Servo_LCC::handleReachedAngle()
     *     for the appropriate Servo_LCC object.
     *  3. Servo_LCC::handleReachedAngle() uses the current angle and direction to 
     *     determine the appropriate event to send.
     *  4. The callback function sendEventCallbackFunction() in main.cpp is
     *     used to send the event.
     */
    ServoEasing servoEasing;

  private:
    uint8_t servoNumber;
    uint8_t pin;
    uint16_t eventToggle;

    std::vector<Position_LCC> positions;

    // // Call back function to send events.
    // void (*sendEvent) (uint16_t eventIndexToSend);

    uint16_t getLeavingEventForCurrentAngle();

    Servo servo;
};

#endif
