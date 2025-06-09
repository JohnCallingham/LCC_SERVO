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

void Servo_LCC::setInitialAngles(uint8_t initialAngle) {
  servoEasing.setMidAngle(initialAngle);
  servoEasing.setInitialAngle(initialAngle);
  servoEasing.setTargetAngle(initialAngle);
}

void Servo_LCC::print() {
  Serial.printf("\nServo pin=%d", this->pin);

  for (auto & position : positions) {
    Serial.printf("\n Position number=%d", position.getPositionNumber());
    Serial.printf("\n Position description=%s", position.getPositionDescription());
    Serial.printf("\n Position angle=%d", position.getPositionAngle());
    Serial.printf("\n Position event move=%d", position.getEventMove());
    Serial.printf("\n Position event leaving=%d", position.getEventLeaving());
    Serial.printf("\n Position event reached=%d", position.getEventReached());
  }

  Serial.printf("\neventToggle=%d", this->eventToggle);
}

bool Servo_LCC::eventIndexMatchesThisServo(uint16_t index) {
  //Serial.printf("\nin eventIndexMatchesThisServo() for index=0x%02X", index);
  if (index == this->eventToggle) return true;

  for (auto & position : positions) {
    if ((index == position.getEventMove()) ||
        (index == position.getEventLeaving()) ||
        (index == position.getEventReached())) return true;
  }

  return false;
}

bool Servo_LCC::eventIndexMatchesCurrentState(uint16_t index) {
  Serial.printf("\nin eventIndexMatchesCurrentState() for index 0x%02X", index);

  /***
   * TO DO: complete from here !!!
   */

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
    if (servoEasing.getCurrentAngle() == positions[0].getPositionAngle()) {
      servoEasing.setTargetAngle(positions[2].getPositionAngle());
      // Send the leaving event for this position.
      if (sendEvent) sendEvent(positions[0].getEventLeaving());
    }

    // If the servo is at position 1 (Mid), no movement, send mid reached event.
    if (servoEasing.getCurrentAngle() == positions[1].getPositionAngle()) {
      // Send the reached event for this position.
      if (sendEvent) sendEvent(positions[1].getEventReached());
    }

    // If the servo is at position 2 (Closed), move to position 0 (Thrown).
    if (servoEasing.getCurrentAngle() == positions[2].getPositionAngle()) {
      servoEasing.setTargetAngle(positions[0].getPositionAngle());
      // Send the leaving event for this position.
      if (sendEvent) sendEvent(positions[2].getEventLeaving());
    }

    return;
  }

  /***
   * Handle the move to a position event.
   */
  // Determine the position for this event.
  for (auto & targetPosition : positions) {
    if (index == targetPosition.getEventMove()) {
      // Check if the servo is already at this target position.
      if (servoEasing.getCurrentAngle() == targetPosition.getPositionAngle()) {
        // Servo is already at the target position so just send the reached event.
        if (sendEvent) sendEvent(targetPosition.getEventReached());
      } else {
        // Start the servo moving to this position.
        Serial.printf("\nServo %d moving to position %d", servoNumber, targetPosition.getPositionNumber());

        // Make the servo move to this position.
        servoEasing.setTargetAngle(targetPosition.getPositionAngle());

        // Send the leaving event for this position.
        if (sendEvent) sendEvent(getLeavingEventForCurrentAngle());
      }
    }
  }
}

uint16_t Servo_LCC::getLeavingEventForCurrentAngle() {
  // Find the position object for the current angle.
  for (auto & position : positions) {
    if (position.getPositionAngle() == servoEasing.getCurrentAngle()) {
      // Return the leaving event for this position.
      return position.getEventLeaving();
    }
  }

  return 0; // To keep the compiler happy!
}

void Servo_LCC::handleReachedAngle(uint8_t currentAngle, AngleDirection direction) {
  uint16_t eventToSend;

  // Determine the position for the current angle.
  if (currentAngle == positions[0].getPositionAngle()) {
    eventToSend = positions[0].getEventReached();
  } else if (currentAngle == positions[1].getPositionAngle()) {
    // This is the mid position so send the appropriate event based on direction.
    if (direction == AngleDirection::INCREASING_ANGLE) {
      eventToSend = positions[1].getEventReached();
    } else {
      eventToSend = positions[1].getEventLeaving();
    }
  } else if (currentAngle == positions[2].getPositionAngle()) {
    eventToSend = positions[2].getEventReached();
  }

  if (sendEvent) sendEvent(eventToSend);
}

void Servo_LCC::process() {
  servoEasing.update();
}
