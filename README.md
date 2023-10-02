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
Addressing input and output pins: 

```C
#define SENSOR  (*((volatile unsigned long *)0x4002401C))   		//port E bit 0 & 1 & 2
#define CAR_LIGHT   (*((volatile unsigned long *)0x400050FC))   //port B bits 0-5
#define PED_LIGHT   (*((volatile unsigned long *)0x40025028))   //port F bits 3&1
```

Assigning each index of a state to a name and declaring variables:
```C
//define states indeces
#define goW 0
#define waitW 1
#define goS 2
#define waitS 3
#define walk 4
#define on1 5
#define off1 6
#define on2 7
#define off2 8

unsigned char cState;       														 //variabe for current state
unsigned long input;

```
Defining a datatype for the FSM and listing the data from the state table to the FSM structure:
```C
struct state{
	unsigned long PBout;  										//6bit pattern output 
	unsigned long PFout;											//pf3 & pf1 out
	unsigned long wait; 											//delay in 10ms
	unsigned long next[8];										//next state of 8 inputs 
}; typedef const struct state stype;
//finite state machine of 9 states
stype FSM[9]={     
{0x0C,0x02,200,{ goW , goW ,waitW,waitW,waitW,waitW,waitW,waitW}},
{0x14,0x02,50 ,{ goS , goS , goS , goS ,walk ,walk , goS , goS }},
{0x21,0x02,200,{ goS ,waitS, goS ,waitS,waitS,waitS,waitS,waitS}},
{0x22,0x02,50 ,{walk , goW ,walk , goW ,walk ,walk ,walk ,walk }},
{0x24,0x08,200,{walk , on1 , on1 , on1 ,walk , on1 , on1 , on1 }},
{0x24,0x02,50 ,{off1 ,off1 ,off1 ,off1 ,off1 ,off1 ,off1 ,off1 }},
{0x24,0x00,50 ,{ on2 , on2 , on2 , on2 , on2 , on2 , on2 , on2 }},
{0x24,0x02,50 ,{off2 ,off2 ,off2 ,off2 ,off2 ,off2 ,off2 ,off2 }},
{0x24,0x00,50 ,{ goW , goW , goS , goW , goW , goW , goS , goW }},
};
```
Declaring functions prototypes includng PLL(Phase Lock Loop) which is used to control the clock frequency:
```C
// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PLL_Init(void);							//enable pll to run at 80MHZ
```
Initialization of PORTS B,E and F by disabling analog functions, enabling digital pins, setting pins as inputs or outputs:
```C
// ***** 3. Subroutines Section *****
//PORT B,E,F INITIALIZATION
void PORTBEF_INIT(){volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x32;                                  // 1) F B E
  delay = SYSCTL_RCGC2_R;                                // 2) no need to unlock

	GPIO_PORTE_AMSEL_R &= ~0x07;                             // 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF;                       // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;                           // 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07;                         // 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;                            // 7) enable digital on PE2-0
  
	GPIO_PORTB_AMSEL_R &= ~0x3F;                           // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF;                     // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;                          // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F;                     // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;                        // 7) enable digital on PB5-0
	
	GPIO_PORTF_LOCK_R = 0x4C4F434B;                     // 8) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x0A;                             // allow changes to PF3 and PF1
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R &= ~0x0A; 			                  // 9) disable analog function on PF3 abd PF1
  GPIO_PORTF_PCTL_R = 0x00000000;                     // 10) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R |= 0x0A;                              // 11) PF3 and PF1 outputs
  GPIO_PORTF_AFSEL_R &= ~0x0A; 		                  	// 12) regular function on PF3 and PF1
  GPIO_PORTF_DEN_R |= 0x0A;    		                	// 13) enable digital on PF3 and PF1
}
```
Initializing the systick timer and a function to take parameters as multiples of 10ms.At 80 MHZ, each cycle takes 12.5 ns, so 10 ms would take 800000 cycles.Once the timer counter gos from 1 to 0 the CTRL flag at bit 16 is set declaring that 10 ms have passed:
```C
//initialize systick timer
void SysTick_Init(){
	NVIC_ST_CTRL_R=0;                           //disable systick dyring setup
	NVIC_ST_CTRL_R=0x00000005;	              	//enable systick with core clock
}

//10ms timer
void delay_ms(unsigned long time){
	unsigned long i;
	for(i=0;i<time;i++){
		NVIC_ST_RELOAD_R=799999;										//reload register with 800000*12.5ns=10ms
		NVIC_ST_CURRENT_R=0;												//any value to current register to start ticking
		while((NVIC_ST_CTRL_R & 0x00010000)==0){}		//wait for bit 16 flag each 10ms
	}
}
```
In the main function, we call defned functions to initialize.In the main loop (while(1)), we apply the steps of moore FSM:
* Outputing the current state.
* Delay of current state
* Readng inputs
* Getting the next state
```C
//10ms timer
void delay_ms(unsigned long time){
	unsigned long i;
	for(i=0;i<time;i++){
		NVIC_ST_RELOAD_R=799999;										//reload register with 800000*12.5ns=10ms
		NVIC_ST_CURRENT_R=0;												//any value to current register to start ticking
		while((NVIC_ST_CTRL_R & 0x00010000)==0){}		//wait for bit 16 flag each 10ms
	}
}
int main(void){
  PLL_Init();																						//PLL to rn at 80MHZ
  SysTick_Init();																				//initialize systic timer
  EnableInterrupts();
  PORTBEF_INIT();																				//initialize port b,e,f
  cState=goW;																						//initialize cState with initial state
  while(1){
		CAR_LIGHT = FSM[cState].PBout;							//output cars leds to port B
		PED_LIGHT = FSM[cState].PFout;							//output pedestrian leds to port F
		delay_ms(FSM[cState].wait);													// delay of current state
		input=SENSOR;														//read port E inputs to input variable 
		cState=FSM[cState].next[input];											//assign next state to cState
  }
}
```
[TrafficLight_FSM.webm](https://github.com/Eslam-Rizk/ARM-based-TrafficLight-FSM/assets/70345162/496aaa9b-3408-4556-b02f-9d8dc093a900)
