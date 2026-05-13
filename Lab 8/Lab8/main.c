/*
 * main.c  -  Lab 8 : Temperature-Based Motor Control with FreeRTOS  [FIXED]
 *
 * Target  : ATmega169PA @ 8 MHz  (1 KB SRAM total)
 * IDE     : Microchip Studio
 *

 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/pgmspace.h>    

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "uart.h"
#include "lm35.h"
#include "button.h"
#include "motor.h"

/* ---------- Motor state --------------------------------------------------- */
#define MOTOR_STOP    0
#define MOTOR_FORWARD 1

/* ---------- FreeRTOS objects ---------------------------------------------- */
static QueueHandle_t     xMotorQueue;   /* uint8_t motor state              */
static SemaphoreHandle_t xUARTMutex;   

/* buttonOverride: written only by the highest-priority task (vButtonTask).
   Single-byte R/W is atomic on AVR - no extra protection needed.            */
static volatile uint8_t buttonOverride = 0;


static void uart_send_P(const char *str_P)
{
    char c;
    while ((c = (char)pgm_read_byte(str_P++)))
        UART_sendChar(c);
}

void vButtonTask(void *pv)
{
    uint8_t prevState = BUTTON_read();

    for (;;)
    {
        uint8_t btnNow = BUTTON_read();

        if (btnNow != prevState)         /* edge detected   */
        {
            

            if (btnNow == 1)             /* button OPEN ? pull-up active      */
            {
                buttonOverride = 1;
                uint8_t state  = MOTOR_STOP;
                xQueueSend(xMotorQueue, &state, 0);

                xSemaphoreTake(xUARTMutex, portMAX_DELAY);
                uart_send_P(PSTR("[Button] Opened ->OVRRide ON :[Motor State] : STOP\r\n"));
                xSemaphoreGive(xUARTMutex);
            }
            else                         /* button PRESS ? pin pulled to GND  */
            {
                buttonOverride = 0;

                xSemaphoreTake(xUARTMutex, portMAX_DELAY);
                uart_send_P(PSTR("[Button] Pressed ->OVRRIDE OFF:temperatur Control \r\n"));
                xSemaphoreGive(xUARTMutex);
            }
            prevState = btnNow;
        }

        vTaskDelay(pdMS_TO_TICKS(50));   
    }
}

void vTempTask(void *pv)
{
    for (;;)
    {
        uint8_t temp  = LM35_read();
        uint8_t state = (temp > TEMP_THRESHOLD) ? MOTOR_FORWARD : MOTOR_STOP;

        xSemaphoreTake(xUARTMutex, portMAX_DELAY);
        uart_send_P(PSTR("[TMP] T="));
        uart_print_number(temp);
        uart_send_P(PSTR("C "));
        uart_send_P((state == MOTOR_FORWARD) ? PSTR("FWD") : PSTR("STOP"));
        if (buttonOverride) uart_send_P(PSTR(" [OVR]"));
        uart_send_P(PSTR("\r\n"));
        xSemaphoreGive(xUARTMutex);

        if (!buttonOverride)
            xQueueSend(xMotorQueue, &state, 0);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vMotorTask(void *pv)
{
    uint8_t state;

    for (;;)
    {
        if (xQueueReceive(xMotorQueue, &state, portMAX_DELAY) == pdTRUE)
        {
            if (state == MOTOR_FORWARD)
            {
                motor_forward();
                xSemaphoreTake(xUARTMutex, portMAX_DELAY);
                uart_send_P(PSTR("[MTR] FORWARD\r\n"));
                xSemaphoreGive(xUARTMutex);
            }
            else
            {
                motor_stop();
                xSemaphoreTake(xUARTMutex, portMAX_DELAY);
                uart_send_P(PSTR("[MTR] STOPPED\r\n"));
                xSemaphoreGive(xUARTMutex);
            }
        }
    }
}

int main(void)
{
    UART_init(9600);
    LM35_init();
    BUTTON_init();
    motor_init();

    uart_send_P(PSTR("===Lab8 FreeRTOS===\r\n"));

    xMotorQueue = xQueueCreate(1, sizeof(uint8_t));
    xUARTMutex  = xSemaphoreCreateMutex();

    if (xMotorQueue == NULL || xUARTMutex == NULL)
    {
        uart_send_P(PSTR("Error :RTOS\r\n"));
        while (1);
    }

 
    xTaskCreate(vButtonTask, "BTN", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(vTempTask,   "TMP", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(vMotorTask,  "MTR", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();
    while (1);
}