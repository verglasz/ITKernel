
#include "itklibc.h"
isize sum(isize * vec, usize len) {
	isize tot = 0;
	for (usize i = 0; i < len; i++) {
		tot += vec[i];
	}
	return tot;
}

