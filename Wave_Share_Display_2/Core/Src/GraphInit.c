
#include "GraphInit.h"
#include "ILI9341_GFX.h"
#include "fonts.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ILI9341_STM32_Driver.h"

#define GRAPH_X0        0
#define GRAPH_Y0        0
#define GRAPH_WIDTH     320
#define GRAPH_HEIGHT    110
#define GRAPH_X1        (GRAPH_X0 + GRAPH_WIDTH  - 1)
#define GRAPH_Y1        (GRAPH_Y0 + GRAPH_HEIGHT - 1)

//graph 2
#define GRAPH_X2_0        0
#define GRAPH_Y2_0        0
#define GRAPH_WIDTH2     320
#define GRAPH_HEIGHT2    240
#define GRAPH_X2_1        (GRAPH_X2_0 + GRAPH_WIDTH2  - 1)
#define GRAPH_Y2_1        (GRAPH_Y2_0 + GRAPH_HEIGHT2 - 1)

//#define ADC_MAX         4095
#define ADC_MAX           4500
//#define ADC_MAX           5120 //25% zoomed out
//#define ADC_MAX           6142 //50% zoomed out

//#define ADC_MAX2            6142
//#define   ADC_MAX2            7166 //75% zoomed out
#define ADC_MAX2           8120 //100% zoomed

#define NUM_POINTS      GRAPH_WIDTH
#define NUM_POINTS2      GRAPH_WIDTH2

/* Colors */
#define C_BG            BLACK
#define C_GRID          0x4208   /* Dark grey  */
#define C_AXIS          WHITE
#define C_GRAPH         GREEN
#define C_TEXT          YELLOW

/* -------------------------------------------------------------------------- */
/*  CIRCULAR BUFFER — stores RAW ADC values                                   */
/* -------------------------------------------------------------------------- */
static uint16_t adc_buffer[NUM_POINTS];
static uint16_t write_idx = 0;
static uint8_t  init_done = 0;

//second ciruclar buffer
static uint16_t adc_buffer2[NUM_POINTS2];
static uint16_t write_idx2 = 0;
static uint8_t  init_done2 = 0;
/* -------------------------------------------------------------------------- */
/*  MAP ADC (0..4095) -> SCREEN Y (bottom..top)                               */
/* -------------------------------------------------------------------------- */
static uint16_t adc_to_y(uint16_t adc)
{
    if (adc > ADC_MAX) adc = ADC_MAX;
    uint32_t scaled2 = ((uint32_t)adc * (GRAPH_HEIGHT - 1)) / ADC_MAX;
    return GRAPH_Y1 - (uint16_t)scaled2;
}

//second adc to y mapping
static uint16_t adc_to_y2(uint16_t adc)
{
    if (adc > ADC_MAX2) adc = ADC_MAX2;
    uint32_t scaled2 = ((uint32_t)adc * (GRAPH_HEIGHT2 - 1)) / ADC_MAX2;
    return GRAPH_Y2_1 - (uint16_t)scaled2;
}

/* ========================================================================== */
/*  Graph_Init — Call ONCE at startup                                         */
/* ========================================================================== */
void Graph_Init(void)
{
    uint16_t i;

    /* Clear graph area once */
    ILI9341_DrawFilledRectangleCoord(GRAPH_X0, GRAPH_Y0, GRAPH_X1, GRAPH_Y1, C_BG);

    /* Start with flat line at bottom */
    for (i = 0; i < NUM_POINTS; i++)
        adc_buffer[i] = 0;

    write_idx = 0;
    init_done = 1;
}

void Graph_Init2(void)
{
    uint16_t i;

    /* Clear graph area once */
    ILI9341_DrawFilledRectangleCoord(GRAPH_X2_0, GRAPH_Y2_0, GRAPH_X2_1, GRAPH_Y2_1, C_BG);

    /* Start with flat line at bottom */
    for (i = 0; i < NUM_POINTS2; i++)
        adc_buffer2[i] = 0;

    write_idx2 = 0;
    init_done2 = 1;
}

/* ========================================================================== */
/*  Graph_Update — Call with each new ADC sample                              */
/*                                                                              */
/*  Oscilloscope wrap mode:                                                     */
/*    - Pen moves left -> right                                                 */
/*    - Only the vertical column at 'write_idx' is cleared                    */
/*    - One line segment connects to the previous point                       */
/*    - At wrap-around (x=0), no line is drawn across the screen               */
/* ========================================================================== */
void Graph_Update(int16_t adc_val)
{
    uint16_t x, y_new;
    uint16_t prev_idx, prev_x, prev_y;

    if (!init_done) Graph_Init();

    /* --- 1. Screen position for this sample -------------------------------- */
    x     = GRAPH_X0 + write_idx;
    y_new = adc_to_y(adc_val);

    /* --- 2. Clear ONLY the 1-pixel-wide column being overwritten ----------- */
    /* This removes the old trace from the previous wrap-around.               */
    ILI9341_DrawLine(x, GRAPH_Y0, x, GRAPH_Y1, C_BG);

    /* --- 3. Store new value in ring buffer --------------------------------- */
    adc_buffer[write_idx] = adc_val;

    /* --- 4. Connect to previous point with a line -------------------------- */
    prev_idx = (write_idx == 0) ? (NUM_POINTS - 1) : (write_idx - 1);
    prev_x   = GRAPH_X0 + prev_idx;
    prev_y   = adc_to_y(adc_buffer[prev_idx]);

    /* Draw line only if previous point is physically adjacent on screen.    */
    /* At wrap-around (write_idx == 0) prev_x is at far right — skip line.   */
    if (prev_x == x - 1)
    {
        ILI9341_DrawLine(prev_x, prev_y, x, y_new, C_GRAPH);
    }
    else
    {
        /* Wrap or first sample: draw just the dot */
        ILI9341_DrawPixel(x, y_new, C_GRAPH);
    }

    /* --- 5. Advance pen ---------------------------------------------------- */
    write_idx = (write_idx + 1) % NUM_POINTS;
}

//for graph 2
void Graph_Update2(int16_t adc_val)
{
    uint16_t x2, y_new2;
    uint16_t prev_idx2, prev_x2, prev_y2;

    if (!init_done) Graph_Init2();

    /* --- 1. Screen position for this sample -------------------------------- */
    x2     = GRAPH_X2_0 + write_idx2;
    y_new2 = adc_to_y2(adc_val);

    /* --- 2. Clear ONLY the 1-pixel-wide column being overwritten ----------- */
    /* This removes the old trace from the previous wrap-around.               */
    ILI9341_DrawLine(x2, GRAPH_Y2_0, x2, GRAPH_Y2_1, C_BG);

    /* --- 3. Store new value in ring buffer --------------------------------- */
    adc_buffer2[write_idx2] = adc_val;

    /* --- 4. Connect to previous point with a line -------------------------- */
    prev_idx2 = (write_idx2 == 0) ? (NUM_POINTS2 - 1) : (write_idx2 - 1);
    prev_x2   = GRAPH_X2_0 + prev_idx2;
    prev_y2   = adc_to_y2(adc_buffer2[prev_idx2]);

    /* Draw line only if previous point is physically adjacent on screen.    */
    /* At wrap-around (write_idx == 0) prev_x is at far right — skip line.   */
    if (prev_x2 == x2 - 1)
    {
        ILI9341_DrawLine(prev_x2, prev_y2, x2, y_new2, C_GRAPH);
    }
    else
    {
        /* Wrap or first sample: draw just the dot */
        ILI9341_DrawPixel(x2, y_new2, C_GRAPH);
    }

    /* --- 5. Advance pen ---------------------------------------------------- */
    write_idx2 = (write_idx2 + 1) % NUM_POINTS2;
}
