/*	library based on jeelabs RTClib [original library at https://github.com/jcw/rtclib ] altered to support Microchip MCP7940M RTC, used in Arduino
	based embedded environments. To use this library, add #include <mcp7940.h> to the top of your program.*/

class DateTime {	//DateTime class constructs the variable to store RTC Date and Time, and is a direct copy from the original RTClib.
  public:

  DateTime(long t =0);
  DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour=0, uint8_t min=0, uint8_t sec=0);
  DateTime(const char* date, const char* time);

  uint16_t year() const  {return 2000+yOff;}
  uint8_t month() const  {return m;}
  uint8_t day() const    {return d;}
  uint8_t hour() const   {return hh;}
  uint8_t minute() const {return mm;}
  uint8_t second() const {return ss;}
  uint8_t dayOfWeek() const;

  long get() const;

  protected:
  uint8_t yOff, m, d, hh, mm, ss;
};

class RTC_MCP7940{	//RTC functions, based off original library. Some functions are designed specifically for use in the
  public:			//3 Channel Data Logger.

  static void begin(){};												//not sure if this is entirely necessary, no use in cpp file
  static void adjust(const DateTime& dt);								//change date and time
  static DateTime now();												//get current time and date from RTC registers
  static uint8_t isrunning();											//check to make sure clock is ticking
  static void configure(uint8_t value);									//configure alarm enables and MFP output
  static void setAlarm(uint8_t value);									//configure alarm settings register
  static uint8_t ordinalDate(uint8_t toDay, uint8_t toMonth);			//convert date and month to ordinal (julian) date

  static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4);}	//bcd to bin conv (RTC to MCU)
  static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10);}	//bin to bcd conv (MCU to RTC)
};
