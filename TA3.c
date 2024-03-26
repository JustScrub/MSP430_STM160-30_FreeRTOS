/* ------------------------------------------------------------------ */
/*                                                                    */
/*                                                                    */
/*                            TA3.c                                   */
/*                                                                    */
/*                    TA3 control functions                           */
/*                    TA3 is used for SW timers                       */
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

/* ---------- TA3_init ------------------------------------------- */

/* Init & start TA3.                                               */

void TA3_init(void) {

    TA3R = 0x0000;
    TA3CCR0 = TA3_INT_PERIOD;   /* = 10 ms @ 8 MHz + :8 prescaler */
    TA3CTL = 0x02D4;            /* SMCLK:8, up mode to CCR0, TAIE disable */
    TA3CCTL0 = 0x0010;          /* compare interrupt enabled */

}

/* ---------- TA3_TA1CCR0_isr ------------------------------------- */

/* Procedure to service TA3 TA3CCR0 interrupt.                      */
/* For FreeRTOS the ISR is in port.c, targeted by configTICK_VECTOR */
/* in FreeRTOSConfig.h                                              */

//#pragma vector=TIMER3_A0_VECTOR         /* vector 0xFFD6 */
//__interrupt void TA3_TA1CCR0_isr(void) {

        /* clear LPM bits in stacked SR */
//      __bic_SR_register_on_exit(unsigned short mask); /* BIC mask, saved_SR */
//        __low_power_mode_off_on_exit();                 /* BIC.W #0xF0, saved_SR */

//}

/* ---------- END -------------------------------------------------- */

