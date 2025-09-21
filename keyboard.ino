#include <Wire.h>
#include <Adafruit_MCP4725.h>


// ====== CONFIG ======
const int numRows = 4;
const int numCols = 3;
const int numOctaves = 2; 

// Pins for two octaves
int rowPins[numOctaves][numRows] = {
  {4, 16, 17, 5},   // Octave 1 rows
  {27, 14, 13, 23}     // Octave 2 rows
};

int colPins[numOctaves][numCols] = {
  {34, 35, 32},       // Octave 1 columns
  {33, 25, 26}         // Octave 2 columns
};

uint16_t note1 = 0;
uint16_t note2 = 0;
uint16_t note3 = 0;

// Store switch states
bool switchStates[numOctaves][numRows][numCols];

// {c, c#, d},  
// {d#, e, f},  
// {f#, g, g#},  
// {a, a#, b}

const uint16_t note[numOctaves][numRows][numCols] = {
  { // Octave c4
    {3277, 3342, 3416},  // Row 0
    {3481, 3547, 3620},  // Row 1
    {3686, 3752, 3825},  // Row 2
    {3891, 3956, 4030}   // Row 3
  },
  { // Octave c3
    {2457,  2523,  2596},   // Row 0
    {2662,  2728,  2801},   // Row 1
    {2867,  2932,  3006},   // Row 2
    {3072, 3137,  3211}   // Row 3
  }
};


// dac
Adafruit_MCP4725 dac1, dac2, dac3;
TwoWire I2CBus1 = TwoWire(1);

void setup() {
  Serial.begin(115200);

  // Configure all pins
  for (int o = 0; o < numOctaves; o++) {
    for (int r = 0; r < numRows; r++) {
      pinMode(rowPins[o][r], OUTPUT);
      digitalWrite(rowPins[o][r], LOW);  // idle state
    }
    for (int c = 0; c < numCols; c++) {
      pinMode(colPins[o][c], INPUT); // external pull-downs
    }
  }

  // I2C Bus 0 (GPIO21/22)
  Wire.begin(21, 22);
  dac1.begin(0x60, &Wire); // A0=GND
  dac2.begin(0x61, &Wire); // A0=VCC

  // I2C Bus 1 (GPIO19/18)
  I2CBus1.begin(19, 18);
  dac3.begin(0x60, &I2CBus1); // A0=GND
}

void loop() {
  scanAllMatrices();
  readNote();
  playNote();
  delay(200);
}

void scanAllMatrices() {
  for (int o = 0; o < numOctaves; o++) {
    for (int r = 0; r < numRows; r++) {
      // Activate row
      digitalWrite(rowPins[o][r], HIGH);

      for (int c = 0; c < numCols; c++) {
        switchStates[o][r][c] = (digitalRead(colPins[o][c]) == HIGH);
      }

      // Deactivate row
      digitalWrite(rowPins[o][r], LOW);
    }
  }
}

void readNote() {
  for (int o = 0; o < numOctaves; o++) {
    for (int r = 0; r < numRows; r++) {
      for (int c = 0; c < numCols; c++) {
        if(switchStates[o][r][c] == HIGH){
           if(note1 == 0){
             note1 = note[o][r][c];
           }else if(note2 == 0){
             note2 = note[o][r][c];
           }else if(note3 == 0){
             note3 = note[o][r][c];
           }
        }
      }
    }
  }
}

void playNote(){
  dac1.setVoltage(note1,false);
  dac2.setVoltage(note2,false);
  dac3.setVoltage(note3,false);
  
  Serial.println(note1);
  Serial.println(note2);
  Serial.println(note3);

  note1 = 0;
  note2 = 0;
  note3 = 0;
}
