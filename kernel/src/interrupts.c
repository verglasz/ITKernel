#include "types.h"
#include "syscalls.h"

void interrupt_handler(usize raw_irq_number) {
    return;
}

void exception_handler(usize raw_cause) {
    return;
}

void syscall_handler(usize arg1, usize arg2, usize arg3, usize arg4) {
	switch (arg1) {
		case SYSCALL_EXIT:
			sys_exit(arg2);
			break;
		case SYSCALL_GETSWS:
			sys_getsws(arg2);
			break;
		case SYSCALL_GETBTNS:
			sys_getbtns(arg2);
			break;
		case SYSCALL_SCREEN_PRINT:
			sys_screen_print(arg1, arg2, arg3);
			break;
		case SYSCALL_SERIAL_WRITE:
			sys_serial_write(arg1, arg2);
			break;
		default:
			break;
	}
    return;
}

