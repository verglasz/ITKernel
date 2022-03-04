#include "itklibc.h"


int fib(int);

int main(void) {
    int loop = 1;
    while (loop) {
        screen_clear(0x0);
        screen_print(0, "Enter a number", 0x0);
        screen_print(1, "Using the switches", 0x0);
        screen_print(3, "Input 15 to exit", 0x0);
        while (getbtns() ==  0) {}
        while (getbtns() > 0) {}
            
        int sws = getsws();
        loop = sws != 15;
        int r = fib(sws);
        char buf1[30], buf2[30];
        sprintf(buf1, "Fibonacci for %d", sws);
        sprintf(buf2, "is: %d", r);
        screen_clear();
        screen_print(1, buf1, 1);
        screen_print(2, buf2, 1);

        while (getbtns() ==  0) {}
        while (getbtns() > 0) {}
    }
    return 0;
}

int fib(int n) {
    if (n < 3) {
        return n;
    }

    int i, t1 = 0, t2 = 1;
    int nextTerm = t1 + t2;

    for (i = 2; i <= n; i++) {
        nextTerm = t1 + t2;
        t1 = t2;
        t2 = nextTerm;
    }
    return nextTerm;
}

