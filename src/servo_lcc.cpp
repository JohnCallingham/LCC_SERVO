#include "servo_lcc.h"

Position_LCC::Position_LCC(uint8_t positionNumber,
                const char *positionDescription,
                uint8_t positionAngle,
                uint16_t eventMove,
                uint16_t eventLeaving,
                uint16_t eventReached) {
  this->positionNumber = positionNumber;
  this->positionDescription = positionDescription;
  this->positionAngle = positionAngle;
  this->eventMove = eventMove;
  this->eventLeaving = eventLeaving;
  this->eventReached = eventReached;
}

Servo_LCC::Servo_LCC(uint8_t servoNumber, uint8_t pin) {
  this->servoNumber = servoNumber;
  this->pin = pin;

  servo.attach(this->pin);

  servoEasing.initialise(servoNumber, &servo);
}

void Servo_LCC::addPosition(uint8_t positionNumber,
                          const char *positionDescription,
                          uint8_t positionAngle,
                          uint16_t eventMove,
                          uint16_t eventLeaving,
                          uint16_t eventReached) {

  positions.push_back(Position_LCC (positionNumber,
                        positionDescription,
                        positionAngle,
                        eventMove,
                        eventLeaving,
                        eventReached));
}

void Servo_LCC::updatePosition(uint8_t positionNumber, uint8_t positionAngle) {
  positions[positionNumber].setAngle(positionAngle);
}

void Servo_LCC::setInitialAngles(uint8_t initialAngle) {
  servoEasing.setMidAngle(initialAngle);
  servoEasing.setInitialAngle(initialAngle);
  servoEasing.setTargetAngle(initialAngle);
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
  if (index == this->eventToggle) return true;

  for (auto & position : positions) {
    if ((index == position.getEventMove()) ||
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
   * Handle the toggle event.
   */
  if (index == eventToggle) {
    // Start the servo moving to the other position.
    Serial.printf("\nServo %d moving to the other position ", servoNumber);
    Serial.printf(" current angle = %d", servoEasing.getCurrentAngle() );

    // If the servo is at position 0 (Thrown), move to position 2 (Closed).
    if (servoEasing.getCurrentAngle() == positions[POS_THROWN].getAngle()) {
      servoEasing.setTargetAngle(positions[POS_CLOSED].getAngle());
      // Send the leaving event for this position.
      if (sendEvent) sendEvent(positions[POS_THROWN].getEventLeaving());
    }

    // If the servo is at position 1 (Mid), no movement, send mid reached event.
    if (servoEasing.getCurrentAngle() == positions[POS_MID].getAngle()) {
      // Send the reached event for this position.
      if (sendEvent) sendEvent(positions[POS_MID].getEventReached());
    }

    // If the servo is at position 2 (Closed), move to position 0 (Thrown).
    if (servoEasing.getCurrentAngle() == positions[POS_CLOSED].getAngle()) {
      servoEasing.setTargetAngle(positions[POS_THROWN].getAngle());
      // Send the leaving event for this position.
      if (sendEvent) sendEvent(positions[POS_CLOSED].getEventLeaving());
    }

    return;
  }

  /***
   * Handle the move to a position event.
   */
  // Determine the target position for this event.
  for (auto & targetPosition : positions) {
    if (index == targetPosition.getEventMove()) {
      // Check if the servo is already at this target position.
      if (servoEasing.getCurrentAngle() == targetPosition.getAngle()) {
        // Servo is already at the target position so just send the reached event.
        if (sendEvent) sendEvent(targetPosition.getEventReached());
      } else {
        // Start the servo moving to this position.
        Serial.printf("\nServo %d moving to position %d", servoNumber, targetPosition.getNumber());

        // Make the servo move to this position.
        servoEasing.setTargetAngle(targetPosition.getAngle());

        // Send the leaving event for this position.
        if (sendEvent) sendEvent(getLeavingEventForCurrentAngle());
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

void Servo_LCC::handleReachedAngle(uint8_t currentAngle, AngleDirection direction) {
  // Determine the position for the current angle.
  if (currentAngle == positions[POS_THROWN].getAngle()) {
    if (sendEvent) sendEvent(positions[POS_THROWN].getEventReached());
  } else if (currentAngle == positions[POS_MID].getAngle()) {
    // This is the mid position so send the appropriate event based on direction.
    if (direction == AngleDirection::INCREASING_ANGLE) {
      if (sendEvent) sendEvent(positions[POS_MID].getEventReached());
    } else {
      if (sendEvent) sendEvent(positions[POS_MID].getEventLeaving());
    }
  } else if (currentAngle == positions[POS_CLOSED].getAngle()) {
    if (sendEvent) sendEvent(positions[POS_CLOSED].getEventReached());
  }
}

void Servo_LCC::sendEventsForCurrentState() {
  // Handle the servo being in position 0 (Thrown).
  if (servoEasing.getCurrentAngle() == positions[POS_THROWN].getAngle()) {
    if (sendEvent) sendEvent(positions[POS_THROWN].getEventReached());
  }

  // Handle the servo being in position 2 (Closed).
  if (servoEasing.getCurrentAngle() == positions[POS_CLOSED].getAngle()) {
    if (sendEvent) sendEvent(positions[POS_CLOSED].getEventReached());
  }

  // Handle the servo being in position 1 (Mid).
  if (servoEasing.getCurrentAngle() == positions[POS_MID].getAngle()) {
    if (sendEvent) sendEvent(positions[POS_THROWN].getEventLeaving());
    if (sendEvent) sendEvent(positions[POS_CLOSED].getEventLeaving());
  }
}

void Servo_LCC::process() {
  servoEasing.update();
}
