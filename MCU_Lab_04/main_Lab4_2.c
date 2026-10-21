#include "i2c-lcd.h"
extern I2C_HandleTypeDef hi2c1;  // ดึงตัวแปร I2C จาก main.c มาใช้
#include <string.h>
#define SLAVE_ADDRESS_LCD 0x4E // เปลี่ยนเป็น 0x7E หากจอของคุณใช้ Address 0x3F
static uint8_t backlight_state = 0x08;	// เริ่มต้นให้เปิดไฟ 0x08



void lcd_send_cmd (char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);

    data_t[0] = data_u | 0x04 | backlight_state;
    data_t[1] = data_u | 0x01 | backlight_state;
    data_t[2] = data_l | 0x04 | backlight_state;
    data_t[3] = data_l | 0x01 | backlight_state;
    HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *)data_t, 4, 100);
}

void lcd_send_data (char data) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);

    data_t[0] = data_u | 0x05 | backlight_state;  // en=1, rs=1
    data_t[1] = data_u | backlight_state;       // en=0, rs=1
    data_t[2] = data_l | 0x05 | backlight_state;  // en=1, rs=1
    data_t[3] = data_l | backlight_state;       // en=0, rs=1
    HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *)data_t, 4, 100);
}

void lcd_clear (void) {
    lcd_send_cmd (0x01);
    HAL_Delay(2);
}

void lcd_put_cur(int row, int col) {
    switch (row) {
        case 0: col |= 0x80; break;
        case 1: col |= 0xC0; break;
    }
    lcd_send_cmd (col);
}

void lcd_init (void) {
    HAL_Delay(50);
    lcd_send_cmd (0x30);
    HAL_Delay(5);
    lcd_send_cmd (0x30);
    HAL_Delay(1);
    lcd_send_cmd (0x30);
    HAL_Delay(10);
    lcd_send_cmd (0x20);  // โหมด 4-bit
    HAL_Delay(10);

    lcd_send_cmd (0x28);  // ตั้งค่า 2 แถว, ฟอนต์ 5x8
    HAL_Delay(1);
    lcd_send_cmd (0x0C);  // เปิดจอ, ปิดเคอร์เซอร์
    HAL_Delay(1);
    lcd_send_cmd (0x06);  // เลื่อนเคอร์เซอร์อัตโนมัติ
    HAL_Delay(1);
    lcd_send_cmd (0x01);  // เคลียร์หน้าจอ
    HAL_Delay(2);
}

void lcd_send_string (char *str) {
    while (*str) lcd_send_data (*str++);
}

void lcd_backlight_on(void) {
    backlight_state = 0x08;
    lcd_send_cmd(0x00);
}

void lcd_backlight_off(void) {
    backlight_state = 0x00;
    lcd_send_cmd(0x00);
}

// ฟังก์ชันเลื่อนข้อความตามโจทย์กำหนด
void lcd_scrolling_text(int row, char *text, int start_pos, int lcd_width) {
    char buffer[20];
    int text_len = strlen(text);

    for (int i = 0; i < lcd_width; i++) {
        if (start_pos + i < text_len) {
            buffer[i] = text[start_pos + i];
        } else {
            buffer[i] = ' ';
        }
    }
    buffer[lcd_width] = '\0';

    lcd_put_cur(row, 0);
    lcd_send_string(buffer);
}
