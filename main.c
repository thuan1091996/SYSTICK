
/* Create SYSTICK INT AFTER 1MS AND COMPARE WITH GPTimer */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.c"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.c"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
double T_s=0.001; //T sample
double T_count=0;
double real_time=0;
uint32_t T_real=0,T_comp;
uint32_t T_comp1;
uint32_t Period;

// Systick result : 1.0000000000000007
// Timer result
//****************************************
/* SysTick ISR*/
void SysTick_ISR()
{
    T_real+=1;
    T_count+=T_s;
}
/* Timer ISR*/
void Timer0_ISR(void)
{
    T_comp=SysTickValueGet();
    // Clear the timer interrupt
    T_real+=1;
    T_comp1=SysTickValueGet();
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

}
int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); //80Mhz
    Period=SysCtlClockGet()*T_s;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
    IntMasterEnable();
    //SysTickIntEnable();
    //SysTickIntRegister(SysTick_ISR);
    /*Using Timer*/
    //Timer Configurations (Full-width periodic timer)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    //Enable Timer
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerLoadSet(TIMER0_BASE, TIMER_A, Period-1);
    IntEnable(INT_TIMER0A);   // Allow interrupts for timer0A
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0_ISR); //Define the ISQ
    SysTickPeriodSet(15000000-1);
    SysTickEnable();
    SysTickValueGet();
    while(1)
    {

        if (T_real==1000)
        {
            real_time=(double)(T_comp - T_comp1)*(1.0/SysCtlClockGet());
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 8);
        }

        //else if (T_real==2)
           // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
    }
}
