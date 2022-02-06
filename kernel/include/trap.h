#pragma once
void trap_init();
void trap_start();

#define TIMER_CLK_RATE (390000000 / 1000)  // timer interrupt interval

// Interrupt Exception Code
#define S_TIMER_INT 5
// non-Interrupt Exception Code
#define ILLEGAL_INSTRUCTION 2
#define SYSCALL_FROM_U_MODE 8