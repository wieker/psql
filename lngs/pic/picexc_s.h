#ifndef PICEXC_H
#define PICEXC_H

#include "libkvec/lowlibs/kspng.h"

struct pic_context {
	struct png_file *file;
	float r;
	float g;
	float b;
};

#endif
