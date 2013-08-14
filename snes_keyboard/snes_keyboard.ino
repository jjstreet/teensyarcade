/*
 * SNES to Keyboard
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

const int PIN_CLOCK = 12;
const int PIN_LATCH = 14;
const int PIN_DATA1 = 5;			// Player 1
const int PIN_DATA2 = 16;			// Player 2

// Debounce time
const int dbTime = 10;

// Controller Data
int cData1[16];						// Player 1
int cData2[16];						// Player 2

// Read Controller Data
int rData1[16];						// Player 1
int rData2[16];						// Player 2

// Counters for Controller Data
int cCount1[16];					// Player 1
int cCount2[16];					// Player 2

// Timer
int time;

// Key maps for keyboard - See keycode.h
const int keymap1[] = {				// Player 1
	K_Q,								// B
	K_W,								// Y
	K_E,								// Select
	K_R,								// Start
	K_T,								// Up
	K_Y,								// Down
	K_U,								// Left
	K_I,								// Right
	K_O,								// A
	K_P,								// X
	K_A,								// L
	K_S									// R
};

const int keymap2[] = {				// Player 2
	K_D,								// B
	K_F,								// Y
	K_G,								// Select
	K_H,								// Start
	K_J,								// Up
	K_K,								// Down
	K_L,								// Left
	K_Z,								// Right
	K_X,								// A
	K_C,								// X
	K_V,								// L
	K_B									// R
};

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
	
	// Cycle 0 - Available immediately
	rData1[0] = !digitalRead(PIN_DATA1);
	rData2[0] = !digitalRead(PIN_DATA2);
	
	// Cycles 1 - 15 - Pulse clock then read
	for (i = 1; i < 16; i++) {
		digitalWrite(PIN_CLOCK, HIGH);
		delayMicroseconds(6);
		digitalWrite(PIN_CLOCK, LOW);
		delayMicroseconds(6);
		rData1[i] = !digitalRead(PIN_DATA1);
		rData2[i] = !digitalRead(PIN_DATA2);
	}
	
	// Handle debounce
	for (i = 0; i < 16; i++) {
		// Player 1
		if (rData1[i] == cData1[i] && cCount1[i] > 0) {
			cCount1[i]--;
		}
		if (rData1[i] != cData1[i]) {
			cCount1[i]++;
		}
		if (cCount1[i] >= dbTime) {
			cCount1[i] = 0;
			cData1[i] = rData1[i];
		}
		// Player 2
		if (rData2[i] == cData2[i] && cCount2[i] > 0) {
			cCount2[i]--;
		}
		if (rData2[i] != cData2[i]) {
			cCount2[i]++;
		}
		if (cCount2[i] >= dbTime) {
			cCount2[i] = 0;
			cData2[i] = rData2[i];
		}
	}
}

void process() {
	// Set keys according to keymaps and button presses
	int i;
	for (i = 0; i < 12; i++) {
		// Player 1
		if (cData1[i])
			Keyboard.pressKey(keymap1[i]);
		else
			Keyboard.releaseKey(keymap1[i]);
		// Player 2
		if (cData2[i])
			Keyboard.pressKey(keymap2[i]);
		else
			Keyboard.releaseKey(keymap2[i]);
	}
	// Send keyboard report
	Keyboard.send();
}
