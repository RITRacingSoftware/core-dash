#pragma once

#include <stdint.h>

#define LINECAR_WIDTH         400
#define LINECAR_HEIGHT        220
#define LINECAR_BYTES_PER_ROW  50

void display_draw_linecar (uint16_t x,
                           uint16_t y,
                           uint16_t linecar_color,
                           uint16_t bg_color);
