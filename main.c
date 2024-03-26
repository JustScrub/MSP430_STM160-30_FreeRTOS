/* -------------------------------------------------------------------------- */
/*                                                                            */
/*                                                                            */
/*                           main.c                                           */
/*                                                                            */
/*   Program to test principles of FreeRTOS                                   */
/*                                                                            */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */

#include    <msp430.h>

#include    "FreeRTOSConfig.h"
#include    "FreeRTOS.h"
#include    "task.h"
#include    "semphr.h"
#include    "timers.h"

#include    <stddef.h>
#include    <stdint.h>
#include    <stdio.h>

#include    "main.h"
#include    "config.h"
#include    "TA1.h"
#include    "TA3.h"
#include    "IO_port.h"

/* ========================================================================== */

/* --------- variables ------------------------------------------------------ */

#if EXAMPLE_TASKS
TaskHandle_t        task_1;             /* handle for task */
TaskHandle_t        task_2;             /* handle for task */
TaskHandle_t        task_3;             /* handle for task */


SemaphoreHandle_t   semaphore_1;        /* handle for semaphore */
SemaphoreHandle_t   semaphore_2;        /* handle for semaphore */

TimerHandle_t       timer_1;            /* handle for sw timer */
#endif

BaseType_t          ret_val;            /* return value for xTaskCreate */

int16_t             err_code;           /* for internal_error */

/* --------- main ----------------------------------------------------------- */

void main(void) {

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    FRAM_init();                    /* init FRAM to insert wait states */
    CSM_init();                     /* init clock system module */
    IO_port_init();                 /* init IO ports */
    TA1_init();                     /* init TA1 periodic interrupting */

    LED_R_ON;                       /* red LED on */
    __delay_cycles(30000000);
    LED_R_OFF;                      /* red LED off */

#if EXAMPLE_TASKS
#if !LED_G_INTERRUPT
    /* --- create task_1 --- */
    ret_val = xTaskCreate( task_1_fn, "task_1", MSP_STACK_SIZE, NULL, TASK_1_PRIO, &task_1);
    if(pdPASS != ret_val) {
        internal_error(0x0001);
    }
#endif

    /*--- create task_2 --- */
    ret_val = xTaskCreate( task_2_fn, "task_2", MSP_STACK_SIZE, NULL, TASK_2_PRIO, &task_2);
    if(pdPASS != ret_val) {
        internal_error(0x0002);
    }

    /*--- create task_3 --- */
    ret_val = xTaskCreate( task_3_fn, "task_3", MSP_STACK_SIZE, NULL, TASK_3_PRIO, &task_3);
    if(pdPASS != ret_val) {
        internal_error(0x0003);
    }


    /*--- create semaphore_1 --- */
    semaphore_1 = xSemaphoreCreateBinary();
    if(NULL == semaphore_1) {
        internal_error(0x0004);
    }

    /*--- create semaphore_2 --- */
    semaphore_2 = xSemaphoreCreateBinary();
    if(NULL == semaphore_2) {
        internal_error(0x0005);
    }

    /*--- create timer_1 --- */
    timer_1 = xTimerCreate( "Timer_1", TIMER_1_PERIOD, pdTRUE, 0, timer_1_callback);
    if(NULL == timer_1) {
        internal_error(0x0006);
    }

    /* start timer_1, wait max. 10 ticks if internal queue is full */
    xTimerStart(timer_1, 10);
#endif
    /*--- create task_uart --- */
    ret_val = xTaskCreate( task_uart_fn, "task_uart", MSP_STACK_SIZE, NULL, TASK_2_PRIO, NULL);
    if(pdPASS != ret_val) {
        internal_error(0x0004);
    }

    /* start the tasks and timer running. */
    vTaskStartScheduler();          /* will not return */

    /* we should not get here */
    while(1) {
    }

}
#if EXAMPLE_TASKS
/* ========================================================================== */

/* ---------- task_1_fn ---- TASK ------------------------------------------- */

/* Task.                                                                      */

void task_1_fn(void *pvParameters) {

    /* task init code */

    while(1) {
        /* task code */
        //vTaskDelay(50);     /* delay 20 ticks = 200 ms */
        //LED_G_TOGGLE;
        if((P5IN & BTN_1) == 0x00) LED_G_ON;
        if((P5IN & BTN_2) == 0x00) LED_G_OFF;
       /*if((P5IN & BTN_1) == 0x00) LED_G_ON;
        else LED_G_OFF;*/
    }
}

/* ========================================================================== */

/* ---------- task_2_fn ---- TASK ------------------------------------------- */

/* Task.                                                                      */

void task_2_fn(void *pvParameters) {

    /* task init code */

    while(1) {
        /* task code */
        xSemaphoreTake(semaphore_1, portMAX_DELAY);     /* no timeout */
        LED_R_TOGGLE;
    }
}

/* ========================================================================== */

/* ---------- task_3_fn ---- TASK ------------------------------------------- */

/* Task.                                                                      */

void task_3_fn(void *pvParameters) {

    /* task init code */

    while(1) {
        /* task code */
        xSemaphoreTake(semaphore_2, portMAX_DELAY);     /* no timeout */
        TRACE_2_TOGGLE;
    }
}
/* ========================================================================== */



/* ---------- timer_1_callback ---------------------------------------------- */

/* callback function for timer_1.                                             */

void timer_1_callback(TimerHandle_t tmr) {

    xSemaphoreGive(semaphore_2);

}
#endif
/* ---------- task_uart_fn ---- TASK ------------------------------------------- */

void uart_write_char(char c) {
    while(!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = c;
}

void uart_write_string(char *s) {
    while(*s) {
        uart_write_char(*s);
        s++;
    }
}

void task_uart_fn(void *pvParameters) {
    char i = 0, ii = 0;
    while(1) {
        ii = i;
        i = P1IN & BIT4;
        if(ii && !i) {
            uart_write_string( "\r\n" );
        }
        uart_write_char( i ? '1' : '0' );
    }
}

/* ========================================================================== */

/* ---------- vApplicationSetupTimerInterrupt ------------------------------- */

/* FreeRTOS (port) procedure to setup system timer.                           */

void vApplicationSetupTimerInterrupt(void) {

    TA3_init();                     /* init SW timer */

}

/* ========================================================================== */

/* ---------- CSM_init ------------------------------------------------------ */

/* Init clock system: ACLK on, SMCLK off, XT2 off.     Family User guide 100                       */

void CSM_init(void) {

    /* configure PJ for LFXT */
    PJSEL1 &= ~0x10;
    PJSEL0 |= 0x10;

    /* write password to enable CSCTLx writing */
    CSCTL0 = 0xA500;

    /* start LFXT */
    CSCTL4 = 0x01C0;                /* HFXT off, LFXT on, LFXT drive = max. */

    /* wait until LFXT starts */
    LED_R_ON;                       /* red LED pin */

    while((CSCTL5 & 0x0001) != 0) { /* wait until LFXTOFFG is set */
        CSCTL5 &= ~0x0001;          /* clear LFXTOFFG */
        SFRIFG1 &= ~0x0002;         /* clear OFIFG */
    }

    LED_R_OFF;                      /* RED LED off */

     __bic_SR_register(0xF0);       /* clear OSCOFF bit (= ACLK on) */

     CSCTL2 = 0x0033;               /* ACLK = LFXT, MCLK = DCO  SMCLK = DCO */
     CSCTL3 = 0x0555;               /* ACLK:32, SMCLK:32, MCLK:32 */
     CSCTL1 = 0x000C;               /* DCORSEL = 0, DCOFSEL = 6 --> 8 MHz */

     __delay_cycles(60);            /*wait until DCO stabilizes */

     CSCTL3 = 0x0000;               /* ACLK:1, SMCLK:1, MCLK:1 */

     CSCTL4 |= 0x0008;              /* turn VLO off */

}

/* ========================================================================== */

/* ---------- FRAM_init ----------------------------------------------------- */

/* Init FRAM controller to insert wait states.                                */

void FRAM_init(void) {


}

/* ========================================================================== */

/* ---------- internal_error ------------------------------------------------ */

/* Internal error indication.                                                 */

void internal_error(uint16_t err) {

    err_code = err;

    LED_R_ON;                       /* red LED on */

    while(1) {
        ;                           /* stop here */
    }

}

/* ========= END ============================================================ */
