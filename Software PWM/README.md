# Lab 4 - Software PWM
#### Jack Pedicone
##### 10-14-2017

## Library Used
msp430.h - default library

## Dependencies
* MSP430 Development boards
* Code Composer Studio
* Two LEDs and a button

## File name
* spwm.c

## Set Requirements
You need to generate a 1kHz PWM signal with a duty cycle between 0% and 100%. Upon the processor starting up, you should PWM one of the on-board LEDs at a 50% duty cycle. Upon pressing one of the on-board buttons, the duty cycle of the LED should increase by 10%. Once you have reached 100%, your duty cycle should go back to 0% on the next button press. You also need to implement the other LED to light up when the Duty Cycle button is depressed and turns back off when it is let go. This is to help you figure out if the button has triggered multiple interrupts.

#### Tasks
* [x] Generate a PWM signal with a default duty cycle of 0%
* [x] Implement a button interupt which increases the duty cycle by 10%
* [x] Implement a timer interrupts which controls the LED
* [x] Extra Work - Linear brightness


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
* Initialize the timer in SMCLK, a frequency of 1MHz, divided by 1, in upmode. It is in compare mode by deault since CAP = 0.
* Enable compare/capture interrupts on the first timer register using CCIE.
* Set the period of the PWM to 1000ms by setting the control/capture register to 1000, so the frequency is 10 Hz.
* Enable compare/capture interrupts on the second timer register using CCIE.
* Set the value of the second timer's cc register to 0 for a 0% duty cycle
* The PWM's duty cycle is based on the comparison between these two registers. Effectively TA0CCR1 / TA0CCR0. By default that means it's 0/1000, or a duty cycle of 0. 
* Blink the second LED on, and check whether the button is being held down during the button interrupt
* If so, use an if statement to determine if the value of the TA0CCR1 is less than or equal to 1000, or the value of TA0CCR0.
* If so, increase the value of TA0CCR1 linearly and decrement count (linear brightness explained under Extra Work).
* If not, set the value of TA0CCR1 to 0 and count to 10.
* Clear the button interrupt flag
* In the timer interrupt, keep the first LED on by setting PxOUT |= with the corresponding bit. 
* Clear the compare/capture flag

#### Example code from FR6989

```C
void TimerA0Setup(void)
{
    TA0CTL |= TASSEL_2 + MC_1; //1 MHz divided by 1, upmode
    TA0CCTL0 = CCIE; //enable compare/contrast interrupt on cctl0
    TA0CCR0 = 1000; //period of PWM
    TA0CCTL1 = CCIE; //enable compare/contrast interrupt on cctl0
    TA0CCR1 = 0; //0% duty cycle
}
```

### Extra Work - Linear Brightness

* A global integer, count, was initialized outside of main and is set to 10 initially.
* Whenever the button is pressed, the duty cycle of the PWM is increased by the value of 1000 divided by count.
* Count is then decremented
* When the duty cycle of the PWM returns to 0, the value of count returns to 10.
* This means that each increase in the brightness is larger with each one, giving the LED the appearance of linearly increasing brightness.

```Example Code
#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void)
{
    if(!((P1IN & btn) == btn))
        {P1OUT ^= light2;
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
```
