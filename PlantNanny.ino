// Definitions
byte statusPins[] = { 10,9,8,7,6,5,4,3,2 }; // From Dry to Wet
byte pumpPin = A5;
byte sensorPin = A0;
byte switchLedPin = 13;
byte switchPin = 12;

/*****************************************
Defaults
*****************************************/

// Default watering mode (default: off)
bool isWateringMode = true;

// How often to check the moisture
int readDelay = 3000;
// At what moisture level should we pump water (only if watering is enabled)
int wateringThreshold = 100;

// How many miliseconds to enable pump for
int wateringLength = 1000;

// Variables
int currentMoisture = 0;

void setup()
{
	// Set up all LEDs
	for (size_t i = 0; i < sizeof(statusPins); i++)
	{
		pinMode(statusPins[i], OUTPUT);

		// Test each pin
		digitalWrite(statusPins[i], HIGH);
		delay(70);
		digitalWrite(statusPins[i], LOW);
	}

	// Sensor Pin
	// Analog - no need to setup mode

	// Switch LED Pin
	pinMode(switchLedPin, OUTPUT);

	// Switch Button Pin
	pinMode(switchPin, INPUT);

	// Pump pin
	pinMode(pumpPin, OUTPUT);

	Serial.begin(115200);
	Serial.println("Program started");
}

void loop()
{

	if (digitalRead(switchPin) == HIGH) {
		isWateringMode = !isWateringMode;
	}

	/*if (isWateringMode) {
	digitalWrite(switchLedPin, HIGH);
	}
	else {
	digitalWrite(switchLedPin, LOW);
	}*/

	currentMoisture = analogRead(sensorPin);
	Serial.println(currentMoisture, DEC);
	ShowStatus(currentMoisture);

	if (isWateringMode && (currentMoisture > wateringThreshold)) {
		Serial.println("Watering the plant!");
		digitalWrite(switchLedPin, HIGH);
		analogWrite(pumpPin, 255);
		delay(wateringLength);
		analogWrite(pumpPin, 0);
		digitalWrite(switchLedPin, LOW);
	}

	delay(readDelay);
}

void ShowStatus(int readout) {
	// Margins - dependable on mouisture sensor.
	int from = 550;
	int to = 900;

	int divider = (int)((to - from) / sizeof(statusPins));
	ResetStatus();

	// Always show at least one LED on
	// this is due to the margin sometimes being lower than the reading.
	digitalWrite(statusPins[0], HIGH);

	for (size_t i = 0; i < sizeof(statusPins); i++)
	{
		if (readout >(divider * i) + from) {
			digitalWrite(statusPins[i], HIGH);
		}
	}
}

void ResetStatus() {
	for (size_t i = 0; i < sizeof(statusPins); i++)
	{
		digitalWrite(statusPins[i], LOW);
	}
}