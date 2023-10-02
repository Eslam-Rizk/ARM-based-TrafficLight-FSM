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

First, we draw what's called State Transition Graph (STG) which helps demonstrate the system states, outputs, delay, inputs and next states:
<img src="https://github.com/Eslam-Rizk/ARM-based-TrafficLight-FSM/blob/main/Moore%20State.jpg" width=600>

then we use this data to from a state table which is used to make our data structre.
# What does this system do?
## Inputs:
* Pedestrians sensor at PE2.
* South cars sensor at PE1.
* West cars sensor at PE0.
## Outputs:
* West red, yellow and green LEDs at PB5, PB4 and PB3 respectively.
* South red, yellow and green LEDs at PB2, PB1 and PB0 respectively.
* Pedestrians red and green LEDs at PF1 and OF3 respectively.
## States:
There should be 6 states to the system, goW , waitW, goS, waitS, walk and waitWalk but we don't have a yellow LED for the waitWalk so we flash the pedestrian red LED 2 times instead.So, The final states are goW , waitW, goS, waitS, walk, on1, off1, on2, and off2.The wait states have 500ms delay and go & walk states have 2000ms delay.
## State Transition Graph:
<img src="https://github.com/Eslam-Rizk/ARM-based-TrafficLight-FSM/blob/main/state%20transition%20graph.png" width=600>

## State Table:
After we fill the state table, we use this data to make our structure which is of struct data type:
<img src="https://github.com/Eslam-Rizk/ARM-based-TrafficLight-FSM/blob/main/state%20table.png" width=600>

# Code


