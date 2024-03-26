/* -------------------------------------------------------------------------- */
/*                                                                            */
/*                                                                            */
/*                           main.h                                           */
/*                                                                            */
/*   Program to test principles of FreeRTOS                                   */
/*                                                                            */
/* -------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------- */


/* ========================================================================== */

void main(void) ;

void task_1_fn(void *pvParameters) ;

void task_2_fn(void *pvParameters) ;

void task_3_fn(void *pvParameters) ;

void task_uart_fn(void *pvParameters);

void timer_1_callback(TimerHandle_t tmr) ;

void CSM_init(void) ;

void FRAM_init(void) ;

void internal_error(uint16_t err) ;

/* ========= END ============================================================ */

