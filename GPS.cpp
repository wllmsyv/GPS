#include "GPS.h"



/****************************************************
//        Constructor
****************************************************/
GPS::GPS(void){
    buffer_index = 0;
  _new_position_ready = false;
};



/****************************************************
//        Public Functions
****************************************************/
void GPS::init(void){
  // Initialize comms first to make sure that 
  // messages are recieved.
  init_com_115200();
  
  send_message(PMTK_API_SET_NMEA_OUTPUT_RMC);
  delay(50);
  send_message(PMTK_SET_NMEA_UPDATE_1HZ);
  delay(50);
  send_message(PMTK_API_SET_FIX_CTL_1HZ);
  delay(50);
};

/*
 * Adds characters to an array until a line feed character is passed
 * which indicates the end of the message.
 * 
 * After LF is detected, a check is done to see if the message is valid.
 *  If the message is valid the message is parsed and the gps_location
 *  data structure is populated.
 * In either case, the buffer will be cleared and ready for a new message.
 */
void GPS::add_char(char c){
  char_buffer[buffer_index] = c;
  buffer_index++;
  if(buffer_index >= BUFFER_SIZE){
    //strchr(c, ',');
    buffer_index = 0;
    
  }else if (c == LF){
     if(message_valid("$GPRMC,", char_buffer, buffer_index)) {
        parse_gprmc(char_buffer, buffer_index);
        _new_position_ready = true;
     } 
     buffer_index = 0;
     clear_buffer(char_buffer, BUFFER_SIZE);
  }
};


bool GPS::new_location_ready(void){
  return _new_position_ready;
};


void GPS::get_new_location(gps_location* loc) {
  *loc = location;
  _new_position_ready = false;
}

/****************************************************
//      Private functions
****************************************************/

void GPS::parse_gprmc(char* mesg, uint8_t index){
    location.msg = mesg;
    char *new_mesg_array = mesg;
    uint8_t _index = 0;
    
    // get time
    new_mesg_array = strchr(new_mesg_array, ',')+1;
    String tmp = "";
    for(int i = 0; i < 10; i ++) {
      tmp = tmp + new_mesg_array[i];
    }
    location.ping_time = tmp.toFloat();
    
    //get Latitude
    new_mesg_array = strchr(new_mesg_array, ',')+1;
    new_mesg_array = strchr(new_mesg_array, ',')+1;
    tmp = "";
    for(int i = 0; i < 10; i ++) {
      tmp = tmp + new_mesg_array[i];
    }
    location.latitude = tmp.toFloat();
    
    //get Longitude
    new_mesg_array = strchr(new_mesg_array, ',')+1;
    new_mesg_array = strchr(new_mesg_array, ',')+1;
    tmp = "";
    for(int i = 0; i < 10; i ++) {
      tmp = tmp + new_mesg_array[i];
    }
    location.longitude = tmp.toFloat();

    //get Speed
    new_mesg_array = strchr(new_mesg_array, ',')+1;
    new_mesg_array = strchr(new_mesg_array, ',')+1;
    tmp = "";
    while(new_mesg_array[_index] != ',' && _index < index) {
      tmp = tmp + new_mesg_array[_index];
      _index++;
    }
    location.speed_knots = tmp.toFloat();
    location.speed_kph = location.speed_knots*1.852;
    location.speed_mph = location.speed_knots*1.15078;
    
    //get heading
    new_mesg_array = strchr(new_mesg_array, ',')+1;
    tmp = "";
    _index = 0;
    while(new_mesg_array[_index] != ',' && _index < index) {
      tmp = tmp + new_mesg_array[_index];
      _index++;
    }
    location.heading = tmp.toFloat();

    //get date
    new_mesg_array = strchr(new_mesg_array, ',')+1;
    location.ping_date = "";
    for(int i = 0; i < 6; i ++) {
      location.ping_date = location.ping_date + new_mesg_array[i];
    }
};


void GPS::send_message(String message){
  // PTMK message packet terminates with a carriage return and a line feed
  Serial2.print(message);Serial2.print(CR);Serial2.print(LF);
};

// The buffer should be emptied before reading a new
// message to make sure there is no leftover information
// from another message;
void GPS::clear_buffer(char *c, uint8_t len){
  for(int i = 0; i < len; i++) {
    c[i] = 0x0;
  }
};


// Still need to calculate checksum to fully validate the message.
// For more information refer to: http://www.hhhh.org/wiml/proj/nmeaxor.html
// The webpage has a javascript function that is viewable in the source code
// for the page
bool GPS::message_valid(String msg_type, char *c, uint8_t index) {
  
  bool validation = false;
  
  for(int i = 0; i < 7; i++) {
    if(c[i] != msg_type.charAt(i)) {
      return validation;
    }
  }
  
  if(char_buffer[index-2] == CR && char_buffer[index-1] == LF && char_buffer[index-5] == '*') {
      validation = true;
  }
  
  return validation;
}

// To init comms, broadcast at all available baud rates
// to make sure the message is recieved, then reset to
// the desired baud rate.
void GPS::init_com_115200(){
  Serial2.begin(9600);
  delay(50);
  send_message(PMTK_SET_BAUD_115200);
  delay(50);
  Serial2.begin(57600);
  delay(50);
  send_message(PMTK_SET_BAUD_115200);
  delay(50);
  Serial2.begin(115200);
  delay(50);
};

void GPS::init_com_57600(){
  Serial2.begin(115200);
  delay(50);
  send_message(PMTK_SET_BAUD_57600);
  delay(50);
  Serial2.begin(9600);
  delay(50);
  send_message(PMTK_SET_BAUD_57600);
  delay(50);
  Serial2.begin(57600);
  delay(50);
};

void GPS::init_com_9600(){
  Serial2.begin(115200);
  delay(50);
  send_message(PMTK_SET_BAUD_9600);
  delay(50);
  Serial2.begin(57600);
  delay(50);
  send_message(PMTK_SET_BAUD_9600);
  delay(50);
  Serial2.begin(9600);
  delay(50);
};

