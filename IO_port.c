/* ------------------------------------------------------------------ */
/*                                                                    */
/*                                                                    */
/*                            IO_port.c                               */
/*                                                                    */
/*                    Functions for IO port manipulation              */
/*                                                                    */
/* ------------------------------------------------------------------ */


/* ---------- --- -------------------------------------------------- */

#include    <msp430.h>

#include    "FreeRTOSConfig.h"
#include    "FreeRTOS.h"

#include    <stddef.h>
#include    <stdint.h>
#include    <stdio.h>

#include    "config.h"
#include    "TA1.h"
#include    "TA3.h"
#include    "IO_port.h"

/* ---------- --- -------------------------------------------------- */


/* ---------- variables -------------------------------------------- */

/* ---------- IO_port_init ----------------------------------------- */

void IO_port_init(void) {

    PM5CTL0 &= ~LOCKLPM5;           /* Disable the GPIO power-on default high-impedance mode */
                                    /* to activate previously configured port settings */
    /* init LED port */
    LED_DIR |= (LED_R | LED_G);     /* P1.0, P1.1 = out (LEDs) */
    LED_R_OFF;                      /*  */
    LED_G_OFF;                      /*  */

    /* init buttons port */
    BTN_DIR &= ~(BTN_1 | BTN_2);	/* input (buttons) */
    P5OUT |= (BTN_1 | BTN_2);		/* pullup */
    P5REN |= (BTN_1 | BTN_2);		/* pullup enable */
#if LED_G_INTERRUPT
    P5IE |= (BTN_1 | BTN_2);		 //interrupt enable
    P5IES |= (BTN_1 | BTN_2);		 //high-to-low transition
    P5IFG &= ~(BTN_1 | BTN_2);		 //clear interrupt flag
#endif

    /* init SMT160 port */
    P1DIR &= ~BIT4;                  /* input (SMT160) */
    P1OUT |= BIT4;                   /* pullup */
    P1REN |= BIT4;                   /* pullup enable */

    /* enable eUSCI_A0 (backchannel) UART Family User Guide 767 */
    // https://e2e.ti.com/support/microcontrollers/msp-low-power-microcontrollers-group/msp430/f/msp-low-power-microcontroller-forum/693911/msp-exp430fr5994-cannot-get-application-backchannel-uart-to-work
    P2SEL1 |= (BIT0 | BIT1); // P2.0 = TXD, P2.1 = RXD
    P2SEL0 &= ~(BIT0 | BIT1);
    UCA0CTLW0 = UCSWRST & ~UCSYNC;  // Put eUSCI in reset & set as UART mode
    UCA0CTLW0 |= UCSSEL__SMCLK;     // CLK = SMCLK
    // Baud Rate from datasheet table
    // fBRCKL = 8000000
    // Baud Rate = 115200
    // UCOS16 = 1, UCBRx = 4, UCxBRFx = 5, UCBRSx = 0x55
    UCA0BRW = 4;
    UCA0MCTLW |= UCOS16 | UCBRF_5 | 0x5500;
    UCA0CTLW0 &= ~UCSWRST;                  // Initialize eUSCI

    /* output MCLK and SMCLK */
    P5DIR |= MCLK_PIN;
    P5SEL0 |= MCLK_PIN;
    P5SEL1 |= MCLK_PIN;

    P3DIR |= SMCLK_PIN;
    P3SEL1 |= SMCLK_PIN;

    TRACE_DIR |= TRACE_1_PIN ;
    TRACE_DIR |= TRACE_2_PIN ;

}
#if LED_G_INTERRUPT
#pragma vector=PORT5_VECTOR
__interrupt void p5vect(void) {
    if (P5IFG & BTN_1) {
        P5IFG &= ~BTN_1;
        LED_G_ON;
    }
    if (P5IFG & BTN_2) {
        P5IFG &= ~BTN_2;
        LED_G_OFF;
    }
 }
#endif

/* ---------- END -------------------------------------------------- */

