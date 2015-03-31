mcp7940 Library
*********************************
*	Software Version: V.1		*
*	Author: C. Koiter			*
*********************************
Platform: Arduino
Language: C++

File list:
	- mcp7940.h
	- mcp7940.cpp
	- keywords.txt

Library to support time, date, and alarm functions using the Microchip MCP7940 RTC.

mcp7940 is based primarily on RTClib by jeelabs [ http://github.com/jcw/rtclib ], and tailored to support only the MCP7940. 

Functions:
	- DateTime Class:
		year:		4 digit YEAR portion of the DateTime variable (see now, in RTC class) [Example: int rtcYear = now.year();]
		month:		2 digit MONTH portion of the DateTime variable (see now, in RTC class) 
					[Example: int rtcMonth = now.month();]
		day:		2 digit DAY portion of the DateTime variable (see now, in RTC class) [Example: int rtcDay = now.day();]
		hour:		2 digit HOUR portion (24 hour format) of the DateTime variable (see now, in RTC class)
					[Example: int rtcHour = now.hour();]
		minute:		2 digit MINUTE portion of the DateTime variable (see now, in RTC class) 
					[Example: intMinute = now.minute();]
		second: 	2 digit SECOND portion of the DateTime variable (see now, in RTC class) 
					[Example: intSecond = now.second();]
		
	- RTC Class:
		begin:		unused
		adjust: 	set RTC time. Requires parameter in a DateTime format.
					[Example: RTC.adjust(DateTime(__DATE__, __TIME__)); //sets RTC time to time program was compiled]
		now:		contains all date and time information from the RTC. now cannot be used as it exists, and must be
					be separated into parts for implementation. (see DateTime class) [Example: DateTime now = RTC.now()]
		isrunning:	unused
		configure:	configures the RTC to enable Alarm 1. Does not accept any parameters.[Example: RTC.configure();]
		setAlarm:	resets Alarm 1 interrupt flag and configures alarm interval. Requires a parameter to set the register.
					Putting a value of 0x90 will set the alarm to hourly (when minutes match) and a value of 0x80 will set
					the alarm to minutely (when seconds match). [Example: RTC.setAlarm(0x90);]
		ordinalDate:	convert a date into an ordinal date, where it is represented as a number 1-365 (366 in a leap year).
					Requires a day and month to operate. Syntax -> ordinalDate(day, month)
					[Example: RTC.ordinalDate(rtcDay, rtcMonth);]
