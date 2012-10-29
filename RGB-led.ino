// RGB-led
// Reading values from serial port and setting analog ports
// with this value


// const for LED out
enum Channel { Red, Green, Blue1, Blue2 };

const int LED[]              = { 9, 10, 11, 3 };   // port numbers for channel
int       color[]            = { 255, 128, 0, 0 }; // values range [0..255]
int       active_channel     = Red;
long      previous_timestamp = 0;
long      base_delay         = 0;


class VerboseSerial
{
  public:
  VerboseSerial()
  {
    Serial.begin(9600);      // initialize the serial port
  }

  bool good()
  {
    return Serial.available() > 0;
  }

  int parse_int()
  {
    return Serial.parseInt();
  }

  char read()
  {
    return Serial.read();
  }

  void flush()
  {
    while( good() )
    {
      read();
    }
  }
};


class Error
{
  public:
  Error()
  {
    pinMode( 13, OUTPUT ); // pin 13 for signaling
  }

  void handle_error( unsigned char error)
  {
    modal_blink( error );
  }

  // blinks the onboard led using the pattern provided
  // in info
  // 
  void modal_blink( unsigned char info )
  {
    while( info != 0 ) // each shift decreases the value, without adding 1s
    {
       digitalWrite( 13, info & 1 );  // use last bit to set output
       info = info >> 1;              // shift to the right to have
                                      // it in position for the next round
       delay( 100 );
    }
    digitalWrite( 13, LOW ); // turn led of, when we are finished
  }
};


VerboseSerial verbose_serial;
Error error_handler;


// the setup routine runs once when you press reset:
void setup() {                
  error_handler.modal_blink( 0x55 );// check if it's running
                                      // by triggering blinking
}




bool handle_channel()
{
  int tmp_value  = verbose_serial.parse_int();
  active_channel = constrain( tmp_value, Red, Blue2 );
  return true;
}

bool handle_value()
{
  int tmp_value = verbose_serial.parse_int();
  color[ active_channel ] = constrain( tmp_value, 0, 255 );
  return true;
}

bool update_current_channel()
{
  analogWrite( LED[active_channel],   color[active_channel] );
  return true;
}

bool parse_input()
{
  bool went_well = false;

  if( verbose_serial.good() )
  {
    char command = verbose_serial.read();
    switch( command )
    {
    case 'c':
    case 'C':
      went_well = handle_channel();
      break;
    case 'v':
    case 'V':
      went_well = handle_value();
      break;
    default:
      went_well = false;
    }

    if( went_well )
    {
      went_well = update_current_channel();
    }

    if( !went_well )
    {
      error_handler.handle_error( 0x05 );
    }

    // no matter how it went, skip the rest of the string
    verbose_serial.flush();
  }
  
  return went_well;
}

// the loop routine runs over and over again forever:
void loop() {
  
  long current_timestamp = millis();
  if( current_timestamp - previous_timestamp > base_delay )
  {
     previous_timestamp = current_timestamp;
     parse_input();
  }
}
 

