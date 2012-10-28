/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// const for LED out
enum Channel { Red, Green, Blue1, Blue2 };

const int LED[] = { 9, 10, 11, 3 }; // Red led, green, blue1, blue2
int color[]            = { 255, 128, 0, 0 }; // R,G,B1,B2 values are constrained to [0..255]

int activeChannel = Red;

long previousTimestamp = 0;
long baseDelay         = 0;
int frame              = 0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pins as output.
  /* pinMode(LED[Red],   OUTPUT);      */
  /* pinMode(LED[Green], OUTPUT);      */
  /* pinMode(LED[Blue1], OUTPUT);      */
  /* pinMode(LED[Blue2], OUTPUT);   */

  pinMode( 13, OUTPUT );
  
  // initialize the serial port
  Serial.begin( 9600 );
  modalBlink(); // check if it's running by triggering blinking
}

void modalBlink()
{
  bool blink = true;

  for( int i = 0; i < 10; i++ )
  {
    digitalWrite( 13, blink );
    blink = !blink;
    delay( 100 );
  }
}

void handleError()
{
  //modalBlink();
}

bool good()
{
  return Serial.available() > 0;
}

bool handleChannel()
{
  int tmpValue  = Serial.parseInt();
  activeChannel = constrain( tmpValue, Red, Blue2 );
  return true;
}

bool handleValue()
{
  int tmpValue = Serial.parseInt();
  color[ activeChannel ] = constrain( tmpValue, 0, 255 );
  return true;
}

bool updateCurrentChannel()
{
  analogWrite( LED[activeChannel],   color[activeChannel] );
  return true;
}

bool parseInput()
{
  bool wentWell = false;

  if( good() )
  {
    char command = Serial.read();
    switch( command )
    {
    case 'c':
    case 'C':
      wentWell = handleChannel();
      break;
    case 'v':
    case 'V':
      wentWell = handleValue();
      break;
    default:
      wentWell = false;
    }

    if( wentWell )
    {
      wentWell = updateCurrentChannel();
    }

    if( !wentWell )
    {
      handleError();
    }

    // no matter how it went, skip the rest of the string
    while( good() )
    {
      Serial.read();
    }
  }
  
  return wentWell;
}

bool switcher = false;


// the loop routine runs over and over again forever:
void loop() {
  
  long currentTimestamp = millis();
  if( currentTimestamp - previousTimestamp > baseDelay )
  {
     previousTimestamp = currentTimestamp;
     parseInput();
  }
}
 

