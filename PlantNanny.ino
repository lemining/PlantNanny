byte ledGreenPin = 10;
byte ledRedPin = 11;

byte pumpPin = 2;
byte sensorPin = A0;

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
int wateringThreshold = 500;

// How many miliseconds to enable pump for
int wateringLength = 3000;


/*****************************************
Variables
*****************************************/
int currentMoisture = 0;

int ConvertToLinearRange(int min, int max, int current, bool isInverted = false, int maxNumber = 255) {
	float y = ((float)maxNumber / (float)max) * (float)current + (float)min;
	if (!isInverted) {
		return (int)y;
	}
	return max - (int)y;
}

void ShowStatus(int readout) {
	// Margins - dependable on mouisture sensor.
	int from = 0;
	int to = 1024;

	byte red = (byte)ConvertToLinearRange(from, to, readout);
	byte green = (byte)ConvertToLinearRange(from, to, readout, true);

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

	// Pump pin
	pinMode(pumpPin, OUTPUT);
	digitalWrite(pumpPin, LOW);

	Serial.begin(9600);
	Serial.println("Program started");

	// If not in watering mode - make the reading quite frequent
	if (!isWateringMode) {
		Serial.println("INFO: Changing readDelay to 1 second");
		readDelay = 1000;
	}
}

// the loop routine runs over and over again forever:
void loop() {
	if (digitalRead(switchPin) == LOW) {
		isWateringMode = !isWateringMode;
	}

	currentMoisture = analogRead(sensorPin);
	Serial.println(currentMoisture, DEC);
	ShowStatus(currentMoisture);

	if (currentMoisture > wateringThreshold) {

		if (isWateringMode) {
			Serial.println("START: Watering the plant");
			digitalWrite(switchLedPin, LOW);
			digitalWrite(pumpPin, HIGH);
			delay(wateringLength);
			digitalWrite(switchLedPin, HIGH);
			digitalWrite(pumpPin, LOW);
			Serial.println("END: Watering the plant.");

			// Put some randomization here / or sleep arduino
			int randNumber = 100;
			int sleepTime = readDelay * randNumber;
			Serial.print("Going to sleep for ");
			Serial.print((int)(sleepTime / 1000), DEC);
			Serial.println(" seconds x 3"); 
			delay(sleepTime);
			delay(sleepTime);
			delay(sleepTime);
		}
		else {
			// Blink watering led to show that it wants to be watered.
			bool isWateringLightOn = false;
			for (int i = 0; i<10; i++) {
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

