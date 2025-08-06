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
