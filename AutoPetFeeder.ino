// Pins
byte enableStepperPin = 8;
byte directionPin = 5;
byte stepPin = 2;

// Stepper
int pulseWidthMicros = 20; // microseconds
int millisbetweenSteps = 15; // milliseconds - or try 1000 for slower steps
int stepsInOneRotation = 200;
int clockwise = HIGH;
int counterClockwise = LOW;

// Time
long oneSecond = 1000; // milliseconds
long oneMinute = 60 * oneSecond;
long oneHour = 60 * oneMinute;
long oneDay = 24 * oneHour;

// Feeding
int rotationsPerCupOfFood = 3;
int cupsOfFoodPerDay = 3;
int feedingsPerDay = 3;
int rotationsPerFeeding = (rotationsPerCupOfFood * cupsOfFoodPerDay) / feedingsPerDay;
long feedingWindow = 12 * oneHour;
long jamPreventionDelay = oneSecond / 4;
int jamPreventionSteps = 10;


/**
 * Main setup
 */
void setup() {
  Serial.begin(9600);
  pinMode(enableStepperPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  delay(oneSecond);
}

/**
 * Main loop
 */
void loop() {
  for (int feeding = 1; feeding <= feedingsPerDay; feeding++) {
    Serial.println("---");
    Serial.println((String) "Starting feeding " + feeding + " of " + feedingsPerDay);

    long timeTakenToFeed = feed();
    long delayAfterFeeding = getDelayAfterFeeding(feeding);
    long adjustedDelay = delayAfterFeeding - timeTakenToFeed;
    
    Serial.println((String) "Finshed feeding " + feeding + " of " + feedingsPerDay + " in " + formatDuration(timeTakenToFeed));
    Serial.println((String) "Delaying for " + formatDuration(delayAfterFeeding) + ", adjusted " + formatDuration(adjustedDelay));
    delay(adjustedDelay);
  }
}

long getDelayAfterFeeding(int feeding) {
  bool isLastFeeding = feeding == feedingsPerDay;
  if (isLastFeeding) {
    return oneDay - feedingWindow;
  } else {
    return feedingWindow / (feedingsPerDay - 1);
  }
}

/**
 * Dispenses one feedings worth of food, returns the time taken to feed
 */
long feed() {
  long feedingStartTime = millis();
  
  for(int r = 0; r < rotationsPerFeeding * 2; r++) {
    delay(jamPreventionDelay);
    step(jamPreventionSteps, counterClockwise);
    delay(jamPreventionDelay);
    step((stepsInOneRotation / 2) + jamPreventionSteps, clockwise);
  }
  
  long feedingEndTime = millis();
  return feedingEndTime - feedingStartTime;
}

/**
 * Turnings the stepper a number of rotations in the given direction
 */
void step(float numSteps, int direction) {
  digitalWrite(enableStepperPin, LOW);
  digitalWrite(directionPin, direction);
  for(int s = 0; s < numSteps; s++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseWidthMicros);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(pulseWidthMicros);
    delay(millisbetweenSteps);
  }
  
  digitalWrite(enableStepperPin, HIGH);
}

/**
 * Formats a duration of milliseconds into a string like: 1h, 2m, 3s, 4ms
 */
String formatDuration(long millis) {
  long hours = millis / oneHour;
  millis -= hours * oneHour;
  
  long minutes = millis / oneMinute;
  millis -= minutes * oneMinute;
  
  long seconds = millis / oneSecond;
  millis -= seconds * oneSecond;
  
  String formatted = String();
  if (hours > 0) formatted += (String) hours + "h ";
  if (minutes > 0) formatted += (String) minutes + "m ";
  if (seconds > 0) formatted += (String) seconds + "s ";
  if (millis > 0) formatted += (String) millis + "ms";
  formatted.trim();

  return formatted;
}
