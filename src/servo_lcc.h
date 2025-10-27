#ifndef SERVO_LCC_H
#define SERVO_LCC_H

/**
 * TO DO: 
 * 
 * Each servo to have a boolean locked flag.
 * Also, a new event, “Check, Lock and Move” which checks if the item is locked.
 *
 * If locked, an event is sent to indicate that the item is locked and didn’t move.
 * If not locked, the item is locked and moved. When the move has completed the appropriate event is sent as normal.
 *
 * A new event will allow the item to be unlocked.
 */

#include <Arduino.h>
#include "LCC_Node_Component_Base.h"
#include "servo_easing.h"
#include <vector>

/**
 * #defines for the indexes to the servo positions vector.
 * Would be better if they were an enum (or an enum class).
 */
#define POS_THROWN 0
#define POS_MID 1
#define POS_CLOSED 2

/**
 * Class Position_LCC represents one position in a servo.
 */
class Position_LCC {
  public:
    Position_LCC(uint8_t positionNumber,
                const char *positionDescription,
                uint8_t positionAngle,
                uint16_t eventMove,
                uint16_t eventMoveConditional,
                uint16_t eventLeaving,
                uint16_t eventReached);

    uint8_t getNumber() { return this->positionNumber; }
    const char *getDescription() { return this->positionDescription; }
    uint8_t getAngle() { return this->positionAngle; }
    uint16_t getEventMove() { return this->eventMove; }
    uint16_t getEventMoveConditional() { return this->eventMoveConditional; }
    uint16_t getEventLeaving() { return this->eventLeaving; }
    uint16_t getEventReached() { return this->eventReached; }
    void setAngle(uint8_t angle) { this->positionAngle = angle; }

  private:
    uint8_t positionNumber;
    const char *positionDescription;
    uint8_t positionAngle;
    uint16_t eventMove;
    uint16_t eventMoveConditional;
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
                          uint16_t eventMoveConditional,
                          uint16_t eventReached,
                          uint16_t eventLeaving);

    // Called when a configuration change is made to the servo angles.
    void updatePosition(uint8_t positionNumber, uint8_t positionAngle);

    // Called when a configuration change is made to the servo speed.
    void updateDelaymS(unsigned long delaymS) { this->servoEasing.setDelaymS(delaymS); }
    
    void setEventToggle(uint16_t eventToggle) { this->eventToggle = eventToggle; }
    void setEventUnLock(uint16_t eventUnLock) { this->eventUnLock = eventUnLock; }
    void setEventLocked(uint16_t eventLocked) { this->eventLocked = eventLocked; }

    /**
     * Allows the initial angle and mid angle to be set to position 1 (Mid).
     * This causes the servo to snap to the mid angle on startup.
     */
    void setInitialAngles(uint8_t initialAngle);

    /**
     * Sets the target angle to one of the positions (thrown, mid or closed).
     * This causes the servo to ease to this angle on startup.
     */
    void setInitialPosition(uint8_t initialPosition);

    /**
     * Added so that the crossover object can start the servos moving to a target position.
     */
    uint8_t getAngleForPosition(uint8_t position) { return positions[position].getAngle(); }

    bool eventIndexMatches(uint16_t index) override;

    bool eventIndexMatchesCurrentState(uint16_t index) override;

    void eventReceived(uint16_t index);

    /**
     * Called when the ServoEasing object has moved its servo to any position.
     * It determines the correct event and sends it.
     */
    void handleReachedAngle(uint8_t currentAngle, ServoEasing::AngleDirection direction);

    /***
     * Called when sending initial events.
     * Used to initialise JMRI when JMRI starts after the node has started.
     */
    void sendEventsForCurrentState() override;

    // void process();
    void loop();

    void print();

    bool isThrown() { return servoEasing.getCurrentAngle() == positions[POS_THROWN].getAngle() ? true : false; }
    bool isClosed() { return servoEasing.getCurrentAngle() == positions[POS_CLOSED].getAngle() ? true : false; }

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

    uint16_t getLeavingEventForCurrentAngle(); // Made public so it can be used by the crossover object.

  private:
    uint8_t servoNumber;
    uint8_t pin;
    uint16_t eventToggle; // If received causes the servo to move to its other position.
    uint16_t eventUnLock; // If received resets the locked flag.
    uint16_t eventLocked; // Sent if the servo cannot move as it is locked.
    bool servoLocked = false;

    std::vector<Position_LCC> positions;

    // Define the test cycles and track the current test cycle.
    enum Test { MOVE_TO_THROWN, WAIT_FOR_THROWN, MOVE_TO_CLOSED, WAIT_FOR_CLOSED };
    Test currentTest;

    void testLoop();
};

#endif
