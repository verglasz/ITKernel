#include "itklibc.h"

int main(void) {
    char text1[] = "This was printed with too many characters!";
    char text2[] = "ABC123";

    screen_print(0, "This is printed", 0xf);
    screen_print(1, "using syscalls!", 0xf);
    
    while (getbtns() == 0) {
    }
    screen_clear(0x0);
    screen_print(2, "You pressed", 0x0);
    screen_print(3, "A button!", 0x0);
}