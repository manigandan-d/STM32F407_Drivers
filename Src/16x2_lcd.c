/*
 * 16x2_lcd.c
 *
 *  Created on: Jan 20, 2024
 *      Author: MANIGANDAN D
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lcd.h"

static void mdelay(uint32_t count) {
	for(uint32_t i=0; i<(count * 1000); i++);
}

int main(void) {
	lcd_init();

	lcd_print_string("Hi");

	lcd_set_cursor(2, 1);
	lcd_print_string("Hello");

	mdelay(2000);

	lcd_display_clear();
	lcd_display_return_home();

	while(1);
}
