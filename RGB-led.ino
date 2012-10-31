// RGB-led
// Reading values from serial port and setting analog ports
// with this value

// const for LED out
enum Channel { Red, Green, Blue1, Blue2 };

const int LED[]              = { 9, 11, 10, 3 };   // port numbers for channel
int       color[]            = { 255, 128, 0, 0 }; // values range [0..255]
int       active_channel     = Red;
long      previous_timestamp = 0;
long      base_delay         = 10;

const int input_buffer_length  = 64;
char input_buffer[ input_buffer_length + 1 ];
char command = 'n';
int  number = 0;
int  idle_counter =0;


class VerboseSerial
{
  public:
  VerboseSerial()
  {
    Serial.begin(9600);      // initialize the serial port
    Serial.println( "Im here" );
    
  }

  ~VerboseSerial()
  {
    Serial.println( "Bye bye..." );
  }
  
  int available()
  {
    return Serial.available();
  }
  
          
  bool good()
  {
    int available = Serial.available();
    
    Serial.print( "good: " );
    Serial.println( available );
    
    return available > 0;
  }

  int parse_int()
  {
    int value = Serial.parseInt();

    Serial.print( "Int value: " );
    Serial.println( value );
    
    return value;
    
  }

  int read_bytes( char *buffer, int size )
  {
    int number = Serial.readBytes(buffer, size);
    Serial.println( buffer );
    
    return number;
    
  }
  
          
                

  char read()
  {
    char value = Serial.read();

    Serial.print( "read: " );
    Serial.println( value );
    
    return value;
  }

  void flush()
  {
    Serial.println( "flush" );
    
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


VerboseSerial *verbose_serial;
Error error_handler;

int parse_int( char *buffer,  int &index )
{
  int number = 0;
  
  while( buffer[index] != 0 && (buffer[index] < 30 || buffer[index] > 39 ))
  {
    ++index;
  }
  
  while( buffer[index] != 0 && buffer[index] > 30 && buffer[index] < 39 )
  {
    number = (number * 10) + (buffer[index] - 30);
    ++index;
  }
  
  return number;
}

bool handle_channel(char *buffer, int &index)
{
  int tmp_value  = parse_int(buffer, index);
  active_channel = constrain( tmp_value, Red, Blue2 );
  return true;
}


bool handle_value(char *buffer, int &index)
{
  int tmp_value = parse_int(buffer, index);
  color[ active_channel ] = constrain( tmp_value, 0, 255 );
  return true;
}



bool update_current_channel()
{
  // Serial.print( "update channel " );
  // Serial.print( active_channel );
  // Serial.print( " value " );
  // Serial.println( color[active_channel] );
  
  analogWrite( LED[active_channel],   color[active_channel] );
  return true;
}

void handle_command( char command )
{
  if( command != 'n' )
  {
    switch( command )
    {
    case 'c':
      active_channel = constrain( number, Red, Blue2 );
      break;
    case 'v':
      color[active_channel] = constrain( number, 0, 255 );
      update_current_channel();
      break;
    default:
      break;
    }
    
    number = 0;
  }
}


bool parse_input()
{
  int avail = Serial.available();
  memset( input_buffer, 0, input_buffer_length );
  Serial.readBytes( input_buffer, avail );
  
  if( avail > 0 )
  {
    idle_counter = 0;
    
     // Serial.print( "Bingo " );
     // Serial.print( avail );
     // Serial.print( " in " );
     // Serial.print( input_buffer );
     // Serial.println( " KK " );
    
    
    bool found   = false;
    char new_command = 'n';
    char c;
    
    for( int i=0; i<avail; ++i )
    {
      c = input_buffer[i];
      if( c >= 0x30 && c<= 0x39 )
      {
        // digit
        int d = c - 0x30;
        number = number * 10 + d;
      }
      else
      {
        new_command = c;
        found = true;
      }
      
      if( found )
      {
        handle_command( command );
        command = new_command;
        found = false;
      }
    }
  }
  else
  {
    ++idle_counter;
    if( idle_counter == 1 )
    {
     // Serial.print( "Bingo " );
     // Serial.print( avail );
     // Serial.print( " in " );
     // Serial.println( " KK " );
      handle_command( command );
      command = 'n';
      
    }
  }
}

// the setup routine runs once when you press reset:
void setup() {        
  memset( input_buffer, 0, input_buffer_length - 1 );
  
  verbose_serial = new VerboseSerial();

  command = 'n';
  number  = 0;
  idle_counter = 0;

  error_handler.modal_blink( 0x55 );// check if it's running
                                    // by triggering blinking
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
 

