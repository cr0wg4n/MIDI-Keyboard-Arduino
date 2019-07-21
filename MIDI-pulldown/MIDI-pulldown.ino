#include "MIDIUSB.h"
#define NUM_ROWS 5
#define NUM_COLS 8

#define NOTE_VELOCITY 64

// Row input pins
const int row1Pin = 6;
const int row2Pin = 5;
const int row3Pin = 4;
const int row4Pin = 3;
const int row5Pin = 2;
//const int row6Pin = 2;

// 74HC595 pins
const int dataPin = 10;
const int latchPin = 16;
const int clockPin = 14;

boolean keyPressed[NUM_ROWS][NUM_COLS];
uint8_t keyToMidiMap[NUM_ROWS][NUM_COLS];

// bitmasks for scanning columns

int bits[] =
{ 
  B00000001,
  B00000010,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B10000000
};
void setup()
{
  keyMapping(48);
  // setup pins output/input mode
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  pinMode(row1Pin, INPUT);
  pinMode(row2Pin, INPUT);
  pinMode(row3Pin, INPUT);
  pinMode(row4Pin, INPUT);
  pinMode(row5Pin, INPUT);
  //pinMode(row6Pin, INPUT);
}

void loop()
{
  for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
  {
    scanColumn(colCtr);
    
    //get row values at this column
    int rowValue[NUM_ROWS];
    rowValue[0] = digitalRead(row1Pin);
    rowValue[1] = digitalRead(row2Pin);
    rowValue[2] = digitalRead(row3Pin);
    rowValue[3] = digitalRead(row4Pin);
    rowValue[4] = digitalRead(row5Pin);
  //  rowValue[5] = !digitalRead(row6Pin);

    // process keys pressed
    for(int rowCtr=0; rowCtr<NUM_ROWS; ++rowCtr)
    {
      if(rowValue[rowCtr] != 0 && !keyPressed[rowCtr][colCtr])
      {
        keyPressed[rowCtr][colCtr] = true;
        noteOn(0,keyToMidiMap[rowCtr][colCtr],NOTE_VELOCITY);
      }
    }

    // process keys released
    for(int rowCtr=0; rowCtr<NUM_ROWS; ++rowCtr)
    {
      if(rowValue[rowCtr] == 0 && keyPressed[rowCtr][colCtr])
      {
        keyPressed[rowCtr][colCtr] = false;
        noteOff(0,keyToMidiMap[rowCtr][colCtr],NOTE_VELOCITY);
      }
    }
  }
  
}
void keyMapping(int note){
  for(int rowCtr = 0; rowCtr < NUM_ROWS; ++rowCtr)
  {
    for(int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
    {
      keyPressed[rowCtr][colCtr] = false;
      keyToMidiMap[rowCtr][colCtr] = note;
      note++;
    }
  }
}
void scanColumn(int colNum)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, bits[colNum]);
  digitalWrite(latchPin, HIGH);
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}
void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}
