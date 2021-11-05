#include "FlipDisplay_Registers.h"

const int MOTOR_SR_CLOCK_PIN = 18;   // pin 11 on the 74hc595, green
const int MOTOR_SR_LATCH_PIN = 19;   // pin 12 on the 74hc595, blue RCLK
const int MOTOR_SR_DATA_PIN  = 22;   // pin 14 on the 74hc595, white

// keep track of if each motor should be paused
bool motorPaused[BOARD_COUNT][MOTOR_COUNT];

// each motor uses four phases, which phase is it in currently
byte currentPhase[BOARD_COUNT][MOTOR_COUNT];

// which index (in motorRegisterOutput) does the motor belong to
const byte motorRegister[MOTOR_COUNT] = {0, 0, 1, 1};

// each motor how many bits left to shift the phase into the register byte
const byte motorRegisterPosition[MOTOR_COUNT] = {4, 0, 4, 0};

// the byte to send to each register, four bits per motor i.e. (MOTOR COUNT * 4 bits) / 8 bits per register
byte motorRegisterOutput[BOARD_COUNT][MOTOR_COUNT / 2];

// the byte to send to the button register - a 1 in a position means we're reading from that motor
byte startButtonRegisterOutput[BOARD_COUNT];

// 4-step motor phase follows this pattern. In this case its wired as orange/yellow/pink/blue
// # bp yo OR oy pb
// 0 01 10 -- 01 10
// 1 00 11 -- 11 00
// 2 10 01 -- 10 01
// 3 11 00 -- 00 11
//const byte PHASE[PHASE_COUNT] = {B0110, B1100, B1001, B0011};
const byte PHASE[PHASE_COUNT] = {B0110, B0011, B1001, B1100};

const byte PHASE_PAUSE = B0000;

void setupRegisters() {
  pinMode(MOTOR_SR_LATCH_PIN, OUTPUT);
  pinMode(MOTOR_SR_CLOCK_PIN, OUTPUT);
  pinMode(MOTOR_SR_DATA_PIN, OUTPUT);
  
  for (int board = 0; board < BOARD_COUNT; board++) {
    pinMode(MOTOR_ENABLE_PIN[board], OUTPUT);
    enableBoard(board);
    
    pinMode(MOTOR_LOOP_PIN[board], INPUT);

    startButtonRegisterOutput[board] = B00000000;
    
    for (int motor = 0; motor < MOTOR_COUNT; motor++) {
      motorPaused[board][motor] = true;
      currentPhase[board][motor] = 0;
    }
    
    for (int motor = 0; motor < MOTOR_COUNT / 2; motor++) {
      motorRegisterOutput[board][motor] = B00000000;
    }
  }
  
  // clear the registers
  writeRegisters();
}

int getMotorStartPin(int board) {
  return MOTOR_LOOP_PIN[board];
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

int isButtonTriggered(int board) {
  return !digitalRead(MOTOR_LOOP_PIN[board]);
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
  
  byte shiftOutput = motorPhase << motorRegisterPosition[motor];

  // get the current output register that this motor belongs to
  byte currentOutput = motorRegisterOutput[board][motorRegister[motor]];

  // update the bits that control this motor, leave the rest intact
  byte mask = ~(B1111 << motorRegisterPosition[motor]);
  currentOutput = (currentOutput & mask) | shiftOutput;

  // update the register
  motorRegisterOutput[board][motorRegister[motor]] = currentOutput;
}

//void writeRegisters() {
//  int board = 0;
//  for (int board = 0; board < BOARD_COUNT; board++) {
//    // enable this board
//    enableBoard(board);
//    
//    // SR2: bits 0-3: button read
//    // SR1: bits 0-3: motor 2, bits 4-7: motor 3
//    // SR0: bits 0-3: motor 0, bits 4-7: motor 1
//    if (board == 0) {
//      printBinary(motorRegisterOutput[board][0]);
//    }
//
//    digitalWrite(MOTOR_SR_LATCH_PIN, LOW); //RCLK
//    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, startButtonRegisterOutput[board]); // SR2
//    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, motorRegisterOutput[board][1]);    // SR1
//    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, motorRegisterOutput[board][0]);    // SR0
//    digitalWrite(MOTOR_SR_LATCH_PIN, HIGH);
//
//    // if I delay here, I can make it work just fine, but itll slow down with every SR I add
//    delay(2);
//  }
//
//  // delay here works fine with only one SR
////  delay(DELAY_AMT);
//}

// this version is setup where all SRs are chained
void writeRegisters() {
  digitalWrite(MOTOR_SR_LATCH_PIN, LOW); //RCLK
  
  for (int board = BOARD_COUNT - 1; board >= 0; board--) {
    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, startButtonRegisterOutput[board]); // SR2
    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, motorRegisterOutput[board][1]);    // SR1
    shiftOut(MOTOR_SR_DATA_PIN, MOTOR_SR_CLOCK_PIN, LSBFIRST, motorRegisterOutput[board][0]);    // SR0
  }
  digitalWrite(MOTOR_SR_LATCH_PIN, HIGH);

  delayMicroseconds(DELAY_AMT);
//  delayMicroseconds(1500); // works!
//  delayMicroseconds(1400); // works!
}

void disableBoard(int board) {
  digitalWrite(MOTOR_ENABLE_PIN[board], HIGH);
}
void enableBoard(int board) {
  digitalWrite(MOTOR_ENABLE_PIN[board], LOW);

//  // disable all other boards
//  for (int otherBoard = 0; otherBoard < BOARD_COUNT; otherBoard++) {
//    if (board != otherBoard) {
//      disableBoard(otherBoard);
//    }
//  }
}
