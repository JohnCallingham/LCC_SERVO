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

  servoEasing.setMidAngle(positions[1].getPositionAngle()); // Set mid angle to position 1 angle.
  servoEasing.initialise(&servo, positions[1].getPositionAngle()); // Initial angle set to position 1 angle.


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
  if (index == this->eventToggle) return true;

  for (auto & position : positions) {
    if (index == position.getEventMove()) return true;
  }

  return false;
}

void Servo_LCC::eventReceived(uint16_t index) {
  // Is this the toggle event for this servo?
  if (index == eventToggle) {
    // Start the servo moving to the other position.
    //Serial.printf("\nServo %d moving to the other pos ", servoNumber);

    //Serial.printf(" current angle = %d", servoEasing.getCurrentAngle() );

    // // If the servo is at position 0, move to position 2.
    // if (servoEasing.getCurrentAngle() == positions[0].getPositionAngle()) {
    //   servoEasing.setTargetAngle(positions[2].getPositionAngle());
    //   return;
    // }

    // // If the servo is at position 1, move to position 2.
    // if (servoEasing.getCurrentAngle() == positions[1].getPositionAngle()) {
    //   servoEasing.setTargetAngle(positions[2].getPositionAngle());
    //   return;
    // }

    // // If the servo is at position 2, move to position 0.
    // if (servoEasing.getCurrentAngle() == positions[2].getPositionAngle()) {
    //   servoEasing.setTargetAngle(positions[0].getPositionAngle());
    //   return;
    // }
  }

  // Determine the position for this event.
  for (auto & position : positions) {
    if (index == position.getEventMove()) {
      // Start the servo moving to this position.
      Serial.printf("\nServo %d moving to position %d", servoNumber, position.getPositionNumber());

      // Make the servo move to this position.
      servoEasing.setTargetAngle(position.getPositionAngle());
    }
  }
}

void Servo_LCC::process() {
  servoEasing.update();
}
