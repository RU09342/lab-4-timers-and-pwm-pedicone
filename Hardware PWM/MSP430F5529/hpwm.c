/*
Hardware PWM - F5529
Author: Jack Pedicone
 */
 
#include <msp430.h> 

#define light 	BIT0;
#define light2	BIT7;
#define btn		BIT1;

void LEDSetup();
void ButtonSetup();
void TimerSetup();

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer
    LEDSetup();
    ButtonSetup();
    TimerSetup();

    _BIS_SR(LPM0_bits + GIE);          // Enter LPM0 w/ interrupt
}


void LEDSetup(void) //set two LEDs as output
{
    P1DIR |= light;
    P4DIR |= light2;
    P4SEL &= ~light2;
    P4OUT &= ~light2;
}

void ButtonSetup(void) 
{
    P2REN |= btn;		//Enable pull up resistor
    P2OUT = btn;
    P2IE |= btn;        //Enable button interrupt               
    P2IES |= btn;		//High edge of button                     
    P2IFG &= ~btn;      //Clear button interrupt flag            

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
    if(!((P2IN & btn) == btn)) //if button is held down
        {P4OUT ^= light2;		// light controlled by PWM toggled
        if(TA0CCR1 <= 1000)
            {
                TA0CCR1 = TA0CCR1+100;		//Increase PWM by 10%
            }
        else
            {
                TA0CCR1 = 0; //reset PWM duty cycle
			}
        }
    P2IFG &= ~btn;			//clear button interrupt flag.

}






