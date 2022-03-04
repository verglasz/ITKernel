#include "itklibc.h"

int uninit_global;
int init_global = 42;
int zero_global = 0;

int main() {
	int astackvar[4];
	astackvar[0] = uninit_global;
	astackvar[1] = init_global;
	astackvar[2] = zero_global;
	astackvar[3] = 13;
	int s = sum(astackvar, 4);
    return s - astackvar[1];
}

