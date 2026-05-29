#pragma once

#include <stdint.h>

#define RA8875_SPI          SPI1
#define RA8875_CS_PORT      GPIOA
#define RA8875_CS_PIN       GPIO_PIN_15
#define RA8875_RESET_PORT   GPIOB
#define RA8875_RESET_PIN    GPIO_PIN_6
                                

typedef struct
{
    const char *label;
    uint8_t max_value_chars;
} display_row_t;


typedef struct
{
    const char *title;
    
    uint16_t x;
    uint16_t y;

    uint16_t margin_x;
    uint16_t margin_y;
    
    uint16_t title_h;

    uint16_t label_value_gap;
    uint16_t row_gap;

    const display_row_t *rows;
    uint8_t row_count;
} display_box_t;


uint16_t display_box_width(const display_box_t *box);

uint16_t display_box_height(const display_box_t *box);

void display_test(void);

void display_init(void);

void display_status_banner(uint16_t x,
                           uint16_t y,
                           uint16_t w,
                           uint16_t h,
                           uint8_t text_scale,
                           const char *status,
                           uint16_t text_color,
                           uint16_t bg_color);

void display_fault_log(uint16_t x,
                       uint16_t y,
                       uint16_t w,
                       uint16_t h,
                       uint16_t title_h,
                       uint16_t margin_x,
                       uint16_t margin_y,
                       uint16_t row_gap,
                       uint8_t text_scale,
                       const char *title,
                       const char *faults[],
                       uint8_t vc_fault_count,
                       uint8_t pdu_fault_count,
                       uint8_t hvbms_fault_count,
                       uint8_t lvbms_fault_count,
                       uint16_t vc_color,
                       uint16_t pdu_color,
                       uint16_t hvbms_color,
                       uint16_t lvbms_color,
                       uint16_t border_color,
                       uint16_t bg_color);

void display_draw_box(const display_box_t *box,
                      uint16_t border_color,
                      uint16_t bg_color);

void display_draw_box_labels(const display_box_t *box,
                             uint16_t text_color);

void display_update_row_value(const display_box_t *box,
                              uint8_t row_index,
                              const char *value,
                              uint16_t text_color,
                              uint16_t bg_color);

void display_update_box_value(const display_box_t *box,
                              uint8_t row_index,
                              const char *value,
                              uint16_t text_color,
                              uint16_t bg_color);
