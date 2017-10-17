/*
Button debounce - FR2311
Author: Jack Pedicone
 */

#include <msp430.h>

#define light BIT0;
#define btn BIT1;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                     // Disable the GPIO power-on default high-impedance mode

    P1DIR |= light;     // configure first LED as output
    P1DIR &= ~btn;      // configure button as input
    P1REN |= btn;       // add pull up resistor
    P1OUT |= btn;

    P1IE |= btn;        // Button interrupt enabled

    TA0CCTL0 = CCIE;                       // Enable capture/compare interrupt

    __enable_interrupt();                       // Enter interrupt
    while(1)
    {

    }
}
    #pragma vector=PORT1_VECTOR
   __interrupt void PORT_1(void)
   {
       P1IE     &= ~btn;          // Disable the button interrupt
       P1OUT    ^= light;                            //Toggle the LED when the button is pressed
       //P2IFG    &= ~btn;         // Clear the button interrupt flag
       TA0CTL   |= TASSEL_2 + MC_1 + TACLR;         // Start the timer
       TA0CCR0 = 10000;
   }
    // Timer A0 interrupt service routine
   #pragma vector=TIMER0_A0_VECTOR
   __interrupt void Timer_A (void)
   {

       TA0CTL &= MC_0;      //Stop timer
       TA0R = 0;            //Clear timer
       P1IE |= btn;        //Re-enable button interrupt
       P1IFG &= ~btn;      //Clear the button interrupt flag
   }
