#define MAX_DUTY_CYCLE 150
#define TOP_PIN 6
#define SWITCH_PIN 8
// Debounce in microseconds
#define DEBOUNCE_DELAY 50000
#define MODE_COUNT 6
#define RATE_PIN A0
#define RATE_SCALE 50

const int slow_delay = 25;
const int med_delay = 15;
const int fast_delay = 5;
const int no_delay = 1;

const int slow_fade = 40;
const int med_fade = 15;
const int fast_fade = 5;
const int no_fade = 1;

int pinValues[14]     = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int pinDirections[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int currentPin = 0;
int upperPin = 6;
int lowerPin = 5;
int lastAnimation = 0;
int currentDirection = 0;
int switchCount = 0;
int switchPosition = 0;

void setup()
{
  int i;
  for (i = 0; i <= TOP_PIN; i++) {
    pinMode(i, OUTPUT);
  } 
  pinMode(SWITCH_PIN, INPUT);
  switchPosition = digitalRead(SWITCH_PIN);
}

void loop()
{
  int i, pin, rate;
  
  checkSwitch();

  rate = max(analogRead(RATE_PIN) / RATE_SCALE, 1);
  animate(rate);

  // Pulse the LEDs
  for (i = 0; i <= 255; i++) {
    for (pin = 0; pin <= TOP_PIN; pin++) {
      if (pinValues[pin] > i && i < MAX_DUTY_CYCLE) {
        lighton(pin);
      } else {
        lightoff(pin);
      }
    }
  }
}

void checkSwitch()
{
  // Check if switch has moved
  if (digitalRead(SWITCH_PIN) != switchPosition) {
    delayMicroseconds(DEBOUNCE_DELAY);

    // read again to ensure we are not bouncing
    if (digitalRead(SWITCH_PIN) != switchPosition) {
      switchPosition = digitalRead(SWITCH_PIN);
      switchCount++;
      if (switchCount >= MODE_COUNT) {
        switchCount = 0;
      }
    }
  }
}

void animate(int rate)
{
  // Run the current animation
  switch(switchCount) {
  case 0:
    fade_run(rate);
    break;
  case 1:
    fade_split(rate);
    break;
  case 2:
    shimmer(rate);
    break;
  case 3:
    hard_random(rate);
    break;
  case 4:
    light_random(rate);
    break;
  case 5:
    fade_random(rate);
    break;
  }
}

void shimmer(int delay) {
  int pin = 0;
  for (pin = currentPin++; pin <= TOP_PIN; pin++) {
    if (pinDirections[pin] == 0) {
      fade_in(pin, delay);
      if (pinValues[pin] == MAX_DUTY_CYCLE) { pinDirections[pin] = 1; }
    } else {
      fade_out(pin, delay);
      if (pinValues[pin] == 0) { pinDirections[pin] = 0; }
    }
  }
  if (currentPin > TOP_PIN) { currentPin = 0; }
}

void light_random(int delay)
{
  int pin = 0;
  lastAnimation++;
  if (lastAnimation >= delay) {
    lastAnimation = 0;
    pinValues[randomPin()] = random(0, 256);
  }
}

void hard_random(int delay)
{
  int pin = 0;
  lastAnimation++;
  if (lastAnimation >= delay) {
    lastAnimation = 0;
    pin = randomPin();
    if (pinValues[pin] == 0) {
      pinValues[pin] = MAX_DUTY_CYCLE;
    } else {
      pinValues[pin] = 0;
    }
  }
}

void fade_run(int delay) {
  if (currentDirection == 0) {
    fade_in(currentPin, delay);
    if (pinValues[currentPin] == MAX_DUTY_CYCLE) {
      currentPin++;
    }
    if (currentPin > TOP_PIN) {
      currentPin = 0;
      currentDirection = 1;
    }
  } else {
    fade_out(currentPin, delay);
    if (pinValues[currentPin] == 0) {
      currentPin++;
    }
    if (currentPin > TOP_PIN) {
      currentPin = 0;
      currentDirection = 0;
    }
  }
}

void fade_split(int delay) {
  if (currentDirection == 0) {
    fade_in(upperPin, delay);
    fade_in(lowerPin, delay);
    if (pinValues[upperPin] == MAX_DUTY_CYCLE) {
      upperPin++;
      lowerPin--;
    }
    if (upperPin > TOP_PIN) {
      upperPin = TOP_PIN/2;
      lowerPin = TOP_PIN/2 - 1;
      currentDirection = 1;
    }
  } else {
    fade_out(upperPin, delay);
    fade_out(lowerPin, delay);
    if (pinValues[upperPin] == 0) {
      upperPin++;
      lowerPin--;
    }
    if (upperPin > TOP_PIN) {
      upperPin = TOP_PIN/2;
      lowerPin = TOP_PIN/2 - 1;
      currentDirection = 0;
    }
  }
}

void fade_random(int delay) {
  if (pinDirections[currentPin] == 0) {
    fade_in(currentPin, delay);
    if (pinValues[currentPin] == MAX_DUTY_CYCLE) {
      pinDirections[currentPin] = 1;
      currentPin = randomPin();
    }
  } else {
    fade_out(currentPin, delay);
    if (pinValues[currentPin] == 0) {
      pinDirections[currentPin] = 0;
      currentPin = randomPin();
    }
  }
}

int randomPin()
{
  return random(0, TOP_PIN + 1);
}

void lighton(int pin)
{
  if (pin < 8) {
    PORTD |= 1 << pin;
  } else {
    PORTB |= 1 << (pin - 8);
  }
}

void lightoff(int pin)
{
  if (pin < 8) {
    PORTD &= (0xFF ^ (1 << pin));
  } else {
    PORTB &= (0xFF ^ (1 << (pin - 8)));
  }
}
  
void fade_in(int pin, int steps)
{
  pinValues[pin] += MAX_DUTY_CYCLE/steps;
  if (pinValues[pin] > MAX_DUTY_CYCLE) { pinValues[pin] = MAX_DUTY_CYCLE; }
}

void fade_out(int pin, int steps)
{
  pinValues[pin] -= MAX_DUTY_CYCLE/steps;
  if (pinValues[pin] < 0) { pinValues[pin] = 0; }
}
