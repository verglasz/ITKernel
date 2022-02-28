
#pragma once

__inline__ static void enable_interrupts() {
	__asm__("ei");
	__asm__("ehb");
}

__inline__ static void disable_interrupts() {
	__asm__("di");
	__asm__("ehb");
}

