
#pragma once

#define LED_DEBUG(code) PORTE = code

#define LED_STARTUP	0x10

#define LED_BOOT		(LED_STARTUP | 0x0)
#define LED_INIT_GPIO	(LED_STARTUP | 0x1)
#define LED_INIT_MEM	(LED_STARTUP | 0x2)
#define LED_INIT_PERIPH	(LED_STARTUP | 0x8)
#define LED_INIT_INTS	(LED_STARTUP | 0x4)
#define LED_INIT_INTS	(LED_STARTUP | 0x4)


#define LED_EXC		0xF0
#define LED_NMI		(LED_EXC | 0x1)

