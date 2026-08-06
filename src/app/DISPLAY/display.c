#include "display.h"
#include "RA8875.h"
#include "spi.h"
#include "rtt.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "appGPIO.h"
#include "debug_screen.h"
#include "race_screen.h"
#include "DataManager.h"
#include "linecar.h"

#define BOTTOM_MARGIN_COMP 2
#define DISPLAY_TEXT_SCALE 0
#define DISPLAY_FONT_WIDTH_SCALE0 8 
#define DISPLAY_FONT_HEIGHT_SCALE0 16
#define DISPLAY_BORDER_MARGIN 1
#define DISPLAY_STATUS_TEXT_SCALE 2

#define LINECAR_CENTER_X    ((SCREEN_WIDTH  - LINECAR_WIDTH)  / 2)
#define LINECAR_CENTER_Y    ((SCREEN_HEIGHT - LINECAR_HEIGHT) / 2)
#define LINECAR_COLOR       0xFB40

void display_init(void)
{
    core_SPI_init(RA8875_SPI, RA8875_CS_PORT, RA8875_CS_PIN);
    RA8875_SPI->CR1 |= 3; // modify clock phase and polarity
    core_GPIO_init(RA8875_RESET_PORT, RA8875_RESET_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP);

    if (!RA8875_begin(
        RA8875_SPI,
        RA8875_RESET_PORT,
        RA8875_RESET_PIN,
        NULL,
        0,
        RA8875_480x272,
        RA8875_16BPP))
    {
        rprintf("Failed to Initialize Display\n");
    }

    RA8875_display_on(1);

    GPIOX(1);
    RA8875_pwm1_config(1, RA8875_PWM_CLK_DIV1024);
    RA8875_pwm1_out(255);
    RA8875_rotate_screen_180();    
       
    uint8_t id = RA8875_read_reg(0x00);
    rprintf("RA8875 ID = 0x%02X\n", id);

    RA8875_graphic_mode();

    display_draw_linecar(LINECAR_CENTER_X, LINECAR_CENTER_Y, LINECAR_COLOR, RA8875_BLACK);
    HAL_Delay (1000);

    draw_debug_screen();
}


//-------------------------HELPERS-------------------------//

static uint16_t display_char_width(void)
{
    return DISPLAY_FONT_WIDTH_SCALE0 * (DISPLAY_TEXT_SCALE + 1);
}

static uint16_t display_text_height(void)
{
    return DISPLAY_FONT_HEIGHT_SCALE0 * (DISPLAY_TEXT_SCALE + 1);
}

static uint16_t display_text_width(const char *text)
{
    if (text == NULL) return 0;
    return strlen(text) * display_char_width();
}

static uint16_t display_max_label_width(const display_box_t *box)
{
    uint16_t max_w = 0;

    for (uint8_t i = 0; i < box->row_count; i++)
    {
        uint16_t w = display_text_width(box->rows[i].label);

        if (w > max_w) max_w = w;
    }

    return max_w;
}

static uint16_t display_max_value_width(const display_box_t *box)
{
    uint8_t max_chars = 0;

    for (uint8_t i = 0; i < box->row_count; i++)
    {
        if (box->rows[i].max_value_chars > max_chars)
        {
            max_chars = box->rows[i].max_value_chars;
        }
    }

    return max_chars * display_char_width();
}

uint16_t display_box_width(const display_box_t *box)
{
    return box->margin_x
         + display_max_label_width(box)
         + box->label_value_gap
         + display_max_value_width(box)
         + box->margin_x;
}

uint16_t display_box_height(const display_box_t *box)
{
    uint16_t rows_h =
        box->row_count * display_text_height()
        + (box->row_count - 1) * box->row_gap;

    return box->title_h
         + box->margin_y
         + rows_h
         + box->margin_y
         + BOTTOM_MARGIN_COMP;
}

static uint16_t display_label_x(const display_box_t *box)
{
    return box->x + box->margin_x;
}

static uint16_t display_value_x(const display_box_t *box)
{
    return box->x
         + box->margin_x
         + display_max_label_width(box)
         + box->label_value_gap;
}

static uint16_t display_row_y(const display_box_t *box, uint8_t row_index)
{
    return box->y
         + box->title_h
         + box->margin_y
         + row_index * (display_text_height() + box->row_gap);
}

static uint16_t display_scaled_char_width(uint8_t scale)
{
    return DISPLAY_FONT_WIDTH_SCALE0 * (scale + 1);
}

static uint16_t display_scaled_text_height(uint8_t scale)
{
    return DISPLAY_FONT_HEIGHT_SCALE0 * (scale + 1);
}

static uint16_t display_scaled_text_width(const char *text,
                                          uint8_t scale)
{
    if (text == NULL) return 0;

    return strlen(text)
         * display_scaled_char_width(scale);
}

//---------------------------------------------------------//


//-----------------------TEXT BOXES------------------------//

void display_draw_box(const display_box_t *box,
                      uint16_t border_color,
                      uint16_t bg_color)
{
    if (box == NULL) return;

    uint16_t box_w = display_box_width(box);
    uint16_t box_h = display_box_height(box);

    RA8875_graphic_mode();

    RA8875_draw_fill_rect(box->x, box->y, box_w, box_h, bg_color);
    RA8875_draw_rect(box->x, box->y, box_w, box_h, border_color);

    if (box->title != NULL && box->title_h > 0)
    {
        RA8875_draw_rect(box->x, box->y, box_w, box->title_h, border_color);

        uint16_t title_w = display_text_width(box->title);
        uint16_t title_x = box->x + ((box_w - title_w) / 2);
        uint16_t title_y = box->y + ((box->title_h - display_text_height()) / 2);

        RA8875_text_mode();
        RA8875_text_scale(DISPLAY_TEXT_SCALE);
        RA8875_text_transparent_color(border_color);
        RA8875_text_cursor_position(title_x, title_y);
        RA8875_text_write(box->title, strlen(box->title));

        RA8875_graphic_mode();
    }
}

void display_draw_box_labels(const display_box_t *box,
                             uint16_t text_color)
{
    if (box == NULL || box->rows == NULL) return;

    RA8875_text_mode();
    RA8875_text_scale(DISPLAY_TEXT_SCALE);
    RA8875_text_transparent_color(text_color);

    for (uint8_t i = 0; i < box->row_count; i++)
    {
        const display_row_t *row = &box->rows[i];

        if (row->label == NULL) continue;

        RA8875_text_cursor_position(
            display_label_x(box),
            display_row_y(box, i)
        );

        RA8875_text_write(row->label, strlen(row->label));
    }

    RA8875_graphic_mode();
}


void display_update_row_value(const display_box_t *box,
                              uint8_t row_index,
                              const char *value,
                              uint16_t text_color,
                              uint16_t bg_color)
{
    if (box == NULL || box->rows == NULL || value == NULL) return;
    if (row_index >= box->row_count) return;

    uint16_t value_x = display_value_x(box);
    uint16_t y = display_row_y(box, row_index);
    uint16_t clear_h = display_text_height();

    uint16_t clear_w =
        display_box_width(box)
        - (value_x - box->x)
        - DISPLAY_BORDER_MARGIN;

    RA8875_graphic_mode();

    RA8875_draw_fill_rect(
        value_x,
        y,
        clear_w,
        clear_h,
        bg_color
    );

    RA8875_text_mode();
    RA8875_text_scale(DISPLAY_TEXT_SCALE);
    RA8875_text_transparent_color(text_color);
    RA8875_text_cursor_position(value_x, y);
    RA8875_text_write(value, strlen(value));

    RA8875_graphic_mode();
}

void display_update_box_value(const display_box_t *box,
                              uint8_t row_index,
                              const char *value,
                              uint16_t text_color,
                              uint16_t bg_color)
{
    display_update_row_value(
        box,
        row_index,
        value,
        text_color,
        bg_color
    );
}

void display_status_banner(uint16_t x,
                           uint16_t y,
                           uint16_t w,
                           uint16_t h,
                           uint8_t text_scale,
                           const char *status,
                           uint16_t text_color,
                           uint16_t bg_color)
{

    if (status == NULL) return;

    uint16_t text_w = display_scaled_text_width(status, text_scale);
    uint16_t text_h = display_scaled_text_height(text_scale);

    // Center Text
    
    uint16_t text_x = x + ((w - text_w) / 2);
    uint16_t text_y = y + ((h - text_h) / 2) - (text_scale * 2);

    
    // Draw banner background + border
    
    RA8875_graphic_mode();

    RA8875_draw_fill_rect(x, y, w, h, bg_color);
    RA8875_draw_rect(x, y, w, h, text_color);

    
    // Draw centered text
     
    RA8875_text_mode();
    RA8875_text_scale(text_scale);
    RA8875_text_transparent_color(text_color);
    RA8875_text_cursor_position(text_x, text_y);
    RA8875_text_write(status,strlen(status));

    RA8875_graphic_mode();
}

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
                       uint16_t bg_color)
{
    if (title == NULL || faults == NULL) return;

    uint8_t fault_count =
        vc_fault_count + pdu_fault_count + hvbms_fault_count + lvbms_fault_count;

    uint16_t text_h = display_scaled_text_height(text_scale);

    RA8875_graphic_mode();

    RA8875_draw_fill_rect(x, y, w, h, bg_color);
    RA8875_draw_rect(x, y, w, h, border_color);
    RA8875_draw_rect(x, y, w, title_h, border_color);

    uint16_t title_w = display_scaled_text_width(title, text_scale);
    uint16_t title_x = x + ((w - title_w) / 2);
    uint16_t title_y = y + ((title_h - text_h) / 2);

    RA8875_text_mode();
    RA8875_text_scale(text_scale);
    RA8875_text_transparent_color(border_color);
    RA8875_text_cursor_position(title_x, title_y);
    RA8875_text_write(title, strlen(title));

    uint16_t body_x = x + margin_x;
    uint16_t body_y = y + title_h + margin_y;
    uint16_t body_h = h - title_h - (2 * margin_y);

    uint16_t line_step = text_h + row_gap;

    if (line_step == 0)
    {
        RA8875_graphic_mode();
        return;
    }

    uint8_t max_lines = body_h / line_step;

    for (uint8_t i = 0; i < (fault_count - 1) && i < (max_lines - 1); i++)
    {
        if (faults[i] == NULL) continue;

        uint16_t fault_color;

        if (i < vc_fault_count) 
        {
            fault_color = vc_color;
        }

        else if (i < (uint8_t)(vc_fault_count + pdu_fault_count))
        {
            fault_color = pdu_color;
        }

        else if (i < (uint8_t)(vc_fault_count + pdu_fault_count + hvbms_fault_count))
        {
            fault_color = hvbms_color;
        }
        
        else
        {
            fault_color = lvbms_color;
        }

        RA8875_text_transparent_color(fault_color);

        RA8875_text_cursor_position(
            body_x,
            body_y + i * line_step
        );

        RA8875_text_write(
            faults[i],
            strlen(faults[i])
        );
    }

    // If there are too many faults to print: 
    if (fault_count > max_lines && max_lines > 0)
    {
        char buf[16];

        snprintf(buf, sizeof(buf),
                 "+%u MORE",
                 fault_count - max_lines + 1);

        RA8875_text_transparent_color(border_color);

        RA8875_text_cursor_position(
            body_x,
            body_y + (max_lines - 1) * line_step
        );

        RA8875_text_write(buf, strlen(buf));
    }

    RA8875_graphic_mode();
}

void display_draw_bar(const display_bar_t *bar,
                      char *value,
                      uint16_t border_color,
                      uint16_t bg_color)
{
    if (bar == NULL) return;

    RA8875_graphic_mode();
    RA8875_draw_fill_rect (bar->x, bar->y, bar->w, bar->h, bg_color);
    RA8875_draw_rect (bar->x, bar->y, bar->w, bar->h, border_color);

    if (bar->title != NULL && bar->title_h > 0)
    { 
        RA8875_draw_rect(bar->x, bar->y, bar->w, bar->title_h, border_color);

        uint16_t title_w = display_text_width(bar->title);
        uint16_t title_x = bar->x + ((bar->w - title_w) / 2);
        uint16_t title_y = bar->y + 1;

        RA8875_text_mode();
        RA8875_text_scale(DISPLAY_TEXT_SCALE);
        RA8875_text_transparent_color(border_color);
        RA8875_text_cursor_position(title_x, title_y);
        RA8875_text_write(bar->title, strlen(bar->title));

        RA8875_graphic_mode();
    }

    if (value != NULL)
    {
        uint16_t value_box_y = bar->y + bar->h - bar->value_h;
        RA8875_draw_rect(bar->x, value_box_y, bar->w, bar->value_h, border_color);

        uint16_t value_w = display_text_width(value);
        uint16_t value_x = bar->x + ((bar->w - value_w) / 2);
        uint16_t value_y = value_box_y + 1;

        RA8875_text_mode();
        RA8875_text_scale(DISPLAY_TEXT_SCALE);
        RA8875_text_transparent_color(border_color);
        RA8875_text_cursor_position(value_x, value_y);
        RA8875_text_write(value, strlen(value));

        RA8875_graphic_mode();
    }
}


void display_update_bar(display_bar_t *bar,
                        char *value,
                        uint16_t percentage,
                        uint16_t fill_color)
{
    if (bar == NULL) return;

    RA8875_graphic_mode();

    uint16_t bar_x = bar->x + 1;
    uint16_t bar_y = bar->y + bar->title_h;
    uint16_t bar_w = bar->w - 2;
    uint16_t bar_h = bar->h - bar->title_h - bar->value_h - 1;

    uint16_t value_x = bar->x + 1;
    uint16_t value_y = bar->y + bar->h - bar->value_h + 1;
    uint16_t value_w = bar->w - 2;
    uint16_t value_h = bar->value_h - 2;

    if (percentage > 100) percentage = 100;

    uint16_t new_bar_h = (percentage * bar_h) / 100;
    uint16_t old_bar_h = bar->last_fill_h;

    // || dash_data.screen_flag is needed for multi screen displays
    if (fill_color != bar->last_fill_color || dash_data.screen_flag)
    {
        RA8875_draw_fill_rect(bar_x,
                              bar_y,
                              bar_w,
                              bar_h,
                              RA8875_BLACK);              // CLEAR BAR

        if (new_bar_h > 0)
        {
            RA8875_draw_fill_rect(bar_x,
                                  bar_y + bar_h - new_bar_h,
                                  bar_w,
                                  new_bar_h,
                                  fill_color);            // REDRAW BAR
        }
    }

    else if (new_bar_h > old_bar_h)
    {
        RA8875_draw_fill_rect(bar_x,
                              bar_y + bar_h - new_bar_h,
                              bar_w,
                              new_bar_h - old_bar_h,
                              fill_color);                // GROW BAR
    }

    else if (new_bar_h < old_bar_h)
    {
        RA8875_draw_fill_rect(bar_x,
                              bar_y + bar_h - old_bar_h,
                              bar_w,
                              old_bar_h - new_bar_h,
                              RA8875_BLACK);             // SHRINK BAR
    }

    bar->last_fill_h = new_bar_h;
    bar->last_fill_color = fill_color;

    RA8875_draw_fill_rect(value_x,
                          value_y,
                          value_w,
                          value_h,
                          RA8875_BLACK);                 // CLEAR VALUE

    if (value != NULL)
    {
        uint16_t text_w = display_text_width(value);
        uint16_t text_x = bar->x + ((bar->w - text_w) / 2);

        RA8875_text_mode();
        RA8875_text_scale(DISPLAY_TEXT_SCALE);
        RA8875_text_transparent_color(RA8875_WHITE);
        RA8875_text_cursor_position(text_x, value_y);
        RA8875_text_write(value, strlen(value));         // UPDATE VALUE

        RA8875_graphic_mode();
    }
}

void display_controls_parameters_log(uint16_t x,
                       uint16_t y,
                       uint16_t w,
                       uint16_t h,
                       uint16_t title_h,
                       uint16_t margin_x,
                       uint16_t margin_y,
                       uint16_t row_gap,
                       uint8_t text_scale,
                       const char *title){
    if(title == NULL) return;


    //uint8_t parameters_count = 
}
