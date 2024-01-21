/*
 * rtc_lcd.c
 *
 *  Created on: Jan 20, 2024
 *      Author: MANIGANDAN D
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "ds3231.h"

#define SYSTICK_TIM_CLK 16000000UL

void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSRVR = (uint32_t*)0xE000E014;
	uint32_t *pSCSR = (uint32_t*)0xE000E010;

    /* calculation of reload value */
    uint32_t count_value = (SYSTICK_TIM_CLK/tick_hz)-1;

    //Clear the value of SVR
    *pSRVR &= ~(0x00FFFFFFFF);

    //load the value in to SVR
    *pSRVR |= count_value;

    //do some settings
    *pSCSR |= ( 1 << 1); //Enables SysTick exception request:
    *pSCSR |= ( 1 << 2);  //Indicates the clock source, processor clock source

    //enable the systick
    *pSCSR |= ( 1 << 0); //enables the counter

}

RTC_Time_t curTime;
RTC_Date_t curDate;

char *time_to_str(RTC_Time_t *rtc_time);
char *date_to_str(RTC_Date_t *rtc_date);
char *get_day_of_week(uint8_t i);

int main() {
	printf("Application is running...\n");

//	if(ds3231_init()) {
//		printf("RTC Initialization failed\n");
//		while(1);
//	}

	ds3231_init();

	init_systick_timer(1);

	curDate.date = 16;
	curDate.month = 10;
	curDate.year = 23;
	curDate.day = MON;

	curTime.time_fmt = TIME_FMT_12HRS_PM;
	curTime.hrs = 3;
	curTime.min = 30;
	curTime.sec = 50;

	ds3231_set_current_date(&curDate);
	ds3231_set_current_time(&curTime);

	ds3231_get_current_date(&curDate);
	ds3231_get_current_time(&curTime);

	char *am_pm;

	if(curTime.time_fmt != TIME_FMT_24HRS) {
		am_pm = (curTime.time_fmt)? "PM" : "AM";

		printf("Current Time: %s %s\n", time_to_str(&curTime), am_pm);
	}

	else {
		printf("Current Time: %s\n", time_to_str(&curTime));
	}

	printf("Current Date: %s (%s)\n", date_to_str(&curDate), get_day_of_week(curDate.day));

	while(1);
}

void num_to_str(uint8_t num, char *buf) {
	if(num < 10) {
		buf[0] = '0';
		buf[1] = num+48;
	}

	else if(num >= 10 && num < 99) {
		buf[0] = (num/10) + 48;
		buf[1] = (num%10) + 48;
	}
}

char *time_to_str(RTC_Time_t *rtc_time) {
	static char time[9];

	num_to_str(rtc_time->hrs, time);
	time[2] = ':';
	num_to_str(rtc_time->min, time+3);
	time[5] = ':';
	num_to_str(rtc_time->sec, time+6);

	time[8] = '\0';

	return time;
}

char *date_to_str(RTC_Date_t *rtc_date) {
	static char date[9];

	num_to_str(rtc_date->date, date);
	date[2] = '/';
	num_to_str(rtc_date->month, date+3);
	date[5] = '/';
	num_to_str(rtc_date->year, date+6);

	date[8] = '\0';

	return date;
}

char *get_day_of_week(uint8_t i) {
	char *days[] = {
			"Sunday",
			"Monday",
			"Tuesday",
			"Wednesday",
			"Thursday",
			"Friday",
			"Saturday"
	};

	return days[i-1];
}

void SysTick_Handler(void) {
	ds3231_get_current_time(&curTime);

	char *am_pm;

	if(curTime.time_fmt != TIME_FMT_24HRS) {
		am_pm = (curTime.time_fmt)? "PM" : "AM";

		printf("Current Time: %s %s\n", time_to_str(&curTime), am_pm);
	}

	else {
		printf("Current Time: %s\n", time_to_str(&curTime));
	}

	ds3231_get_current_date(&curDate);

	printf("Current Date: %s (%s)\n", date_to_str(&curDate), get_day_of_week(curDate.day));
}
