#define MAX_DUTY_CYCLE 32

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
int lastDelay = 0;
int currentDirection = 0;

void setup()
{
  int i;
  for (i = 0; i <= 13; i++) {
    pinMode(i, OUTPUT);
  } 
}

void loop()
{
  int i, pin;
  
  lastDelay++;
  if (lastDelay > no_delay) {
    //shimmer(med_fade);
    //light_random(fast_fade);
    //fade_run(fast_fade);
    fade_split(fast_fade);
    //fade_random(fast_fade);

    lastDelay = 0;
  }

  for (i = 0; i <= 255; i++) {
    for (pin = 0; pin <= 13; pin++) {
      if (pinValues[pin] > i && i < MAX_DUTY_CYCLE) {
        lighton(pin);
      } else {
        lightoff(pin);
      }
    }
  }
}

void shimmer(int delay) {
  int pin = 0;
  for (pin = currentPin++; pin <= 13; pin++) {
    if (pinDirections[pin] == 0) {
      fade_in(pin, delay);
      if (pinValues[pin] == MAX_DUTY_CYCLE) { pinDirections[pin] = 1; }
    } else {
      fade_out(pin, delay);
      if (pinValues[pin] == 0) { pinDirections[pin] = 0; }
    }
  }
  if (currentPin > 13) { currentPin = 0; }
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

void fade_run(int delay) {
  if (currentDirection == 0) {
    fade_in(currentPin, delay);
    if (pinValues[currentPin] == MAX_DUTY_CYCLE) {
      currentPin++;
    }
    if (currentPin > 13) {
      currentPin = 0;
      currentDirection = 1;
    }
  } else {
    fade_out(currentPin, delay);
    if (pinValues[currentPin] == 0) {
      currentPin++;
    }
    if (currentPin > 13) {
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
    if (upperPin > 13) {
      upperPin = 6;
      lowerPin = 5;
      currentDirection = 1;
    }
  } else {
    fade_out(upperPin, delay);
    fade_out(lowerPin, delay);
    if (pinValues[upperPin] == 0) {
      upperPin++;
      lowerPin--;
    }
    if (upperPin > 13) {
      upperPin = 6;
      lowerPin = 5;
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
  return random(0, 14);
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
