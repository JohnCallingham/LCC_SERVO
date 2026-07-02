# LCC_SERVO

This component is part of a suite of components which can be used as part of a program which implements an OpenLCB/LCC node. It has been developed using PlatformIO and has been tested on an Arduino Nano ESP32.

The file servo_lcc.h contains two classes. Class Servo_LCC represents one servo and class Position_LCC represents one position for the servo.

This component has two dependencies;-
- SERVO_EASING: https://github.com/JohnCallingham/SERVO_EASING.git
- LCC_NODE_COMPONENT_BASE: https://github.com/JohnCallingham/LCC_NODE_COMPONENT_BASE.git

These are automatically downloaded when using PlatformIO.

## PlatformIO installation

It can be installed in PlatformIO by adding the following line to platform.ini;-

lib_deps = https://github.com/JohnCallingham/LCC_SERVO.git

## Arduino IDE installation

It can be installed into the Arduino IDE by downloading the .zip file and adding to the Arduino library using Sketch -> Include Library -> Add .Zip Library

## Functions

The following functionality is implemented for each of the servos;-
- Three servo angles can be set using JMRI's configuration editor. The servo moves in real time as these angles are being set. The three angles will represnt the Thrown, Closed and Midpoint positions. The reason for having a Midpoint angle is to allow for frog switching when the servo moves between Thrown and Closed. Also, it allows for a 'safe' initial position before the Thrown and Closed positions have been determined.
- Associated with each of the three positions is an event. When this event is received the servo will move to the angle which has been set for that position.
- As well as explicitly moving to one of the three positions a toggle event is implemented. When this event is received the servo will move according to the following;-
    - if currently at the Thrown position, the servo will move to the Closed position.
    - if currently at the Closed position, the servo will move to the Thrown position.
    - if currently at the Midpoint position, the servo will move to the Closed position.
- When the servo starts moving away from a position a leaving event is sent for that position. When the servo reaches a position a reached event is sent for that position. When the servo moves through the Midpoint position the reached event is sent if the servo angle is increasing, and the leaving event is sent if the servo angle is decreasing.
- To cater for the situation where the LCC node is running before the LCC hub starts this software component can be commanded to send the current state of all its events when the hub is connected.
- Responds to a JMRI query with the current state of the requested event.
- A test event can be configured which, when received, causes the servo to continuously move between the Thrown and Closed positions.


