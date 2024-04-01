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
#include    <string.h>

#include    "main.h"
#include    "config.h"
#include    "TA1.h"
#include    "TA3.h"
#include    "IO_port.h"

/* ========================================================================== */

#if UART_DEBUG
#define uart_dbg_str(s) uart_write_string(s)
#define uart_dbg_char(c) uart_write_char(c)
#else
#define uart_dbg_str(s)
#define uart_dbg_char(c)
#endif

/* --------- variables ------------------------------------------------------ */

#if EXAMPLE_TASKS
TaskHandle_t        task_1;             /* handle for task */
TaskHandle_t        task_2;             /* handle for task */
TaskHandle_t        task_3;             /* handle for task */


SemaphoreHandle_t   semaphore_1;        /* handle for semaphore */
SemaphoreHandle_t   semaphore_2;        /* handle for semaphore */

TimerHandle_t       timer_1;            /* handle for sw timer */
#endif

QueueHandle_t       queue_stm_vals;     /* handle for STM queue */

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
    __delay_cycles(10000000);
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
    queue_stm_vals = xQueueCreate(STM_QUEUE_SIZE, sizeof(STM_MEASURE_TYPE));
    if(NULL == queue_stm_vals) {
        internal_error(0x0071);
    }
    ret_val = xTaskCreate( task_stm160_fn, "task_stm160", 100, NULL, TASK_STM160_PRIO, NULL);
    if(pdPASS != ret_val) {
        internal_error(0x0073);
    }
    
    ret_val = xTaskCreate( task_uart_fn, "task_uart", 150, NULL, TASK_3_PRIO, NULL);
    if(pdPASS != ret_val) {
        internal_error(0x0074);
    }

    /* start the tasks and timer running. */
    //uart_dbg_str("vTaskStartScheduler\r\n");
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
/* ---------- task_stm160_fn ---- TASK ------------------------------------------- */

//https://stackoverflow.com/a/56173153
static char * ftoa(double x, char *p, int n) {
    char *s = p + n-1; // go to end of buffer
    uint16_t decimals;  // variable to store the decimals
    int units;  // variable to store the units (part to left of decimal place)
    if (x < 0) { // take care of negative numbers
        decimals = (int)(x * -100) % 100; // make 1000 for 3 decimals etc.
        units = (int)(-1 * x);
    } else { // positive numbers
        decimals = (int)(x * 100) % 100;
        units = (int)x;
    }

    *--s = (decimals % 10) + '0';
    decimals /= 10; // repeat for as many decimal places as you need
    *--s = (decimals % 10) + '0';
    *--s = '.';

    while (units > 0) {
        *--s = (units % 10) + '0';
        units /= 10;
    }
    if (x < 0) *--s = '-'; // unary minus sign for negative numbers
    return s;
}

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

/* 
    CPU freq: 8_000_000 Hz, OS clock freq: 100 Hz, STM freq: 1000 - 4000 Hz
    STM produces 10 - 40 pulses per task burst
    CPU samples 8_000 - 2_000 pulses per STM period
*/

char buf[64] = {0};
/**
 * @brief Read the duty cycle of one period of the SMT160 sensor
*/
 inline STM_MEASURE_TYPE duty() {
    int z = 0;
    int i = 0;
    STM_MEASURE_TYPE ret = (STM_MEASURE_TYPE)0;

    while( (P1IN & BIT4) == 0x10 ) ; // wait for the next period
    while( (P1IN & BIT4) == 0x00 ) z++; // reads zeros
    while( (P1IN & BIT4) == 0x10 ) i++; // reads ones
    z++; // in case both are zero
    ret = (STM_MEASURE_TYPE)(z + i);
    ret = ( i ) / ret ;
    return ret;
}

inline STM_MEASURE_TYPE avg_duty() {
    STM_MEASURE_TYPE sum = (STM_MEASURE_TYPE)0;
    int i = STM_AVERAGE_COUNT;
    //uart_dbg_str("avg_duty() in\r\n");
    for(; i ; --i) {
        sum += duty();
    }
    //uart_dbg_str("avg_duty() out\r\n");
    return sum/STM_AVERAGE_COUNT;
}

void task_stm160_fn(void *pvParameters) {
    STM_MEASURE_TYPE t = (STM_MEASURE_TYPE)0;
    while(1) {
        t = avg_duty();
        xQueueSend(queue_stm_vals, &t, portMAX_DELAY);
    }
}

#if STM_CALC_STATS
inline STM_MEASURE_TYPE sqrt(STM_MEASURE_TYPE x) {
    STM_MEASURE_TYPE xhalf = (STM_MEASURE_TYPE)(.5) * x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(STM_MEASURE_TYPE*)&i;
    x = x*( ((STM_MEASURE_TYPE)(1.5)) - xhalf*x*x);
    return 1/x;
}
struct STM160_stats {
    STM_MEASURE_TYPE std;
    STM_MEASURE_TYPE avg;
};
inline struct STM160_stats stm_stats() {
    struct STM160_stats stats = {0};
    STM_MEASURE_TYPE s1 = 0, s2 = 0;
    int i = STM_QUEUE_SIZE;
    for(; i ; --i) {
        STM_MEASURE_TYPE t = (STM_MEASURE_TYPE)0;
        xQueueReceive(queue_stm_vals, &t, portMAX_DELAY);
        t = STM_CONVERT(t);
        s1 += t;
        s2 += t*t;
    }
    stats.avg = s1/STM_QUEUE_SIZE;
    stats.std = sqrt(s2/STM_QUEUE_SIZE - (stats.avg)*(stats.avg));
    return stats;
}
#endif

void task_uart_fn(void *pvParameters) {
#if STM_CALC_STATS
    struct STM160_stats s = {0};
    while(1) {
        s = stm_stats();
        uart_write_string(ftoa(s.avg, buf, 64));
        uart_write_string(" +- ");
        uart_write_string(ftoa(s.std, buf, 64));
        uart_write_string(" C\r\n");
    }
#else
    STM_MEASURE_TYPE t = (STM_MEASURE_TYPE)0;
    while(1) {
        xQueueReceive(queue_stm_vals, &t, portMAX_DELAY);
        t = STM_CONVERT(t);
        uart_write_string(ftoa(t, buf, 64));
        uart_write_string(" C\r\n");
    }
#endif
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
