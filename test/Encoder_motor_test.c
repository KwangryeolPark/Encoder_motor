/*      기본 정보

  * Encoder_motor_test
  * 2021-02-13
  * 박광렬
  * @copyright Copyright (c) Park Kwangryeol All right reserved.


*/
/*      설명

  USART2에서 정보를 받아 Encoder를 동작시키고, 회전 속도를 제어하는 프로그램입니다.

  PID 제어를 통해 Ecoder의 속도를 제어합니다.
  
  시계방향으로 돌 경우  초록색부터 Rising
  반시계 방량으로 돌 경우 노란색부터 Rising

11 pulse / cycle * channel
30 ratio
12v 250rpm 3.5 kgcm 1A

330 pulse / cycle
(t * 10) us / pulse
tGap = t * 10

(330 * tGap) us / cycle
1 cycle / (330 * tGap) us
1,000,000 / (330 * tGap) sec
(1,000,000 * 60) / (330 * tGap) min
(60,000,000 / (330 * tGap)) cycle / min
(20,000,000 / (110 * tGap)) cycle / min
(2,000,000 / (11 * tGap)) cycle / min


y cycle / min

*/

#include "pid.h"
#include "gpio.h"
#include "exti.h"
#include "rcc.h"
#include "afio.h"
#include "tim.h"
#include "nvic.h"
#include "print.c"

void OS_CPU_RCCInit(void);
void OS_CPU_USARTInit(void);

PID enc(1.0, 1.0, 1.0);

extern "C" {
  void EXTI0_IRQHandler(void);
  void TIM2_IRQHandler(void);
}

volatile int32_t pulse = 0;
volatile int32_t strPulse = 0;
volatile uint32_t time = 0;
volatile uint32_t preTime = 0;
volatile uint8_t timeFlag = 1;
volatile int8_t direction = 0;
volatile uint16_t rpm = 0;

void EXTI0_IRQHandler(void) {
  if(EXTI -> PR & (1 << 0)) {
    EXTI -> PR |= (1 << 0);
    
    if(GPIOG -> IDR & (1 << 1)) {++pulse; ++strPulse; direction = 1;}
    else {--pulse; --strPulse; direction = -1;}
    
    rpm = (400000) / (11 * time);
    preTime = time = 0;
  }
}


void TIM2_IRQHandler(void) {
  TIM2 -> SR = 0;
  time++;
  if(strPulse == pulse) {
    if(timeFlag == 1) {
      preTime = time;
      timeFlag = 0;
    } else {
      if(time >= 4000000) time = preTime = 0;
      if(time - preTime >= 36400) {
        direction = rpm = preTime = time = 0;
      }
    }
  } else {
    timeFlag = 1;
    strPulse = pulse;
  }
}

int main(void) {
  
  OS_CPU_RCCInit();
  OS_CPU_USARTInit();
  
  RCC -> APB2ENR |= (1 << IOPGEN) | (1 << AFIOEN);
  RCC -> APB1ENR |= (1 << 0);
  
  GPIOG -> CRL |= (4 << MODE0) | (4 << MODE1);
  
  AFIO -> EXTICR1 |= (6 << 0) | (6 << 4);
  
  EXTI -> RTSR |= (1 << 0);
  EXTI -> FTSR &= ~((1 << 0));
  EXTI -> IMR |= (1 << 0);

  TIM2 -> PSC = 90 - 1; // 400,000Hz
  TIM2 -> ARR = 2 - 1;  // 5 us, 200,000Hz
  TIM2 -> CNT = 0;
  TIM2 -> SR = 0;
  
  TIM2 -> DIER = (1 << 0);
  
  TIM2 -> EGR |= (1 << 0);
  TIM2 -> CR1 |= (1 << 0);
  
  NVIC_ISER0 |= (1 << 6);
  NVIC_ISER0 |= (1 << 28);
  
  enc.setTarget(100);
  while(true) {
    enc.calPIDValue(rpm);
    putn(enc._target);
    putc('\t');
    putn(pulse);
    putc('\n');
  }
  
  
  return 0;
}

void OS_CPU_RCCInit(void) {
  RCC -> CFGR &= ~(7 << MCO);    // PLL clock / 2 is selected as MCO
  RCC -> CFGR |= (1 << USBPRE); // set USB prescaler to 1
  RCC -> CFGR |= (7 << PLLMUL);       // PLL x9
  RCC -> CFGR |= (1 << PLLXTPRE);      // HSE divided
  RCC -> CFGR |= (1 << PLLSRC); //HSE seleted as PLL input clock
  RCC -> CFGR |= (1 << ADCPRE); // prescaler to 4
  RCC -> CFGR &= ~(7 << PPRE1); // no prescaler
  RCC -> CFGR &= ~(7 << PPRE2); // no prescaler
	
  RCC -> CR |= (1 << HSEON);    // enable HSE
  while(!(RCC -> CR & (1 << HSERDY)));   // waiting for HSE ready

  RCC -> CR |= (1 << PLLON);
  while(!(RCC -> CR & (1 << PLLRDY)));  // waiting for PLL ready

  RCC -> CFGR |= (2 << SW);  
  while(((RCC -> CFGR >> SWS) & 0x03) != (2 << SW));
}

void OS_CPU_USARTInit(void) {
/*
  250,000 baud
*/
  RCC -> APB2ENR |= (1 << IOPAEN) | (1 << AFIOEN);
  GPIOA -> CRH  |= (11 << MODE9) | (4 << MODE10);
  RCC -> APB2ENR |= (1 << USART1EN);
  USART1 -> BRR = (9 << 4);     // 250000 baud
  
  USART1 -> CR1 |= (1 << UE) | (1 << TE) | (1 << RE);
}
