byte ledGreenPin = 10;
byte ledRedPin = 11;

byte pumpPin = 2;
byte sensorPin = A0;

byte coasterPin = A5;

byte switchLedPin = 13;
byte switchPin = 12;

/*****************************************
 Defaults
*****************************************/

// Default watering mode (default: false)
bool isWateringMode = false;

// How often to check the moisture
int readDelay = 60000;

// At what moisture level should we pump water (only if watering is enabled)
int wateringThreshold = 550;

// How many miliseconds to enable pump for
int wateringLength = 5000;


/*****************************************
 Variables
*****************************************/
int currentSoilMoisture = 0;
bool isWaterInCoaster = 0;

/*****************************************
 Functions
*****************************************/

int ConvertToLinearRange(int current, bool isInverted = false) {
	// Margins - dependable on mouisture sensor.
	int ledMaximum = 255;

	int lowerLevel = 400;
	int highLevel = wateringThreshold;

	if (current < lowerLevel) {
		current = lowerLevel + 1;
	}

	if (current > highLevel) {
		current = highLevel - 1;
	}

	int actual = current - lowerLevel;
	int upperLimit = highLevel - lowerLevel;

	float percent = (float)actual / (float)upperLimit;
	int number = (int)((float)percent * (float)ledMaximum);

	if (isInverted) {
		return ledMaximum - number;
	}
	else {
		return number;
	}

	return (percent * (float)ledMaximum);
}

void ShowStatus(int readout) {
	byte red = (byte)ConvertToLinearRange(readout);
	byte green = (byte)ConvertToLinearRange(readout, true);

	Serial.print("G:");
	Serial.print(green);
	Serial.print(" R:");
	Serial.print(red);
	Serial.print(" V:");
	Serial.println(readout);

	analogWrite(ledGreenPin, green);
	analogWrite(ledRedPin, red);

	// This should be moved elsewhere
	if (isWateringMode) {
		digitalWrite(switchLedPin, HIGH);
	}
	else {
		digitalWrite(switchLedPin, LOW);
	}
}

// the setup routine runs once when you press reset:
void setup() {
	// initialize the digital pin as an output.
	pinMode(ledGreenPin, OUTPUT);
	pinMode(ledRedPin, OUTPUT);

	// Switch LED Pin
	pinMode(switchLedPin, OUTPUT);

	// Switch Button Pin
	pinMode(switchPin, INPUT_PULLUP);

	// Coaster Pin
	pinMode(coasterPin, INPUT_PULLUP);

	// Pump pin
	pinMode(pumpPin, OUTPUT);
	digitalWrite(pumpPin, LOW);

	Serial.begin(9600);
	Serial.println("Program started");

	// If not in watering mode - make the reading quite frequent
	if (!isWateringMode) {
		Serial.println("INFO: Changing readDelay to 2 second");
		readDelay = 2000;
	}
}

// the loop routine runs over and over again forever:
void loop() {
	if (digitalRead(switchPin) == LOW) {
		isWateringMode = !isWateringMode;
	}

	unsigned long time = millis();
	//prints time since program started
	Serial.println(time);

	currentSoilMoisture = analogRead(sensorPin);
	isWaterInCoaster = !digitalRead(coasterPin);
	Serial.println(currentSoilMoisture, DEC);
	ShowStatus(currentSoilMoisture);

	if (isWaterInCoaster) {
		// Still water in the coaster
		Serial.println("Water in the coaster detected.");
	}
	else {
		Serial.println("Coaster is dry.");
	}

	if (!isWaterInCoaster && currentSoilMoisture > wateringThreshold)
	{
		if (isWateringMode) {
			Serial.println("START: Watering the plant");
			digitalWrite(switchLedPin, LOW);
			digitalWrite(pumpPin, HIGH);
			delay(wateringLength);
			digitalWrite(switchLedPin, HIGH);
			digitalWrite(pumpPin, LOW);
			Serial.println("END: Watering the plant.");

			// Put some randomization here / or sleep arduino
			int randNumber = 10;
			unsigned int sleepTime = readDelay * randNumber;
			Serial.print("Going to sleep for ");
			Serial.print((int)(sleepTime / 1000), DEC);

			//TODO make this nicer.
			Serial.println(" seconds x 3");
			delay(sleepTime);

		} else {

			// Blink watering led to show that it wants to be watered.
			bool isWateringLightOn = false;
			for (int i = 0; i < 10; i++) {
				if (isWateringLightOn) {
					digitalWrite(switchLedPin, HIGH);
				}
				else {
					digitalWrite(switchLedPin, LOW);
				}
				isWateringLightOn = !isWateringLightOn;
				delay(500);
			}
		}
	}

	delay(readDelay);
}

