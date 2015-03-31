/*	library based on jeelabs RTClib [original library at https://github.com/jcw/rtclib ] altered to support Microchip MCP7940M RTC, used in Arduino 
	based embedded environments. To use this library, add #include <mcp7940.h> to the top of your program.*/

#include <Wire.h>
#ifndef ENERGIA
#include <avr/pgmspace.h>
#else
#define pgm_read_word(data) *data
#define pgm_read_byte(data) *data
#define PROGMEM
#endif
#include <mcp7940.h>
#include <Arduino.h>

#define RTC_ADD           0x6F
#define RTC_CONREG        0x07
#define RTC_ALARM         0x0D
#define SECONDS_PER_DAY   86400L

/*	THIS SECTION COPIED DIRECTLY FROM RTClib [ https://github.com/jcw/rtclib ]
	USE PER http://opensource.org/licenses/mit-license.php */

static const uint8_t daysInMonth [] PROGMEM = {
  31,28,31,30,31,30,31,31,30,31,30,31
};

static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
  if(y >= 2000)
    y -= 2000;
  uint16_t days = d;
  for(uint8_t i = 1; i<m; ++i)
    days += pgm_read_byte(daysInMonth + i - 1);
  if(m>2 && y%4 ==0)
    ++days;
  return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
  return ((days * 24L + h) * 60 + m) * 60 + s;
}

DateTime::DateTime(long t) {
  ss=t%60;
  t/=60;
  mm=t%60;
  t/=60;
  hh=t%24;
  uint16_t days = t/24;
  uint8_t leap;
  for (yOff = 0; ;++yOff) {
    leap = yOff%4==0;
    if(days<365+leap)
      break;
    days -= 365 + leap;
  }
  for(m=1; ;++m){
    uint8_t daysPerMonth=pgm_read_byte(daysInMonth + m - 1);
    if(leap && m == 2)
      ++daysPerMonth;
    if(days<daysPerMonth)
      break;
    days -= daysPerMonth;
  }
  d=days+1;
}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
  if(year >= 2000)
    year -= 2000;
  yOff = year;
  m = month;
  d = day;
  hh = hour;
  mm = min;
  ss = sec;
}

static uint8_t conv2d(const char* p) {
  uint8_t v=0;
  if('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

DateTime::DateTime (const char* date, const char* time) {
  yOff = conv2d(date + 9);
  switch(date[0]) {
    case 'J': m = date[1] == 'a' ? 1 : m = date[2] == 'n' ? 6 : 7; break;
    case 'F': m = 2; break;
    case 'A': m = date[2] == 'r' ? 4 : 8; break;
    case 'M': m = date[2] == 'r' ? 3 : 5; break;
    case 'S': m = 9; break;
    case 'O': m = 10; break;
    case 'N': m = 11; break;
    case 'D': m = 12; break;
  }
  d=conv2d(date+4);
  hh=conv2d(time);
  mm=conv2d(time+3);
  ss=conv2d(time+6);
}

uint8_t DateTime::dayOfWeek() const{
  uint16_t day = get()/SECONDS_PER_DAY;
  return (day+6)%7;
}

long DateTime::get()const{
  uint16_t days=date2days(yOff, m ,d);
  return time2long(days, hh, mm, ss);
}

/*	END SECTION COPIED FROM RTClib */

/*	THIS SECTION BASED ON RTClib 
	It has been adjusted to support the Microchip MCP7940M
	
	Version:	Initial
	Author:		C.Koiter
	Date:		August 2014
*/

void RTC_MCP7940::adjust(const DateTime& dt) {	//change date and time registers based on user input
  Wire.beginTransmission(RTC_ADD);
  Wire.write((byte) 0);
  Wire.write(bin2bcd((byte) 10000000));
  Wire.write(bin2bcd(dt.minute()));
  Wire.write(bin2bcd(dt.hour()));
  Wire.write(bin2bcd(0));
  Wire.write(bin2bcd(dt.day()));
  Wire.write(bin2bcd(dt.month()));
  Wire.write(bin2bcd(dt.year()-2000));
  Wire.write((byte) 0);
  Wire.endTransmission();
}

DateTime RTC_MCP7940::now(){					//current date and time
  Wire.beginTransmission(RTC_ADD);
  Wire.write((byte) 0);
  Wire.endTransmission();
  Wire.requestFrom(RTC_ADD, 7);
  uint8_t ss = bcd2bin(Wire.read())-80;
  uint8_t mm = bcd2bin(Wire.read());
  uint8_t hh = bcd2bin(Wire.read());
  Wire.read();
  uint8_t d = bcd2bin(Wire.read());
  uint8_t m = bcd2bin(Wire.read());
  uint16_t y = bcd2bin(Wire.read()) + 2000;
  
  return DateTime (y, m, d, hh, mm, ss);
}

void RTC_MCP7940::configure(uint8_t value){		//enable and disable alarms, external oscillator, sqw output
  Wire.beginTransmission(RTC_ADD);
  Wire.write(RTC_CONREG);
  Wire.write(value);
  Wire.endTransmission();
}


void RTC_MCP7940::setAlarm(uint8_t value){		//set Alarm 0 interval, interrupt flag, MFP state when triggered
  Wire.beginTransmission(RTC_ADD);
  Wire.write(RTC_ALARM);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t RTC_MCP7940::ordinalDate(uint8_t toDay, uint8_t toMonth){	//convert user date to ordinal (julian) date
	Wire.beginTransmission(RTC_ADD);
	Wire.write(0x05);
	Wire.endTransmission();
	Wire.requestFrom(RTC_ADD, 1);
	uint8_t leap = Wire.read();
	uint8_t ordinal = 0;
	if(leap>=32){
		switch(toMonth){
			case 1: ordinal = toDay; break;
			case 2: ordinal = toDay + 31; break;
			case 3: ordinal = toDay + 60; break;
			case 4: ordinal = toDay + 91; break;
			case 5: ordinal = toDay + 121; break;
			case 6: ordinal = toDay + 152; break;
			case 7: ordinal = toDay + 182; break;
			case 8: ordinal = toDay + 213; break;
			case 9: ordinal = toDay + 244; break;
			case 10: ordinal = toDay + 274; break;
			case 11: ordinal = toDay + 305; break;
			case 12: ordinal = toDay + 335; break;
		}
	}
	else if(leap<32){												//leap year support
		switch(toMonth){
			case 1: ordinal = toDay; break;
			case 2: ordinal = toDay + 31; break;
			case 3: ordinal = toDay + 59; break;
			case 4: ordinal = toDay + 90; break;
			case 5: ordinal = toDay + 120; break;
			case 6: ordinal = toDay + 151; break;
			case 7: ordinal = toDay + 181; break;
			case 8: ordinal = toDay + 212; break;
			case 9: ordinal = toDay + 243; break;
			case 10: ordinal = toDay + 273; break;
			case 11: ordinal = toDay + 304; break;
			case 12: ordinal = toDay + 334; break;
		}
	}
	return ordinal;
}
