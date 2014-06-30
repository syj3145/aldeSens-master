/*
    Code Developed by the 2014 UC Davis iGEM team (with the help of many examples)
 */
//---------------------------------------------------------------------------------Function Specific Variables
// Anodic Stripping
float ASstartVolt;    // Value delivered from QT instructions
float ASpeakVolt;     // Value delivered from QT instructions
float ASscanRate;     // Value delivered from QT instructions
int ASwaveType;       // Value delivered from QT instructions

// Cyclic Voltammetry
float CVstartVolt;    // Value delivered from QT instructions
float CVpeakVolt;     // Value delivered from QT instructions
float CVscanRate;     // Value delivered from QT instructions
int CVwaveType;

// Potentiostatic Amperometry 
float PAsampTime;     // Value delivered from QT instructions
float PApotVolt;      // Value delivered from QT instructions

// Changing Resolution
char resolution;

// Changing Sampling Speed
int samplingDelay = 500;             //(value in µs) >> 1/samplingDelay = Sampling Rate
float samplingDelayFloat = 500.0;    //(value in µs) >> 1/samplingDelay = Sampling Rate

//---------------------------------------------------------------------------------Pin Assignments

const int readPin = A2;  // Main Analog Input
const int sp4tOne = 10;  // Resolution Switch 1
const int sp4tTwo = 11;  // Resolution Switch 2

elapsedMicros usec = 0;

//---------------------------------------------------------------------------------Pin Assignments

String inStruct;  // Main instructions from USB
double value = 0; // ADC reading value
float aRef = 3.3; // Analog Reference

//---------------------------------------------------------------------------------Setup

void setup() {

  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(readPin, INPUT);
  pinMode(readPin, INPUT);

  analogWriteResolution(12);
  analogReadAveraging(32);
  analogReadRes(16);

  //adc->setAveraging(32); // set number of averages
  //adc->setResolution(16); // set bits of resolution
  //adc->setAveraging(32, ADC_1); // set number of averages
  //adc->setResolution(16, ADC_1); // set bits of resolution

  while (usec < 5000); // wait
  usec = usec - 5000;
}

//---------------------------------------------------------------------------------Main Loop

void loop() {
  if (Serial.available()) {
    // Interprets commands from computer
    // All commands must be terminated with a comma ","

    inStruct = Serial.readStringUntil(',');
  }

  //---------------------------------------------------------------------------------Anodic Stripping
  // Example Instruction "anoStrip0.101.002001,"
  //     Parsed into 
  //        float  ASstartVolt  
  //        float  ASpeakVolt
  //        float  ASscanRate 
  //        int    ASwaveType
  //

  if (inStruct.startsWith("anoStrip")) {
    String numStruct = inStruct.substring(8);
    const char * ASSVarray = numStruct.substring(0, 4).c_str();
    ASstartVolt = atof(ASSVarray);
    const char * ASPVarray = numStruct.substring(4, 8).c_str();
    ASpeakVolt = atof(ASPVarray);
    const char * ASSRarray = numStruct.substring(8, 11).c_str();
    ASscanRate = atof(ASSRarray);
    ASwaveType = numStruct.substring(11).toInt();

    anoStrip();
  }

  //---------------------------------------------------------------------------------Anodic Stripping
  // Example Instruction "cycVolt0.101.002001,"
  //     Parsed into 
  //        float  CVstartVolt  
  //        float  CVpeakVolt
  //        float  CVscanRate
  //        int    CVwaveType 
  //

  if (inStruct.startsWith("cycVolt")) {
    String numStruct = inStruct.substring(7);
    const char * CVSTarray = numStruct.substring(0, 4).c_str();
    CVstartVolt = atof(CVSTarray);
    const char * CVPVarray = numStruct.substring(4, 8).c_str();
    CVpeakVolt = atof(CVPVarray);
    const char * CVSRarray = numStruct.substring(8,11).c_str();
    CVscanRate = atof(CVSRarray);
    CVwaveType = numStruct.substring(11).toInt();

    cycVolt();
  }

  //---------------------------------------------------------------------------------Parsing Potentiostatic Amperometry
  // Example Instruction "potAmpero1.001.00,"
  //      Sampling Time (1.00 seconds)    PAsampTime  float
  //      Potential Voltage (1.00 Volts)  PApotVolt   float 
  //
  if (inStruct.startsWith("potAmpero")) {
    String numStruct = inStruct.substring(9);
    const char * PASTarray = numStruct.substring(0, 4).c_str();
    PAsampTime = atof(PASTarray);
    const char * PAPVarray = numStruct.substring(4).c_str();
    PApotVolt = atof(PAPVarray);

    potAmpero();
  }

  //---------------------------------------------------------------------------------Parsing Resolution Adjustment
  // Exmaple Instruction "resolutionA" or "resolutionB"
  //      A:  +/- 10 uA
  //      B:  +/- 1000 nA  
  //      C:  +/- 100 nA
  //      D:  +/- 10 nA
  //

  if (inStruct.startsWith("resolution")) {
    resolution = inStruct[10];

  }

}



/*
    Code Developed by the 2014 UC Davis iGEM team (with the help of many examples)
 */


// Sine Function 
float phase = 0.0;
float twopi = 3.14159 * 2;

// Cyclic Voltametry
//---------------------------------------------------------------------------------Cyclic Voltammetry
// Example Instruction "cycVolt0.101.001002"
//      Start Volt (0.10 Volts)    CVstartVolt  float
//      Peak Volt  (1.00 Volts)    CVpeakVolt   float
//      Scan Rate  (100 mV/S)      CVscanRate   float
//      Wave Type (  0 - constant  )            int
//                   1 - sin wave
//                   2 - triangle wave
//  
void cycVolt() {
  float CVsampTime = 2000*(CVpeakVolt - CVstartVolt)/CVscanRate;
  sample(CVsampTime, CVwaveType, CVstartVolt, CVpeakVolt);

  inStruct = "";
}

//---------------------------------------------------------------------------------Potentiostatic Amperometry
// Example Instruction "potAmpero1.001.00"
//      Sampling Time     (1.00 seconds) AsampTime  float
//      Potential Voltage (1.00 Volts)   PApotVolt    float 
//
void potAmpero() {
  sample(PAsampTime, 0, PApotVolt, 0);
  inStruct = "";
}

//---------------------------------------------------------------------------------Anodic Stripping
// Example Instruction "anoStrip0.101.005002"
//      Start Volt (0.10 Volts)    ASstartVolt  float
//      Peak Volt  (1.00 Volts)    ASpeakVolt   float
//      Scan Rate (100 mV/S)       ASscanRate   float
//      Wave Type (  0 - constant  )            int
//                   1 - sin wave
//                   2 - triangle wave
//
void anoStrip() {
  float ASsampTime = 1000*(ASpeakVolt - ASstartVolt)/ ASscanRate;
  sample(ASsampTime, ASwaveType, ASstartVolt, ASpeakVolt);
  inStruct = "";
}
//---------------------------------------------------------------------------------Sampling Loop
//  Inputs:
//      float sampTime
//      int   waveType
//      float startVolt
//      float endVolt (or peakVolt)
//

void sample(float sampTime, int waveType, float startVolt, float endVolt) {
  int samples = round(sampTime * (1 / samplingDelayFloat * 1000000)); // With delay of 0.5 ms, 2000 samples per second
  switch (waveType) {
    //---------------------------------------------------------------------------------Constant Potential
    case (0):
    {
      float val = startVolt * 4095.0 / aRef + 2048.0;
      analogWrite(A14, (int)val);

      for (int i = 0; i < samples; i++) {
        value = analogRead(readPin);                  // analog read == # out of 2^16
        Serial.println(value * aRef / 65535.0, 6);    // ratio, value/2^16, is the percent of ADC reference... * aRef (ADC Reference Voltage) == Voltage measured
        while (usec < samplingDelay);                 // wait
        usec = usec - samplingDelay;
      }
    }
    analogWrite(A14, 0);
    break;
    //---------------------------------------------------------------------------------Sine Wave
    case (1):
    {
      for (int i = 0; i < samples; i++) {

        float val2 = sin(phase) * (endVolt/aRef)*4096.0 + 2048.0;
        analogWrite(A14, (int)val2);
        phase = phase + 0.05;


        value = analogRead(readPin);                  // analog read == # out of 2^16
        //Serial.println(value * aRef / 65535.0, 6);    // ratio, value/2^16, is the percent of ADC reference... * aRef (ADC Reference Voltage) == Voltage measured
        Serial.println(value * aRef / 65535.0, 6);    // 
        if (phase >= twopi) phase = 0;
        while (usec < samplingDelay); // wait
        usec = usec - samplingDelay;
      }
      phase = 0.0;
    }
    analogWrite(A14, 0);
    break;
    //---------------------------------------------------------------------------------Triangle Wave
    // Consider the range of the DAC >> 2.048/4096 = 500 uV (range of DAC = 4096 values (0-4095))
    //    500 uV == val3 == 1
    //
    //
    case (2): // triangle wave
    {
      float scanRate = 1000*(endVolt - startVolt)/sampTime;
      float val3 = startVolt/aRef*4096.0 + 2048.0;
      for (int i = 0; i < samples/2; i++) {

        analogWrite(A14, (int)val3);
        val3 += (scanRate/1000.0/samples)*4096.0;

        value = analogRead(readPin);                  // analog read == # out of 2^16
        Serial.println(value * aRef / 65535.0, 6);    // ratio, value/2^16, is the percent of ADC reference... * aRef (ADC Reference Voltage) == Voltage measured
        while (usec < samplingDelay); // wait
        usec = usec - samplingDelay;
      }
      for (int i = 0; i < samples/2; i++) {
        
        val3 -= (scanRate/1000.0/samples)*4096.0;
        analogWrite(A14, (int)val3);

        value = analogRead(readPin);                  // analog read == # out of 2^16
        Serial.println(value * aRef / 65535.0, 6);    // ratio, value/2^16, is the percent of ADC reference... * aRef (ADC Reference Voltage) == Voltage measured
        while (usec < samplingDelay); // wait
        usec = usec - samplingDelay;
      }
    }
    break;
    analogWrite(A14, 0);
  }
}








