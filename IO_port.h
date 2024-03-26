/* ------------------------------------------------------------------ */
/*                                                                    */
/*                                                                    */
/*                         IO_port.h                                  */
/*                                                                    */
/*                 Include file for IO_port.c                         */
/*                                                                    */
/* ------------------------------------------------------------------ */

#ifndef __IO_PORT__

#define __IO_PORT__

/* ---------- --- -------------------------------------------------- */

#define LED_DIR         P1DIR
#define LED_OUT         P1OUT

#define BTN_DIR         P5DIR
#define BTN_OUT         P5OUT
#define BTN_IN          P5IN
#define BTN_REN         P5REN

#define LED_R           0x01
#define LED_G           0x02

#define BTN_1           0x40 // P5.6
#define BTN_2           0x20 // P5.5

#define MCLK_PIN        0x80
#define SMCLK_PIN       0x10

#define TRACE_1_PIN     0x01    /* P3.0 */
#define TRACE_2_PIN     0x02    /* P3.1 */
#define TRACE_OUT       P3OUT
#define TRACE_DIR       P3DIR

/* ---------- --- -------------------------------------------------- */

#define LED_R_ON        LED_OUT |= LED_R
#define LED_R_OFF       LED_OUT &= ~LED_R
#define LED_R_TOGGLE    LED_OUT ^=LED_R

#define LED_G_ON        LED_OUT |= LED_G
#define LED_G_OFF       LED_OUT &= ~LED_G
#define LED_G_TOGGLE    LED_OUT ^=LED_G

#define TRACE_1_TOGGLE   TRACE_OUT ^=TRACE_1_PIN
#define TRACE_2_TOGGLE   TRACE_OUT ^=TRACE_2_PIN

/* ---------- prototypes ------------------------------------------- */

void IO_port_init(void) ;

#endif

/* ---------- END -------------------------------------------------- */

