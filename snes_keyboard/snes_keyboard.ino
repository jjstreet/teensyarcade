/*
 * SNES to Keyboard
 */

const int PIN_CLOCK = 0;
const int PIN_LATCH = 1;
const int PIN_DATA = 2;

const int dbTime = 10;

// Controller Data
int cData[16];

// Read Controller Data
int rData[16];

// Count of data
int cCount[16];

void setup() {
	// Setup pin assignments
	pinMode(PIN_CLOCK, OUTPUT);
	pinMode(PIN_LATCH, OUTPUT);
	pinMode(PIN_DATA, INPUT);
	
	// Initialize clock and latch
	digitalWrite(PIN_CLOCK, HIGH);
	digitalWrite(PIN_LATCH, HIGH);
}

int time;

void loop() {
	if (millis() != time) {
		readData();
		processButtons();
	}
	time = millis();
}

void readData() {
	digitalWrite(PIN_CLOCK, LOW);
	digitalWrite(PIN_LATCH, LOW);
	
	digitalWrite(PIN_LATCH, HIGH);
	delayMicroseconds(12);
	digitalWrite(PIN_LATCH, LOW);
	
	// Cycle 0 - Available immediately
	delayMicroseconds(6);
	rData[0] = !digitalRead(PIN_DATA);
	if (rData[0] == cData[0] && cCount[0] > 0) {
		cCount[0]--;
	}
	if (rData[0] != cData[0]) {
		cCount[0]++;
	}
	if (cCount[0] >= dbTime) {
		cCount[0] = 0;
		cData[0] = rData[0];
	}
	
	// Cycles 1 - 15
	for (int i = 1; i < 16; i++) {
		digitalWrite(PIN_CLOCK, HIGH);
		delayMicroseconds(6);
		digitalWrite(PIN_CLOCK, LOW);
		delayMicroseconds(6);
		rData[i] = !digitalRead(PIN_DATA);
		if (rData[i] == cData[i] && cCount[i] > 0) {
			cCount[i]--;
		}
		if (rData[i] != cData[i]) {
			cCount[i]++;
		}
		if (cCount[i] >= dbTime) {
			cCount[i] = 0;
			cData[i] = rData[i];
		}
	}
}

const int key_map[] = {
	K_Z, K_A, K_V, K_F, K_UP, K_DOWN, K_LEFT, K_RIGHT, K_X, K_S, K_D, K_C
};

const int num_btns = 12;

// Button Reference
// Note controller is active low
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

void processButtons() {
	int i;
	for (i = 0; i < num_btns; i++) {
		if (cData[i]) {
			Keyboard.pressKey(key_map[i]);
		}
		else {
			Keyboard.releaseKey(key_map[i]);
		}
	}
	Keyboard.send();
}
