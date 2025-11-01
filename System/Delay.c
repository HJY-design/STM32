#include "Delay.h"
#include "core_cm3.h"

static uint32_t fac_us = 0;
static uint32_t fac_ms = 0;

void Delay_init()
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    fac_us = SystemCoreClock / 8000000;
    fac_ms = fac_us * 1000;
}

void Delay_ms(uint32_t nms)
{
    uint32_t temp;
    SysTick->LOAD = (uint32_t)(nms * fac_ms);
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

void Delay_us(uint32_t nus)
{
    uint32_t temp;
    SysTick->LOAD = (uint32_t)(nus * fac_us);
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}
