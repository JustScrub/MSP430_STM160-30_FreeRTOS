/* -------------------------------------------------------------------------- */
/*                                                                            */
/*                                                                            */
/*                           config.h                                         */
/*                                                                            */
/*                   Basic program configuration                              */
/*                                                                            */
/* -------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------- */

#define EXAMPLE_TASKS 0
#define LED_G_INTERRUPT 1
#define UART_DEBUG 0

/* ========================================================================== */
// STM160 measuring configuration
#define STM_AVERAGE_COUNT           ( 10*8 )/* number of measurements to average, should be multiple of 8 */
#define STM_MEASURE_TYPE            double /* type of measurement */
#define STM_DC2TEMP(dc)            ( ((STM_MEASURE_TYPE)(212.77)) * (dc) - ((STM_MEASURE_TYPE)(68.085)) ) /* convert STM160 output to temperature */
#define STM_DC2TEMP_2(dc)          ( ((STM_MEASURE_TYPE)(-1.43)) * ( (dc)*(dc) ) + ((STM_MEASURE_TYPE)(214.56)) * (dc) - ((STM_MEASURE_TYPE)(68.6)) )
#define STM_CONVERT               STM_DC2TEMP_2  /* select conversion function */
#define STM_CALC_STATS            0          /* calculate statistics */
#define STM_QUEUE_SIZE            10          /* size of the queue for STM160 measurements */

/* ========================================================================== */
/* task stack size */
#define MSP_STACK_SIZE          30      /* stack size for all tasks (in 16bit words) */

/* task priority */
#define TASK_1_PRIO             1
#define TASK_2_PRIO             2
#define TASK_3_PRIO             3
#define TASK_STM160_PRIO        4

#define TA3_INT_PERIOD          10000   /* = 10 ms @ 8 MHz + :8 prescaler */
#define TA1_INT_PERIOD          15625   /* = 125 ms @ 8 MHz + :8 :8 prescalers */

#define TIMER_1_PERIOD          200     /* 200 ticks = 2 sec. */
/* ========= END ============================================================ */

