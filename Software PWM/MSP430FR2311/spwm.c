/*
Software PWM - FR2311
Author: Jack Pedicone
 */
#include <msp430.h>

#define light 	BIT0;
#define light2 	BIT0;
#define btn		BIT1;				

void LEDSetup();
void ButtonSetup();
void TimerA0Setup();

int count = 10;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;                   	// Disable the GPIO power-on default high-impedance mode
	LEDSetup();
	ButtonSetup();
	TimerA0Setup();

	_BIS_SR(LPM0_bits + GIE);          // Enter LPM0 w/ interrupt
}

void LEDSetup(void)
{
    P1DIR |= light;
    P2DIR |= light2;
    P2SEL &= ~light2;
    P2OUT &= ~light2;
}

void ButtonSetup(void)
{
    P1REN |= btn;
    P1OUT = btn;
    P1IE |= btn;                       // P1.3 interrupt enabled
    P1IES |= btn;                     // P1.3 Hi/lo edge
    P1IFG &= ~btn;                  // P1.3 IFG cleared

}

void TimerA0Setup(void)
{
    TA0CTL |= TASSEL_2 + MC_1; //1 MHz divided by 1, upmode
    TA0CCTL0 = CCIE; //enable compare/contrast interrupt on cctl0
    TA0CCR0 = 1000; //period of PWM
    TA0CCTL1 = CCIE; //enable compare/contrast interrupt on cctl0
    TA0CCR1 = 0; //50% duty cycle
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    if(!((P1IN & btn) == btn))
        {P2OUT ^= light2;
        if(TA0CCR1 <= 1000)
            {
                TA0CCR1 = TA0CCR1+(1000/count);
				count--;
            }
        else
            {
                TA0CCR1 = 0;
				count = 10;
            }
        }
    P1IFG &= ~btn;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER_A0(void)
{
    P1OUT |= (light);
    TA0CCTL0 &= ~light;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER_A1(void)
{
       if(TA0CCR1 != 1000)
           {
               P1OUT &= ~light;
           }
       TA0CCTL1 &= ~light;
}