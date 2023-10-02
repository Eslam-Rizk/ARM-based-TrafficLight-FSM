// ***** Documentation Section *****
// TableTrafficLight.c
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Eslam Rizk
// October, 2, 2023

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
#define SENSOR  (*((volatile unsigned long *)0x4002401C))   		//port E bit 0 & 1 & 2
#define CAR_LIGHT   (*((volatile unsigned long *)0x400050FC))   //port B bits 0-5
#define PED_LIGHT   (*((volatile unsigned long *)0x40025028))   //port F bits 3&1

struct state{
	unsigned long PBout;  										//6bit pattern output 
	unsigned long PFout;											//pf3 & pf1 out
	unsigned long wait; 											//delay in 10ms
	unsigned long next[8];										//next state of 8 inputs 
}; typedef const struct state stype;

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
unsigned long input;   																	 //variale to store sensor data

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

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PLL_Init(void);							//enable pll to run at 80MHZ

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
		input=GPIO_PORTE_DATA_R;														//read port E inputs to input variable 
		cState=FSM[cState].next[input];											//assign next state to cState
  }
}



