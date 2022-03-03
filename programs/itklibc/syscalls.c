

#include "itklibc.h"

#define SYSCALL_EXIT 			1
#define SYSCALL_GETBTNS 		2
#define SYSCALL_GETSWS 			3
#define SYSCALL_SCREEN_PRINT 	4
#define SYSCALL_SERIAL_WRITE 	5
#define SYSCALL_SCREEN_CLEAR    6

__attribute__((noreturn))
void exit(usize status) {
	syscall(SYSCALL_EXIT, status);
	// __builtin_unreachable(); // should we have this?
}

u8 getbtns() {
	u8 result;
	syscall(SYSCALL_GETBTNS, &result);
	return result;
}

u8 getsws() {
	u8 result;
	syscall(SYSCALL_GETSWS, &result);
	return result;
}

void screen_print(usize line, char *str, usize flags) {
	syscall(SYSCALL_SCREEN_PRINT, line, str, flags);
}

void serial_write(u8 *buffer, usize len) {
	syscall(SYSCALL_SERIAL_WRITE, buffer, len);
}

void screen_clear(usize flags) {
	syscall(SYSCALL_SCREEN_CLEAR, flags);
}

