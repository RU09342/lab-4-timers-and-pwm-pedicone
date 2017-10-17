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






