const int SOLENOID = 9;

const int MAX_TAP_LENGTH = 11;

unsigned int taps[MAX_TAP_LENGTH];
unsigned char tapsIndex = 0;
unsigned long lastReadingTimestamp = 0;

typedef enum {
  WAITING,
  LISTENING,
  REPLYING
} State;

State state;

void setup() {
  pinMode(SOLENOID, OUTPUT);
  
  state = WAITING;
}

void loop() {
  switch (state) {
    case WAITING:
      manageWaiting();
      break;

    case LISTENING:
      manageListening();
      break;

    case REPLYING:
      manageReplying();
      break;
  }

  endListeningOnTooManyTaps();
  endListeningOnTimeout();
}

void manageWaiting() {
  if (validReading()) {
    registerTap();
    state = LISTENING;
  }
}

void manageListening() {
  if (validReading()) {
    registerTap();
  }
}

void endListeningOnTooManyTaps() {
  if (tapsIndex == MAX_TAP_LENGTH) {
    state = REPLYING;
  }
}

void endListeningOnTimeout() {
  const int TAP_TIMEOUT_IN_MILLISECONDS = 2000;

  if (state == LISTENING) {
    if (millis() > lastReadingTimestamp + TAP_TIMEOUT_IN_MILLISECONDS) {
      state = REPLYING;
    }
  }
}

void registerTap() {
  unsigned long now = millis();

  taps[tapsIndex++] = now - (tapsIndex ? taps[0] : 0);
  lastReadingTimestamp = now;

  delay(75);
}

boolean validReading() {
  const int PIEZO = A0;
  const int TAP_READING_THRESHOLD = 3;

  return analogRead(PIEZO) > TAP_READING_THRESHOLD;
}

void manageReplying() {
  playBack();
  reset();
}

void reset() {
  for (int i = 0; i < MAX_TAP_LENGTH; i++) {
    taps[i] = 0;
  }
  
  tapsIndex = 0;
  state = WAITING;

  delay(1000);
}

void playBack() {
  delay(1000);
  unsigned long start = millis();
  int i = 0;
  while (i < tapsIndex) {
    while (millis() < start + (i == 0 ? 0 : taps[i])) {
      ;
    }

    knock();
    i++;
  }
}

void knock() {
  digitalWrite(SOLENOID, LOW);
  delay(12);
  digitalWrite(SOLENOID, HIGH);
}
