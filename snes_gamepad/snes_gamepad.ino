/*
 * SNES to Gamepad
 */

// Button Reference
// 10000000 00000000 - B
// 01000000 00000000 - Y
// 00100000 00000000 - Select
// 00010000 00000000 - Start
// 00001000 00000000 - Up
// 00000100 00000000 - Down
// 00000010 00000000 - Left
// 00000001 00000000 - Right
// 00000000 10000000 - A
// 00000000 01000000 - X
// 00000000 00100000 - L
// 00000000 00010000 - R
const unsigned int B_PRESSED       = 1;
const unsigned int Y_PRESSED       = 1 << 1;
const unsigned int SELECT_PRESSED  = 1 << 2;
const unsigned int START_PRESSED   = 1 << 3;
const unsigned int UP_PRESSED      = 1 << 4;
const unsigned int DOWN_PRESSED    = 1 << 5;
const unsigned int LEFT_PRESSED    = 1 << 6;
const unsigned int RIGHT_PRESSED   = 1 << 7;
const unsigned int A_PRESSED       = 1 << 8;
const unsigned int X_PRESSED       = 1 << 9;
const unsigned int L_PRESSED       = 1 << 10;
const unsigned int R_PRESSED       = 1 << 11;

void processPad(usb_gamepad_class &pad,  int data);
const int PIN_CLOCK = PIN_C2;
const int PIN_LATCH = PIN_E0;
const int PIN_DATA1 = PIN_D5;			// Player 1
const int PIN_DATA2 = PIN_C6;			// Player 2

// Debounce time
const int dbTime = 10;


// Read Controller Data
unsigned int rData1;						// Player 1
unsigned int rData2;						// Player 2

// Counters for Controller Data
//int cCount1[16];					// Player 1
//int cCount2[16];					// Player 2

// Timer
int time;

void setup() {
	// Set pin assignments
	// Pull up data pins
	pinMode(PIN_CLOCK, OUTPUT);
	pinMode(PIN_LATCH, OUTPUT);
	pinMode(PIN_DATA1, INPUT_PULLUP);
	pinMode(PIN_DATA2, INPUT_PULLUP);
	
	// Initialize clock and latch
	digitalWrite(PIN_CLOCK, HIGH);
	digitalWrite(PIN_LATCH, HIGH);
}

void loop() {
	// Only process every 1ms
	if (millis() != time) {
		readControllers();
		process();
	}
	time = millis();
}

void readControllers() {
	int i;

	// Reset controller read state
	digitalWrite(PIN_CLOCK, LOW);
	digitalWrite(PIN_LATCH, LOW);
	
	// Latch
	digitalWrite(PIN_LATCH, HIGH);
	delayMicroseconds(12);
	digitalWrite(PIN_LATCH, LOW);
	delayMicroseconds(6);
	rData1 =0;
  rData2 =0; 
	// Cycle 0 - Available immediately
	rData1 |= !digitalRead(PIN_DATA1);
	rData2 |= !digitalRead(PIN_DATA2);
	
	// Cycles 1 - 15 - Pulse clock then read
	for (i = 1; i < 16; i++) {
		digitalWrite(PIN_CLOCK, HIGH);
		delayMicroseconds(6);
		digitalWrite(PIN_CLOCK, LOW);
		delayMicroseconds(6);
		rData1 |= (!digitalRead(PIN_DATA1)<< i);
		rData2 |= (!digitalRead(PIN_DATA2)<< i);
	}

}

void process() {
    
	// Player 1
 processPad(Gamepad1, rData1);
  // Player 2
 processPad(Gamepad2, rData2);

	
}


void processPad(usb_gamepad_class &pad,  int data)
{
    // Player 1
  pad.releaseAll();
  // D-X
  if (data & LEFT_PRESSED)
  {
    pad.setX(D_LEFT);
  }
 if (data & RIGHT_PRESSED)
    pad.setX(D_RIGHT);
  // D-Y
  if (data & UP_PRESSED)
    pad.setY(D_UP);
  if (data & DOWN_PRESSED)
    pad.setY(D_DOWN);
  // B
  if (data & B_PRESSED)
    pad.pressButton(B_1);
  // Y
  if (data & Y_PRESSED)
    pad.pressButton(B_2);
  // A
  if (data & A_PRESSED)
    pad.pressButton(B_3);
  // X
  if (data & X_PRESSED)
    pad.pressButton(B_4);
  // L
  if (data & L_PRESSED)
    pad.pressButton(B_5);
  // R
  if (data & R_PRESSED)
    pad.pressButton(B_6);
  // Start
  if (data & START_PRESSED)
    pad.pressButton(B_7);
  // Select
  if (data & SELECT_PRESSED)
    pad.pressButton(B_8);
  pad.send();

}
