#ifndef _IMAGE_DATA_H_
#define _IMAGE_DATA_H_

#include <stdint.h>

/* Set this to 1 to include the embedded image in the build, or 0 to exclude it. */
#define IMAGE_1_ENABLE 0
#define image2_ENABLE 0
#define image3_ENABLE 1

#if image3_ENABLE
extern const uint8_t Background[153600];
#endif

//#if image2_ENABLE
//extern const uint8_t image_data_Background[151200];
//#endif
//
//#if IMAGE_1_ENABLE
//extern const uint8_t image_1[153600];
//#endif

#endif
