/*
 * SNES to Gamepad
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
	// Reset gamepad
	Gamepad1.releaseAll();
	// D-X
	if (cData[6] == 1 && cData[7] == 0)
		Gamepad1.setX(D_LEFT);
	else if (cData[6] == 0 && cData[7] == 1)
		Gamepad1.setX(D_RIGHT);
	// D-Y
	if (cData[4] == 1&& cData[5] == 0)
		Gamepad1.setY(D_UP);
	else if (cData[4] == 0 && cData[5] == 1)
		Gamepad1.setY(D_DOWN);
	// B
	if (cData[0])
		Gamepad1.pressButton(B_1);
	// Y
	if (cData[1])
		Gamepad1.pressButton(B_2);
	// A
	if (cData[8])
		Gamepad1.pressButton(B_3);
	// X
	if (cData[9])
		Gamepad1.pressButton(B_4);
	// L
	if (cData[10])
		Gamepad1.pressButton(B_5);
	// R
	if (cData[11])
		Gamepad1.pressButton(B_6);
	// Start
	if (cData[3])
		Gamepad1.pressButton(B_7);
	// Select
	if (cData[4])
		Gamepad1.pressButton(B_8);
		
	Gamepad1.send();
}
