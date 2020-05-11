// 16 Channel PT1000 Reader
// Lee Hagaman 2020

const int analog_pin = A0;
const int analog_pin_amp = A1;
const int heater_1_pin = 6;
const int heater_2_pin = 7;
const int digital_pin_0 = 2;
const int digital_pin_1 = 3;
const int digital_pin_2 = 4;
const int digital_pin_3 = 5;

const int resolution = pow(2,10); // 10 bit DAC
const float vmax = 3.3;
const float analog_to_voltage = vmax / resolution;

int activePTs[] = {8, 9};

// list of more precisely measured values for the 16 installed 1kOhm resistors
float resistors[] = {998.4, 983.3, 983.3, 985.0, 982.2, 985.1, 979.4, 986.5, 986.1, 983.3, 983.2, 983.0, 984.5, 982.6, 989.9, 980.7};

float roff1 = 978.7; // resistance for offset generator connected to vmax
float roff2 = 980.8; // resistance for offset generator connected to ground
float r1 = 981.8;  // resistance for amplifier, average of the measured values 982.4 and 981.2, the measured values for r1amp and r2amp
float r2 = 9735;  // resistance for amplifier, average of the measured values 9729 and 9724, the measured values for r3amp and r4amp

const int delay_after_multiplexing = 10; // milliseconds
const int delay_between_readings = 10; // milliseconds

const int numPTs = sizeof(activePTs) / sizeof(activePTs[0]);
int ptindex = 0;
int ptnum;
float v;
float v_amp;
float v_amp_converted;
float rpt;
float rpt_amp;
float temp;
float temp_amp;

// 10 * PT1000 resistance, or 100 * PT100 resistance
// -200 C to 50 C in one degree increments
// from https://github.com/drhaney/pt100rtd/blob/master/pt100rtd.h
int resistances[] = {
  1852, 1896, 1939, 1982, 2025, 2068, 2111, 2154, 2197, 2240, // -200 C
  2283, 2326, 2369, 2412, 2455, 2497, 2539, 2582, 2625, 2667, 
  2710, 2752, 2795, 2837, 2880, 2922, 2965, 3007, 3049, 3092, 
  3134, 3176, 3218, 3261, 3303, 3345, 3386, 3428, 3470, 3512, 
  3554, 3596, 3638, 3680, 3722, 3763, 3805, 3847, 3889, 3931, 
  3972, 4014, 4056, 4097, 4139, 4180, 4222, 4264, 4305, 4346, // -150 C
  4388, 4429, 4471, 4512, 4553, 4595, 4635, 4676, 4718, 4759, 
  4800, 4841, 4882, 4923, 4964, 5006, 5047, 5088, 5129, 5170, 
  5211, 5252, 5292, 5333, 5374, 5415, 5456, 5497, 5538, 5578, 
  5619, 5660, 5700, 5741, 5782, 5822, 5863, 5904, 5944, 5985, 
  6026, 6067, 6107, 6148, 6187, 6229, 6269, 6310, 6350, 6391, // -100 C
  6430, 6470, 6511, 6551, 6591, 6631, 6672, 6712, 6752, 6792, 
  6833, 6873, 6913, 6953, 6993, 7033, 7073, 7113, 7153, 7193, 
  7233, 7273, 7313, 7353, 7393, 7433, 7473, 7513, 7553, 7593,
  7633, 7673, 7713, 7752, 7792, 7832, 7872, 7911, 7951, 7991, 
  8031, 8070, 8110, 8150, 8189, 8229, 8269, 8308, 8348, 8388, //  -50 C
  8427, 8467, 8506, 8546, 8585, 8625, 8664, 8704, 8743, 8783, 
  8822, 8862, 8901, 8940, 8980, 9019, 9059, 9098, 9137, 9177,
  9216, 9255, 9295, 9334, 9373, 9412, 9452, 9491, 9530, 9569, 
  9609, 9648, 9687, 9726, 9765, 9804, 9844, 9883, 9922, 9961, 
  10000,10039,10078,10117,10156,10195,10234,10273,10312,10351, //  00 C
  10390,10429,10468,10507,10546,10585,10624,10663,10702,10740,
  10779,10818,10857,10896,10935,10973,11012,11051,11090,11128,
  11167,11206,11245,11283,11322,11361,11399,11438,11477,11515, 
  11554,11593,11631,11670,11708,11747,11785,11824,11862,11901, 
  11940};                                                      //  50 C

const int size = 251;

float gettemp(float rpt) {
  const float d = 10.0; // 10 for PT1000, 100 for PT100

  if (rpt < resistances[0] / d) return -199.99;
  if (rpt > resistances[size - 1] / d) return 49.99;
  
  int pos = 0;
  while (rpt > resistances[pos] / d) pos++;
  
  if (rpt == resistances[pos]) return pos - 200; // rare equality

  float r1 = resistances[pos-1] / d; // low value for resistance in table
  float r2 = resistances[pos] / d;   // high value for resistance in table
  int t1 = pos-1 - 200;              // low value for temperature in table
  int t2 = pos - 200;                // high value for temperature in table

  return t1 + (rpt - r1) / (r2-r1) * (t2-t1);
}

void setup() {
  Serial.begin(9600);

  Serial.println(numPTs);

  pinMode(digital_pin_0, OUTPUT);
  pinMode(digital_pin_1, OUTPUT);
  pinMode(digital_pin_2, OUTPUT);
  pinMode(digital_pin_3, OUTPUT);

  pinMode(heater_1_pin, OUTPUT);
  pinMode(heater_2_pin, OUTPUT);
}

void loop() {

  ptnum = activePTs[ptindex];
  
  digitalWrite(digital_pin_0, ptnum % 2);
  digitalWrite(digital_pin_1, (ptnum / 2) % 2);
  digitalWrite(digital_pin_2, (ptnum / 4) % 2);
  digitalWrite(digital_pin_3, (ptnum / 8) % 2);

  delay(delay_after_multiplexing);

  v = analogRead(analog_pin) * analog_to_voltage;
  v_amp = analogRead(analog_pin_amp) * analog_to_voltage;
  v_amp_converted = r1 / r2 * v_amp + vmax * roff2 / (roff1 + roff2);

  rpt = resistors[ptnum] * v / (vmax - v);
  temp = gettemp(rpt);

  rpt_amp = resistors[ptnum] * v_amp_converted / (vmax - v_amp_converted);
  temp_amp = gettemp(rpt_amp);

  /*
  Serial.print(ptnum);
  Serial.print(": ");
  Serial.print(v, 4);
  Serial.print("V, ");
  Serial.print(v_amp, 4);
  Serial.print("V, ");
  Serial.print(v_amp_converted, 4);
  Serial.print("V, ");
  Serial.print(rpt_amp);
  Serial.print("Ω, ");
  Serial.print(temp_amp, 4);
  Serial.print("C, ");
  Serial.print(temp_amp * 9. / 5. + 32., 4);
  Serial.println("F");
  */
  

  // this used for arduino serial plotter
  /*
  Serial.print(ptnum);
  Serial.print(":");
  Serial.print(temp, 4);
  Serial.print("   ");
  if (ptnum == numPTs - 1) {
    Serial.println("");
  }
  */
  
  Serial.print(ptnum);
  Serial.print(":");
  Serial.print(temp, 4);
  Serial.print("    ");
  Serial.print(ptnum);
  Serial.print("_amp:");
  Serial.print(temp_amp, 4);
  Serial.print("   ");
  if (ptindex == numPTs - 1) {
    Serial.println("");
  }

  // this used for labview
  /*
  Serial.print(ptnum);
  Serial.print(": ");
  Serial.print(temp);
  Serial.println("");
  */

  if (ptindex == numPTs - 1) {
    ptindex = 0;
  } else {
    ptindex++;
  }
  
  delay(delay_between_readings);
}
