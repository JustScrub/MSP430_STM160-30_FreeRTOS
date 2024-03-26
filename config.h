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

/* ========================================================================== */

/* task stack size */
#define MSP_STACK_SIZE          30      /* stack size for all tasks (in 16bit words) */

/* task priority */
#define TASK_1_PRIO             1
#define TASK_2_PRIO             2
#define TASK_3_PRIO             3

#define TA3_INT_PERIOD          10000   /* = 10 ms @ 8 MHz + :8 prescaler */
#define TA1_INT_PERIOD          15625   /* = 125 ms @ 8 MHz + :8 :8 prescalers */

#define TIMER_1_PERIOD          200     /* 200 ticks = 2 sec. */
/* ========= END ============================================================ */

