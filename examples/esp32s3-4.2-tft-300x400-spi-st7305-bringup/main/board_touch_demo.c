#include "board_touch.h"

#include "board_config.h"

#if BOARD_ENABLE_TOUCH

#include <stdio.h>
#include <string.h>

#include "board_touch_priv.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "st7305_lcd.h"

static const char *TAG = "touch_demo";

static const uint8_t s_font_digits[10][7] = {
    {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F},
    {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E},
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},
    {0x1F, 0x10, 0x10, 0x1E, 0x01, 0x01, 0x1E},
    {0x0E, 0x10, 0x10, 0x1E, 0x11, 0x11, 0x0E},
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x0E},
};

static void map_touch_point(const ft3269_touch_point_t *in, uint16_t *x, uint16_t *y)
{
    *x = in->y;
    *y = in->x;
    if (*y < BOARD_LCD_V_RES) {
        *y = (BOARD_LCD_V_RES - 1) - *y;
    }
    if (*x >= BOARD_LCD_H_RES) {
        *x = BOARD_LCD_H_RES - 1;
    }
    if (*y >= BOARD_LCD_V_RES) {
        *y = BOARD_LCD_V_RES - 1;
    }
}

static void draw_touch_cross(uint16_t cx, uint16_t cy)
{
    const uint16_t thick = BOARD_TOUCH_CROSS_THICK;
    uint16_t horiz_y = (cy > (thick / 2)) ? (cy - (thick / 2)) : 0;
    uint16_t horiz_h = thick;
    if (horiz_y + horiz_h > BOARD_LCD_V_RES) {
        horiz_h = BOARD_LCD_V_RES - horiz_y;
    }

    uint16_t vert_x = (cx > (thick / 2)) ? (cx - (thick / 2)) : 0;
    uint16_t vert_w = thick;
    if (vert_x + vert_w > BOARD_LCD_H_RES) {
        vert_w = BOARD_LCD_H_RES - vert_x;
    }

    st7305_lcd_fill_rect(0, horiz_y, BOARD_LCD_H_RES, horiz_h, ST7305_COLOR_BLACK);
    st7305_lcd_fill_rect(vert_x, 0, vert_w, BOARD_LCD_V_RES, ST7305_COLOR_BLACK);
}

static void draw_glyph_5x7_scaled(uint16_t x, uint16_t y, char c, uint16_t scale)
{
    const uint8_t *rows = NULL;
    static const uint8_t comma[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x08};
    static const uint8_t blank[7] = {0};

    if (c >= '0' && c <= '9') {
        rows = s_font_digits[c - '0'];
    } else if (c == ',') {
        rows = comma;
    } else {
        rows = blank;
    }

    for (uint16_t ry = 0; ry < 7; ++ry) {
        for (uint16_t rx = 0; rx < 5; ++rx) {
            if (rows[ry] & (1U << (4 - rx))) {
                st7305_lcd_fill_rect(x + rx * scale,
                                     y + ry * scale,
                                     scale,
                                     scale,
                                     ST7305_COLOR_BLACK);
            }
        }
    }
}

static void draw_coord_text(uint16_t x, uint16_t y)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%u,%u", x, y);

    const uint16_t scale = 3;
    const uint16_t char_w = 5 * scale + scale;
    const uint16_t text_w = (uint16_t)(strlen(buf) * char_w);
    const uint16_t text_h = 7 * scale + 4;

    st7305_lcd_fill_rect(0, 0, text_w + 8, text_h, ST7305_COLOR_WHITE);

    uint16_t pen_x = 4;
    uint16_t pen_y = 2;
    for (size_t i = 0; buf[i] != '\0'; ++i) {
        draw_glyph_5x7_scaled(pen_x, pen_y, buf[i], scale);
        pen_x += char_w;
    }
}

static void touch_poll_task(void *arg)
{
    (void)arg;
    ft3269_touch_point_t points[5];

    while (true) {
        uint8_t count = 0;
        esp_err_t err = board_touch_read_raw_points(points, 5, &count);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "read_points: %s", esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }

        if (count > 0) {
            st7305_lcd_clear();
            for (uint8_t i = 0; i < count; ++i) {
                uint16_t x = 0;
                uint16_t y = 0;
                map_touch_point(&points[i], &x, &y);
                ESP_LOGI(TAG, "touch[%u] x=%u y=%u", i, x, y);
                draw_touch_cross(x, y);
                draw_coord_text(x, y);
            }
            st7305_lcd_flush();
        }

        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

void board_touch_demo_start(void)
{
    st7305_lcd_clear();
    st7305_lcd_flush();
    xTaskCreate(touch_poll_task, "touch_poll", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "Touch crosshair demo started");
}

#else

void board_touch_demo_start(void) {}

#endif
