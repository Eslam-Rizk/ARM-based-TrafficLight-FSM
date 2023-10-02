# Overview
A Finite State Machine (FSM) is an abstraction that describes the solution to a problem very much like an Algorithm. Unlike an algorithm which gives a sequence of steps that need to be followed to realize the solution to a problem, a FSM describes the system as a machine that changes states in reaction to inputs and produces appropriate outputs.This project implements Moore FSM to design a traffic light system.The system has 2 traffic lights (red, yellow, green) for cars at a crossroad and a pedestrian light (red, green).There are 3 sensors (2 for cars and 1 for pedestrians)
# Tools
* EK-TM4C123XL evaluaton board
* LEDs
* Pushbuttons as sensors
* Resistorsled
* Keil uVesion4
# Theory
The execution of a Moore FSM repeats this sequence over and over:
* Perform output, which depends on the current state.
* Wait a prescribed amount of time.
* Input.
* Go to next state, which depends on the input and the current state.

The TM4C123GH6PM MCU reads the 3 sensors as inputs at PE2(pedestrians), PE1(south cars) and PE0(west cars) Then outputs the cars LEDs states at PB5-0 and pedestrians LEDs at PF3(walk) and PF0(don't walk).


First, we draw what's called State Transition Graph (STG) which helps demonstrate the system states, inputs and next states:

