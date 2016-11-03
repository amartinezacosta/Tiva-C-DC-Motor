#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"

#define FREQUENCY 10000;

int main(void)
{
	volatile uint16_t period, duty_cycle;
	volatile float adjust = .1;

	//pwm configuration -----------------------------------------------------------------
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPinConfigure(GPIO_PD0_M1PWM0);

	// switch configuration --------------------------------------------------------------
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);



	period = SysCtlPWMClockGet()/FREQUENCY;			//PWM running at 625kHz
	duty_cycle = period*adjust;            	//adjust will vary at 10% duty cycle increses, testing is required

	//configure period and duty cycle -----------------------------------------------------
	PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, period);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, duty_cycle);
	PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_0);

	while(1)
	{
		if( GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x00){
			adjust += .1;
			duty_cycle = period*adjust;
			if(adjust > 1){
				adjust = 0;
			}
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, duty_cycle);
	}

	SysCtlDelay(1000000);

	}
}
