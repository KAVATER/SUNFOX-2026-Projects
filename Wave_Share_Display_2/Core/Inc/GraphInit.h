/*
 * GraphInit.h
 *
 *  Created on: 08-Aug-2026
 *      Author: kushp
 */

#ifndef SRC_GRAPHINIT_H_
#define SRC_GRAPHINIT_H_

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/*  DISPLAY & GRAPH CONFIGURATION (320 x 240 Horizontal)                     */
/* -------------------------------------------------------------------------- */
#define GRAPH_X0        40      /* Left margin for Y labels */
#define GRAPH_Y0        15      /* Top margin for title */
#define GRAPH_WIDTH     270     /* 320 - 40 - 10 right padding */
#define GRAPH_HEIGHT    195     /* 240 - 15 - 30 bottom */

#define GRAPH_X1        (GRAPH_X0 + GRAPH_WIDTH)
#define GRAPH_Y1        (GRAPH_Y0 + GRAPH_HEIGHT)

#define ADC_MAX         4095    /* 12-bit ADC */
#define NUM_POINTS      GRAPH_WIDTH

/* Colors */
#define C_BG            BLACK
#define C_GRID          0x4208  /* Dark grey */
#define C_AXIS          WHITE
#define C_GRAPH         GREEN
#define C_TEXT          YELLOW

void Graph_Init(void);
void Graph_Update(uint16_t adc_val);

#endif /* SRC_GRAPHINIT_H_ */
