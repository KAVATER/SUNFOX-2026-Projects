/*
 * GraphInit.c
 *
 *  Created on: 08-Aug-2026
 *      Author: kushp
 */

#include "GraphInit.h"
#include "ILI9341_GFX.h"
#include "fonts.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ILI9341_STM32_Driver.h>
/* -------------------------------------------------------------------------- */
/*  DISPLAY & GRAPH CONFIGURATION (320 x 240 Horizontal)                     */
/* -------------------------------------------------------------------------- */
#define GRAPH_X0        0      /* Left margin for Y labels */
#define GRAPH_Y0        0      /* Top margin for title */
#define GRAPH_WIDTH     320    /* 320 - 40 - 10 right padding */
#define GRAPH_HEIGHT    240     /* 240 - 15 - 30 bottom */

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

/* -------------------------------------------------------------------------- */
/*  CIRCULAR BUFFER                                                            */
/* -------------------------------------------------------------------------- */
static uint16_t y_buffer[NUM_POINTS];
static uint16_t write_idx = 0;
static uint8_t  init_done = 0;

/* -------------------------------------------------------------------------- */
/*  MAP ADC → SCREEN Y (ADC=0 at bottom, ADC=4095 at top)                      */
/* -------------------------------------------------------------------------- */
static uint16_t adc_to_y(uint16_t adc)
{
    if (adc > ADC_MAX) adc = ADC_MAX;
    uint32_t scaled = ((uint32_t)adc * (GRAPH_HEIGHT - 1)) / ADC_MAX;
    return (GRAPH_Y1 - 1) - (uint16_t)scaled;
}

/* ============================================================================
 *  Graph_Init — Call ONCE at startup                                          *
 * ============================================================================ */
void Graph_Init(void)
{
    uint16_t i;
    char label[8];

    /* Clear graph area */
    ILI9341_DrawFilledRectangleCoord(GRAPH_X0, GRAPH_Y0, GRAPH_X1, GRAPH_Y1, C_BG);

//    /* Border */
//    ILI9341_DrawHollowRectangleCoord(GRAPH_X0, GRAPH_Y0, GRAPH_X1, GRAPH_Y1, C_AXIS);

    /* Horizontal grid lines (4 lines = 5 divisions) */
//    for (i = 1; i < 5; i++)
//    {
//        uint16_t y = GRAPH_Y0 + (GRAPH_HEIGHT * i) / 5;
//        ILI9341_DrawHLine(GRAPH_X0 + 1, y, GRAPH_WIDTH - 1, C_GRID);
//    }
//
//    /* Vertical grid lines (5 lines = 6 divisions) */
//    for (i = 1; i < 6; i++)
//    {
//        uint16_t x = GRAPH_X0 + (GRAPH_WIDTH * i) / 6;
//        ILI9341_DrawVLine(x, GRAPH_Y0 + 1, GRAPH_HEIGHT - 1, C_GRID);
//    }

    /* Y-axis labels */
//    ILI9341_DrawText("4095", FONT3, 2, GRAPH_Y0, C_TEXT, C_BG);
//
//    snprintf(label, sizeof(label), "%4d", ADC_MAX * 3 / 4);
//    ILI9341_DrawText(label, FONT3, 2, GRAPH_Y0 + GRAPH_HEIGHT/4 - 4, C_TEXT, C_BG);
//
//    snprintf(label, sizeof(label), "%4d", ADC_MAX / 2);
//    ILI9341_DrawText(label, FONT3, 2, GRAPH_Y0 + GRAPH_HEIGHT/2 - 4, C_TEXT, C_BG);
//
//    snprintf(label, sizeof(label), "%4d", ADC_MAX / 4);
//    ILI9341_DrawText(label, FONT3, 2, GRAPH_Y0 + GRAPH_HEIGHT*3/4 - 4, C_TEXT, C_BG);
//
//    ILI9341_DrawText("   0", FONT3, 2, GRAPH_Y1 - 8, C_TEXT, C_BG);
//
//    /* Title */
//    ILI9341_DrawText("ADC Live Graph", FONT3, GRAPH_X0 + 80, 2, C_TEXT, C_BG);

    /* Init buffer to bottom line */
    for (i = 0; i < NUM_POINTS; i++)
        y_buffer[i] = adc_to_y(0);

    write_idx = 0;
    init_done = 1;
}

/* ============================================================================
 *  Graph_Update — Call with each new ADC value                                *
 *  Draws scrolling waveform: new point enters at RIGHT, old points shift LEFT *
 * ============================================================================ */
void Graph_Update(uint16_t adc_val)
{
    uint16_t i;
    uint16_t prev_x, prev_y;
    uint8_t first;

    if (!init_done) Graph_Init();

    /* Store new point in circular buffer */
    y_buffer[write_idx] = adc_to_y(adc_val);

    /* ---------------------------------------------------------------------- */
    /*  REDRAW ENTIRE TRACE                                                   */
    /*  Rightmost pixel (GRAPH_X1) = newest sample                            */
    /*  Leftmost pixel (GRAPH_X0)  = oldest sample                            */
    /* ---------------------------------------------------------------------- */

    first = 1;

    for (i = 0; i < NUM_POINTS; i++)
    {
        /* Read buffer: newest first */
        uint16_t buf_idx = (write_idx + NUM_POINTS - i) % NUM_POINTS;
        uint16_t x = GRAPH_X1 - i;      /* Right to left */
        uint16_t y = y_buffer[buf_idx];

        if (x < GRAPH_X0) break;        /* Off left edge */

        if (first)
        {
            ILI9341_DrawPixel(x, y, C_GRAPH);   /* Rightmost point only */
            first = 0;
        }
        else
        {
            /* Connect previous point to current with a line */
            ILI9341_DrawLine(prev_x, prev_y, x, y, C_GRAPH);
        }

        prev_x = x;
        prev_y = y;
    }

    /* ---------------------------------------------------------------------- */
    /*  OVERWRITE OLD ADC VALUE TEXT (prevent ghosting)                       */
    /* ---------------------------------------------------------------------- */
//    char val_str[16];
//    snprintf(val_str, sizeof(val_str), "ADC:%4d", adc_val);

//    ILI9341_DrawFilledRectangleCoord(GRAPH_X1 - 70, GRAPH_Y0 + 2,
//                                       GRAPH_X1 - 2, GRAPH_Y0 + 14, C_BG);

    ILI9341_DrawFilledRectangleCoord(GRAPH_X0 , GRAPH_Y0,
                                       GRAPH_X1, GRAPH_Y1, C_BG);

    //ILI9341_DrawText(val_str, FONT3, GRAPH_X1 - 70, GRAPH_Y0 + 2, C_TEXT, C_BG);

    /* Advance circular buffer */
    write_idx = (write_idx + 1) % NUM_POINTS;
}
//void Graph_Update(uint16_t adc_val)
//{
//
//}
