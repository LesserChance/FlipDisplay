#include "FlipDisplay_Registers.h"

const int MOTOR_SR_CLOCK_PIN = 18;   // pin 11 on the 74hc595, green
const int MOTOR_SR_LATCH_PIN = 19;   // pin 12 on the 74hc595, blue
const int MOTOR_SR_DISABLE_PIN = 21; // pin 13 on the 74hc595, yellow
const int MOTOR_SR_DATA_PIN = 22;    // pin 14 on the 74hc595, white
const int MOTOR_START_PIN = 23;      // from button output, red, pull low with 220

// keep track of if each motor should be paused
bool motorPaused[BOARD_COUNT][MOTOR_COUNT] = {{true, true, true, true}};

// each motor uses four phases, which phase is it in currently
byte currentPhase[BOARD_COUNT][MOTOR_COUNT] = {{0, 0, 0, 0}};

// which index (in motorRegisterOutput) does the motor belong to
byte motorRegister[BOARD_COUNT][MOTOR_COUNT] = {{0, 0, 1, 1}};

// each motor how many bits left to shift the phase into the register byte
const byte motorRegisterPosition[BOARD_COUNT][MOTOR_COUNT] = {{4, 0, 4, 0}};

// the byte to send to each register, four bits per motor
byte motorRegisterOutput[BOARD_COUNT][MOTOR_COUNT / 2] = {{B00000000, B00000000}};

// the byte to send to the button register - a 1 in a position means we're reading from that motor
byte startButtonRegisterOutput[BOARD_COUNT] = {B00000000};

const byte PHASE[PHASE_COUNT] = {B0110, B1100, B1001, B0011};

const byte PHASE_PAUSE = B0000;

void setupRegisters() {
  pinMode(MOTOR_SR_LATCH_PIN, OUTPUT);
  pinMode(MOTOR_SR_CLOCK_PIN, OUTPUT);
  pinMode(MOTOR_SR_DATA_PIN, OUTPUT);
  pinMode(MOTOR_SR_DISABLE_PIN, OUTPUT);
  pinMode(MOTOR_START_PIN, INPUT);
  digitalWrite(MOTOR_SR_DISABLE_PIN, LOW);
  
  // clear the registers
  writeRegisters();
}

int getMotorStartPin() {
  return MOTOR_START_PIN;
}

void pauseMotor(int board, int motor) {
  motorPaused[board][motor] = true;
  updateMotorRegister(board, motor);
}

void stepMotor(int board, int motor) {
  motorPaused[board][motor] = false;
  currentPhase[board][motor] = (currentPhase[board][motor] + 1) % PHASE_COUNT;
  updateMotorRegister(board, motor);
}

int isButtonTriggered() {
  return digitalRead(MOTOR_START_PIN);
}

void updateButtonRegister(int board, int motor) {
  // output high on the correct button register
  byte data = B00000000;
  bitSet(data, (7-motor));

  setButtonRegister(board, data);
}

void resetButtonRegister(int board) {
  // output low on the for the whole button register
  setButtonRegister(board, B00000000);
}

void setButtonRegister(int board, byte data) {
  startButtonRegisterOutput[board] = data;
}

void updateMotorRegister(int board, int motor) {
  byte motorPhase = PHASE[currentPhase[board][motor]];

  if (motorPaused[board][motor]) {
    motorPhase = PHASE_PAUSE;
  }
  
  byte shiftOutput = motorPhase << motorRegisterPosition[board][motor];

  // get the current output register that this motor belongs to
  byte currentOutput = motorRegisterOutput[board][motorRegister[board][motor]];

  // update the bits that control this motor, leave the rest intact
  byte mask = ~(B1111 << motorRegisterPosition[board][motor]);
  currentOutput = (currentOutput & mask) | shiftOutput;

  // update the register
  motorRegisterOutput[board][motorRegister[board][motor]] = currentOutput;
}

void writeRegisters() {
  for (int board = 0; board < BOARD_COUNT; board++) {
    // enable this board
    // TODO
    
    // SR2: bits 0-3: button read
    // SR1: bits 0-3: motor 2, bits 4-7: motor 3
    // SR0: bits 0-3: motor 0, bits 4-7: motor 1
    digitalWrite(MOTOR_SR_LATCH_PIN, LOW);
    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, startButtonRegisterOutput[board]); // SR2
    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, motorRegisterOutput[board][1]);    // SR1
    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, motorRegisterOutput[board][0]);    // SR0
    digitalWrite(MOTOR_SR_LATCH_PIN, HIGH);

    // disable this board
    // TODO
  }
  
  delay(DELAY_AMT);
}

//void writeRegistersNow() {
//  for (int board = 0; board < BOARD_COUNT; board++) {
//    // enable this board
//    // TODO
//    
//    // SR2: bits 0-3: button read
//    // SR1: bits 0-3: motor 2, bits 4-7: motor 3
//    // SR0: bits 0-3: motor 0, bits 4-7: motor 1
//    digitalWrite(MOTOR_SR_LATCH_PIN, LOW);
//    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, startButtonRegisterOutput[board]); // SR2
//    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, motorRegisterOutput[board][1]);    // SR1
//    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, motorRegisterOutput[board][0]);    // SR0
//    digitalWrite(MOTOR_SR_LATCH_PIN, HIGH);
//
//    // disable this board
//    // TODO
//  }
//}
