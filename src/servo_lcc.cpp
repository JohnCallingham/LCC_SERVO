#include "servo_lcc.h"

Position_LCC::Position_LCC(uint8_t positionNumber,
                const char *positionDescription,
                uint8_t positionAngle,
                uint16_t eventMove,
                uint16_t eventMoveConditional,
                uint16_t eventLeaving,
                uint16_t eventReached) {
  this->positionNumber = positionNumber;
  this->positionDescription = positionDescription;
  this->positionAngle = positionAngle;
  this->eventMove = eventMove;
  this->eventMoveConditional = eventMoveConditional;
  this->eventLeaving = eventLeaving;
  this->eventReached = eventReached;
}

Servo_LCC::Servo_LCC(uint8_t servoNumber, uint8_t pin) {
  this->servoNumber = servoNumber;
  this->pin = pin;
}

void Servo_LCC::addPosition(uint8_t positionNumber,
                          const char *positionDescription,
                          uint8_t positionAngle,
                          uint16_t eventMove,
                          uint16_t eventMoveConditional,
                          uint16_t eventReached,
                          uint16_t eventLeaving) {

  positions.push_back(Position_LCC (positionNumber,
                        positionDescription,
                        positionAngle,
                        eventMove,
                        eventMoveConditional,
                        eventLeaving,
                        eventReached));
}

void Servo_LCC::updatePosition(uint8_t positionNumber, uint8_t positionAngle) {
  positions[positionNumber].setAngle(positionAngle);
}

void Servo_LCC::setInitialAngles(uint8_t initialAngle) {
  servoEasing.initialise(servoNumber, pin);

  servoEasing.setMidAngle(initialAngle);

  // Set the angle to which the servo snaps on startup, usually to the mid position.
  servoEasing.setInitialAngle(initialAngle);
}

void Servo_LCC::setInitialPosition(uint8_t initialPosition) {
  // Set the target angle so that the servo eases to this position on startup.
  servoEasing.easeTo(positions[initialPosition].getAngle());
}

void Servo_LCC::print() {
  Serial.printf("\nServo pin=%d", this->pin);

  for (auto & position : positions) {
    Serial.printf("\n Position number=%d", position.getNumber());
    Serial.printf("\n Position description=%s", position.getDescription());
    Serial.printf("\n Position angle=%d", position.getAngle());
    Serial.printf("\n Position event move=%d", position.getEventMove());
    Serial.printf("\n Position event leaving=%d", position.getEventLeaving());
    Serial.printf("\n Position event reached=%d", position.getEventReached());
  }

  Serial.printf("\neventToggle=%d", this->eventToggle);
}

bool Servo_LCC::eventIndexMatches(uint16_t index) {
  // Check for one of the testing event indexes.
  if ((index == this->testStartEventIndex) ||
      (index == this->testStopEventIndex)) return true;

  // Check for one of the servo event indexes.
  if ((index == this->eventToggle) ||
      (index == this->eventUnLock) ||
      (index == this->eventLocked)) return true;

  // Check for one of the position event indexes.
  for (auto & position : positions) {
    if ((index == position.getEventMove()) ||
        (index == position.getEventMoveConditional()) ||
        (index == position.getEventLeaving()) ||
        (index == position.getEventReached())) return true;
  }

  return false;
}

bool Servo_LCC::eventIndexMatchesCurrentState(uint16_t index) {
  // Determine the position for this event index.
  for (auto & position : positions) {
    if (index == position.getEventReached()) {
      // Is the servo angle at this position?
      if (servoEasing.getCurrentAngle() == position.getAngle()) return true;
    }
    if (index == position.getEventLeaving()) {
      // Is the servo angle not at this position?
      if (servoEasing.getCurrentAngle() != position.getAngle()) return true;
    }
  }

  return false;
}

void Servo_LCC::eventReceived(uint16_t index) {
  /**
   * Handle the test cycle start and stop events.
   */
  if (index == testStartEventIndex) {
    Serial.printf("\nServo %d starting the testing cycle.", servoNumber);

    // Set the first test.
    currentTest = MOVE_TO_THROWN;

    // Set the timer so that testing starts immediately.
    testingTimer = millis();

    testing = true;
  }
  if (index == testStopEventIndex) {
    Serial.printf("\nServo %d stopping the testing cycle.", servoNumber);

    // Leave the servo at the mid position.
    servoEasing.moveTo(positions[POS_MID].getAngle());

    // Send the leaving events for both thrown and closed.
    // Send the reached event for the mid position.
    if (sendEvent) {
      sendEvent(positions[POS_THROWN].getEventLeaving());
      sendEvent(positions[POS_CLOSED].getEventLeaving());
      sendEvent(positions[POS_MID].getEventReached());
    }

    testing = false;
  }

  // Stop normal operation if testing.
  if (testing) return;

  /**
   * Handle the toggle event.
   */
  if (index == eventToggle) {
    // Start the servo moving to the other position.
    Serial.printf("\nServo %d moving to the other position ", servoNumber);
    Serial.printf(" current angle = %d", servoEasing.getCurrentAngle() );

    // If the servo is at position 0 (Thrown), move to position 2 (Closed).
    if (servoEasing.getCurrentAngle() == positions[POS_THROWN].getAngle()) {
      servoEasing.easeTo(positions[POS_CLOSED].getAngle());
      // Send the leaving event for this position.
      Serial.printf("\n%6ld Servo %d sending leaving event for position thrown", millis(), servoNumber);
      if (sendEvent) sendEvent(positions[POS_THROWN].getEventLeaving());
    }

    // // If the servo is at position 1 (Mid), no movement, send mid reached event.
    // if (servoEasing.getCurrentAngle() == positions[POS_MID].getAngle()) {
    //   // Send the reached event for this position.
    //   Serial.printf("\n%6ld Servo %d sending reached event for position mid", millis(), servoNumber);
    //   if (sendEvent) sendEvent(positions[POS_MID].getEventReached());
    // }

    // If the servo is at position 1 (Mid), move to position 2 (Closed).
    if (servoEasing.getCurrentAngle() == positions[POS_MID].getAngle()) {
      servoEasing.easeTo(positions[POS_CLOSED].getAngle());
      // Send the leaving event for this position.
      Serial.printf("\n%6ld Servo %d sending leaving event for position mid", millis(), servoNumber);
      if (sendEvent) sendEvent(positions[POS_MID].getEventLeaving());
    }

    // If the servo is at position 2 (Closed), move to position 0 (Thrown).
    if (servoEasing.getCurrentAngle() == positions[POS_CLOSED].getAngle()) {
      // servoEasing.setTargetAngle(positions[POS_THROWN].getAngle());
      servoEasing.easeTo(positions[POS_THROWN].getAngle());
      // Send the leaving event for this position.
      Serial.printf("\n%6ld Servo %d sending leaving event for position closed", millis(), servoNumber);
      if (sendEvent) sendEvent(positions[POS_CLOSED].getEventLeaving());
    }

    return;
  }

  /**
   * Handle the 'unconditional move to a position' event.
   */
  // Determine the target position for this event.
  for (auto & targetPosition : positions) {
    if (index == targetPosition.getEventMove()) {
      // Check if the servo is already at this target position.
      if (servoEasing.getCurrentAngle() == targetPosition.getAngle()) {
        // Servo is already at the target position so just send the reached event.
        Serial.printf("\n%6ld Servo %d sending reached event for position %d", millis(), servoNumber, targetPosition.getNumber());
        if (sendEvent) sendEvent(targetPosition.getEventReached());
      } else {
        // Start the servo moving to this position.
        Serial.printf("\nServo %d moving to position %d", servoNumber, targetPosition.getNumber());

        // Make the servo move to this position.
        servoEasing.easeTo(targetPosition.getAngle());

        // Send the leaving event for this position.
        Serial.printf("\n%6ld Servo %d sending leaving event", millis(), servoNumber);
        if (sendEvent) sendEvent(getLeavingEventForCurrentAngle());
      }
    }
  }

  /**
   * Handle the 'conditional move to a position' event.
   */
  // Determine the target position for this event.
  for (auto & targetPosition : positions) {
    if (index == targetPosition.getEventMoveConditional()) {
      // Check if the servo is locked.
      if (servoLocked) {
        // Send the servo locked event.
        Serial.printf("\n%6ld Servo %d sending locked event", millis(), servoNumber);
        if (sendEvent) sendEvent(eventLocked);
      } else {
        // The servo is not locked.

        // Lock the servo to prevent other movements from changing the position of this servo.
        servoLocked = true;

        // Check if the servo is already at this target position.
        if (servoEasing.getCurrentAngle() == targetPosition.getAngle()) {
          // Servo is already at the target position so just send the reached event.
          Serial.printf("\n%6ld Servo %d sending reached event for position %d", millis(), servoNumber, targetPosition.getNumber());
          if (sendEvent) sendEvent(targetPosition.getEventReached());
        } else {
          // Start the servo moving to this position.
          Serial.printf("\nServo %d moving to position %d", servoNumber, targetPosition.getNumber());

          // Make the servo move to this position.
          servoEasing.easeTo(targetPosition.getAngle());

          // Send the leaving event for this position.
          Serial.printf("\n%6ld Servo %d sending leaving event", millis(), servoNumber);
          if (sendEvent) sendEvent(getLeavingEventForCurrentAngle());
        }
      }
    }
  }
}

uint16_t Servo_LCC::getLeavingEventForCurrentAngle() {
  // Find the position object for the current angle.
  for (auto & position : positions) {
    if (position.getAngle() == servoEasing.getCurrentAngle()) {
      // Return the leaving event for this position.
      return position.getEventLeaving();
    }
  }

  return 0; // To keep the compiler happy!
}

void Servo_LCC::handleReachedAngle(uint8_t currentAngle, ServoEasing::AngleDirection direction) {
  // Determine the position for the current angle.
  if (currentAngle == positions[POS_THROWN].getAngle()) {
    Serial.printf("\n%6ld Servo %d sending reached event for position thrown", millis(), servoNumber);
    if (sendEvent) sendEvent(positions[POS_THROWN].getEventReached());
  } else if (currentAngle == positions[POS_MID].getAngle()) {
    // This is the mid position so send the appropriate event based on direction.
    if (direction == ServoEasing::AngleDirection::INCREASING_ANGLE) {
      Serial.printf("\n%6ld Servo number %d sending reached event for position mid", millis(), servoNumber);
      if (sendEvent) sendEvent(positions[POS_MID].getEventReached());
    } else {
      Serial.printf("\n%6ld Servo %d sending leaving event for position mid", millis(), servoNumber);
      if (sendEvent) sendEvent(positions[POS_MID].getEventLeaving());
    }
  } else if (currentAngle == positions[POS_CLOSED].getAngle()) {
    Serial.printf("\n%6ld Servo %d sending reached event for position closed", millis(), servoNumber);
    if (sendEvent) sendEvent(positions[POS_CLOSED].getEventReached());
  }
}

void Servo_LCC::sendEventsForCurrentState() {
  // Handle the servo being in position 0 (Thrown).
  if (servoEasing.getCurrentAngle() == positions[POS_THROWN].getAngle()) {
    if (sendEvent) sendEvent(positions[POS_THROWN].getEventReached());
    if (sendEvent) sendEvent(positions[POS_CLOSED].getEventLeaving());
  }

  // Handle the servo being in position 2 (Closed).
  if (servoEasing.getCurrentAngle() == positions[POS_CLOSED].getAngle()) {
    if (sendEvent) sendEvent(positions[POS_CLOSED].getEventReached());
    if (sendEvent) sendEvent(positions[POS_THROWN].getEventLeaving());
  }

  // Handle the servo being in position 1 (Mid).
  if (servoEasing.getCurrentAngle() == positions[POS_MID].getAngle()) {
    if (sendEvent) sendEvent(positions[POS_THROWN].getEventLeaving());
    if (sendEvent) sendEvent(positions[POS_CLOSED].getEventLeaving());
  }
}

void Servo_LCC::loop() {
  servoEasing.loop();
  if (testing) testLoop();
}

void Servo_LCC::testLoop() {
  if (millis() < testingTimer) return;

  // Time to move to the next part of the test cycle.

  switch (currentTest) { // ?? should be nextTest ??
    case MOVE_TO_THROWN:
      if (sendEvent) sendEvent(getLeavingEventForCurrentAngle());
      servoEasing.easeTo(positions[POS_THROWN].getAngle());
      testingTimer = millis() + 1000;
      currentTest = WAIT_FOR_THROWN;
      break;
    
    case WAIT_FOR_THROWN:
      // When at thrown move to the next part of the test cycle.
      if (servoEasing.getCurrentAngle() == positions[POS_THROWN].getAngle()) {
        testingTimer = millis() + 2000;
        currentTest = MOVE_TO_CLOSED;
      }
      break;
    
    case MOVE_TO_CLOSED:
      if (sendEvent) sendEvent(getLeavingEventForCurrentAngle());
      servoEasing.easeTo(positions[POS_CLOSED].getAngle());
      testingTimer = millis() + 1000;
      currentTest = WAIT_FOR_CLOSED;
      break;
    
    case WAIT_FOR_CLOSED:
      // When at closed move to the next part of the test cycle.
      if (servoEasing.getCurrentAngle() == positions[POS_CLOSED].getAngle()) {
        testingTimer = millis() + 2000;
        currentTest = MOVE_TO_THROWN;
      }
      break;
    
    default:
      break;
  }
}
