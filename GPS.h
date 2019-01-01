/* 
 * Testbech
 *  Board: Atmega2650
 *  Breakout: PA6H
 *  Using Serial2 pins 16 & 17
 *  
 * For use with: https://www.adafruit.com/product/746
 * Adafruit Example: https://learn.adafruit.com/adafruit-ultimate-gps
 * Data Sheet Location: https://learn.adafruit.com/adafruit-ultimate-gps/resources
 * PTMK Command Sheet: https://cdn-shop.adafruit.com/datasheets/PMTK+command+packet-Complete-C39-A01.pdf
 * Checksum calculator: http://www.hhhh.org/wiml/proj/nmeaxor.html
 * 
 */
#ifndef _GPS
#define _GPS

#include "Arduino.h"


/*
 *  PTMK Command Sheet: https://cdn-shop.adafruit.com/datasheets/PMTK+command+packet-Complete-C39-A01.pdf
 *  When modifying any of the commands below, the checksum will need to be recalculated.
 *  There is a link above for a checksum calculator.
 *  The checksum is the 2 characters after *. The checksum must be follow by a carriage return and line feed 
 *  character in order to end the message. The void send_message(String) function sends the CR and LF after
 *  sending the message.
 */

// Restart
#define PMTK_CMD_HOT_START "$PMTK101*32"
#define PMTK_CMD_WARM_START "$PMTK102*31"
#define PMTK_CMD_COLD_START "$PMTK103*30"
#define PMTK_CMD_FULL_COLD_START "$PMTK104*37"

// Broadcast Rate
#define PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ  "$PMTK220,10000*2F"
#define PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ  "$PMTK220,5000*1B"
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*2F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

// Fix Position Update Rate.
#define PMTK_API_SET_FIX_CTL_100_MILLIHERTZ  "$PMTK300,10000,0,0,0,0*2C"
#define PMTK_API_SET_FIX_CTL_200_MILLIHERTZ  "$PMTK300,5000,0,0,0,0*18"
#define PMTK_API_SET_FIX_CTL_1HZ  "$PMTK300,1000,0,0,0,0*1C"
#define PMTK_API_SET_FIX_CTL_5HZ  "$PMTK300,200,0,0,0,0*2F"

// Message Outputs
// There are a number if different highly configurable message outputs. However, only the GPRMC Was utilized by this class files.
// For more information see Packet Type: 314 PMTK_API_SET_NMEA_OUTPUT on page 12: http://www.adafruit.com/datasheets/PMTK%20command%20packet-Complete-C39-A01.pdf
#define PMTK_API_SET_NMEA_OUTPUT_RMC "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"

// Querries
#define PMTK_Q_RELEASE "$PMTK605*31"

// Message baud rate
#define PMTK_SET_BAUD_115200 "$PMTK251,115200*1F" // Discourage use with SoftwareSerial.
#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17"



struct gps_location {
  double ping_time;
  double latitude;
  double longitude;
  double speed_mph;//1.15078
  double speed_kph;//1.852
  double speed_knots;
  double heading;
  String ping_date;
  String msg;
};


class GPS {
  
private:
  // Constants
  const static uint8_t BUFFER_SIZE = 100;
  const static char CR = 0xD;
  const static char LF = 0xA;
  
  // Structures
  char char_buffer[BUFFER_SIZE];
  gps_location location;

  // Variables
  uint8_t buffer_index;
  bool _new_position_ready;

  // Functions
	void send_message(String);
  void clear_buffer(char*, uint8_t);
  void parse_gprmc(char*, uint8_t);
  bool message_valid(String, char *, uint8_t);
  
  void init_com_115200(void);
  void init_com_57600(void);
  void init_com_9600(void);
  
public:
  GPS(void);
  void init(void);
  void add_char(char);
  bool new_location_ready(void);
  void get_new_location(gps_location*);
};
#endif
