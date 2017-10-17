# Lab 4 - Hardware PWM
#### Jack Pedicone
##### 10-14-2017

## Library Used
msp430.h - default library

## Dependencies
* MSP430 Development boards
* Code Composer Studio
* Two LEDs and a button

## File name
* hpwm.c

## Set Requirements
Now that you have done the software version of PWM, now it is time to start leveraging the other features of these Timer Modules.

#### Tasks
* [x] Generate a PWM signal with a default duty cycle of 50%
* [x] Implement a button interupt which increases the duty cycle by 10% and controls the LED

## Compatibility
* [x] MSP430F5529
* [x] MSP430FR2311
* [x] MSP430FR6989
* [x] MSP430FR5594
* [x] MSP430G2553

### Detailed Description

* Stop watchdog timer
* For FR boards, disable the GPIO power-on default high-impedance mode
* Initialize GPIO pins for the LEDs using the LEDSetup function
* Setup the button's pullup resistor, button interrupt enable, high/low edge, and clear the interrupt flag in the ButtonSetup function.
* Initialize the timer in SMCLK, a frequency of 1MHz, divided by 1, in upmode.
* Set the capture/compare control to OUTMODE7, or reset/set mode
* Set the period of the PWM to 1000ms by setting the control/capture register to 1000, so the frequency is 10 Hz.
* Set the value of the second register to 500 for a 50% duty cycle
* The rising edge of the PWM signal occurs whenever the timer returns to 0 after the timer in upmode counts to its maximum value.
* The falling edge of the the PWM signal occurs based on the set value of the TA0CCR1 register, in this case 500.
* The result is that the PWM's edge is high for half of the period by default, or a 50% duty cycle.
* During the button interrupt, use an XOR statement to blink the first LED whenever the button is pressed
* Check if the button is being held down. If so, blink the second LED and use an if statement to determine if the value of the TA0CCR1 is less than or equal to 1000, or the value of TA0CCR0.
* If so, increase the value of TA0CCR1 by 100, or a 10% increase in duty cycle
* If not, set the value of TA0CCR1 to 0.
* Clear the button interrupt flag

#### Example code

```C
/*
Hardware PWM - FR5994
Author: Jack Pedicone
 */
 
#include <msp430.h> 

#define light 	BIT0;
#define light2	BIT1;
#define btn		BIT6;

void LEDSetup();
void ButtonSetup();
void TimerSetup();

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;                   	// Disable the GPIO power-on default high-impedance mode
    LEDSetup();
    ButtonSetup();
    TimerSetup();

    _BIS_SR(LPM0_bits + GIE);          // Enter LPM0 w/ interrupt
}


void LEDSetup(void) //set two LEDs as output
{
    P1DIR |= light;
    P1DIR |= light2;
    P1SEL &= ~light2;
    P1OUT &= ~light2;
}

void ButtonSetup(void) 
{
    P5REN |= btn;		//Enable pull up resistor
    P5OUT = btn;
    P5IE |= btn;        //Enable button interrupt               
    P5IES |= btn;		//High edge of button                     
    P5IFG &= ~btn;      //Clear button interrupt flag            

}

void TimerSetup(void) //50% PWM
{
    TA0CTL |= TASSEL_2 + MC_1; //1 MHz divided by 1
    TA0CCTL1 = OUTMOD_7;	   //Reset/Set
    TA0CCR0 = 1000;			   //Period
    TA0CCR1 = 500; 			   //50% PWM
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
	P1OUT ^= light;
    if(!((P5IN & btn) == btn)) //if button is held down
        {P1OUT ^= light2;		// light controlled by PWM toggled
        if(TA0CCR1 <= 1000)
            {
                TA0CCR1 = TA0CCR1+100;		//Increase PWM by 10%
            }
        else
            {
                TA0CCR1 = 0; //reset PWM duty cycle
			}
        }
    P1IFG &= ~btn;			//clear button interrupt flag.

}
```

