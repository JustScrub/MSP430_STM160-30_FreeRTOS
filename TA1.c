/* ------------------------------------------------------------------ */
/*                                                                    */
/*                                                                    */
/*                            TA1.c                                   */
/*                                                                    */
/*                    TA1 control functions                           */
/*                    TA1 is used like beep generator                 */
/*                                                                    */
/* ------------------------------------------------------------------ */


/* ---------- --- -------------------------------------------------- */


#include    <msp430.h>

#include    "FreeRTOSConfig.h"
#include    "FreeRTOS.h"
#include    "semphr.h"

#include    <stddef.h>
#include    <stdint.h>
#include    <stdio.h>

#include    "config.h"
#include    "TA1.h"
#include    "TA3.h"
#include    "IO_port.h"

/* ---------- --- -------------------------------------------------- */


/* ---------- variables -------------------------------------------- */
#if EXAMPLE_TASKS
extern SemaphoreHandle_t  semaphore_1;  /* handle for semaphore */
#endif
BaseType_t  ta1_higher_woken;           /* flag for system scheduler */

/* ---------- TA1_init --------------------------------------------- */


void TA1_init(void) {

    TA1R = 0x0000;
    TA1CCR0 = TA1_INT_PERIOD;   /* = 10 ms @ 8 MHz + :8 prescaler :8 IDEX */
    TA1EX0 = 0x0007;            /* :8 in IDEX prescaler */
    TA1CTL = 0x02D4;            /* SMCLK:8, up mode to CCR0, TAIE disable */
    #if EXAMPLE_TASKS
    TA1CCTL0 = 0x0010;          /* compare interrupt enabled */
    #endif

}

/* ---------- TA1_TA1CCR0_isr ------------------------------------- */

/* Procedure to service TA1 TA1CCR0 interrupt.                      */

#pragma vector=TIMER1_A0_VECTOR         /* vector 0xFFE2 */
__interrupt void TA1_TA1CCR0_isr(void) {

    ta1_higher_woken = pdFALSE;
#if EXAMPLE_TASKS
    xSemaphoreGiveFromISR(semaphore_1, &ta1_higher_woken);
#endif
    /* clear LPM bits in stacked SR */
    __bic_SR_register_on_exit( SCG1 + SCG0 + OSCOFF + CPUOFF );

    /* THIS MUST BE THE LAST THING DONE IN THE ISR. */
    portYIELD_FROM_ISR(ta1_higher_woken);

}

/* ---------- END -------------------------------------------------- */

