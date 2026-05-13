/*
 * FreeRTOSConfig.h  -  Lab 8  [FIXED for ATmega169PA 1 KB SRAM]
 *
 * Key change from original:
 *   configTOTAL_HEAP_SIZE  700  ->  500
 *
 * SRAM budget (approx):
 *   .data / .bss (globals, stack frame)  ~100 B
 *   FreeRTOS heap (tasks + queue + mutex) ~490 B  (fits in 500)
 *   AVR hardware stack (main, ISRs)       ~80 B
 *   ------------------------------------------
 *   Total                                ~670 B  < 1024 B  OK
 */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define F_CPU 8000000UL

#define configUSE_PREEMPTION            1
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define configCPU_CLOCK_HZ              ( F_CPU )
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES            4
#define configMINIMAL_STACK_SIZE        ( ( unsigned short ) 60 )

/* FIX 2: reduced from 700 to 500 to leave room for .data/.bss and HW stack */
#define configTOTAL_HEAP_SIZE           ( ( size_t ) ( 500 ) )

#define configMAX_TASK_NAME_LEN         4   /* shortened from 8 to save a few bytes */
#define configUSE_MUTEXES               1
#define configUSE_COUNTING_SEMAPHORES   1
#define configUSE_16_BIT_TICKS          1
#define configUSE_TIMERS                0

#define INCLUDE_vTaskDelay              1
#define INCLUDE_vTaskDelete             1

#endif /* FREERTOS_CONFIG_H */