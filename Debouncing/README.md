# Lab 4 - Button Debounce
#### Jack Pedicone
##### 10-14-2017

## Library Used
msp430.h - default library

## Dependencies
* MSP430 Development boards
* Code Composer Studio
* One LED and a button

## File name
* debounce.c

## Set Requirements
You need to utilize the TIMER modules within the MSP430 processors to implement a debounced switch to control the state of an LED. You most likely will want to hook up your buttons on the development boards to an oscilloscope to see how much time it takes for the buttons to settle. The idea here is that your processor should be able to run other code, while relying on timers and interrupts to manage the debouncing in the background. You should not be using polling techniques for this assignment. Your code should also be able to detect

#### Tasks
* [x] Create a button interrupt which blinks the LED and starts the timer to create a delay
* [x] Implement a timer interrupt which stops/clears the timer, and re-enables the button interrupt


## Compatibility
* [x] MSP430F5529
* [x] MSP430FR2311
* [x] MSP430FR6989
* [x] MSP430FR5594
* [x] MSP430G2553

### Detailed Description

* Stop watchdog timer
* For FR boards, disable the GPIO power-on default high-impedance mode
* Initialize GPIO pins for the LED and the button
* Enable button interrupt, and CCIE
* Enable general interrupts and use a while loop to continually run the program
* Set up a button interrupt which disables button interrupts, blinks an LED, and starts the timer in ACLK upmode. Also sets the compare/control register to 10000, which results in a time of 10ms.
* The timer then counts to this value and interrupts, where the timer is once again stopped using Mode Control 0 (MC_0).
* The timer clears by setting the timer register TA0R to 0
* Button interrupt is enabled again, and the button interrupt flag is cleared.
* The count to 10000 for the timer to interrupt creates a debouncing effect on the button, as there is 10ms where the button cannot be interrupted again until the timer interrupts.


#### Example code from FR6989

```C
/*
Button debounce - G2553
Author: Jack Pedicone
 */

#include <msp430.h>

#define light BIT0;
#define btn BIT3;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer
    //PM5CTL0 &= ~LOCKLPM5;                     // Disable the GPIO power-on default high-impedance mode

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
       P1OUT    ^= light;         //Toggle the LED when the button is pressed
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
```

