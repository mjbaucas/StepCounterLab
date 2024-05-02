#include <Wire.h> // Library to interface with MPU-6050

const int mpu_addr = 0x68; // Address pointing to MPU-6050 module
float x_raw, y_raw, z_raw; // Accelerometer data
float a_avg; // Combined accelerometer data

float threshold = 1.2; // Value that defines what is considered a step 
float x_cal, y_cal, z_cal; // Callibrated accelerometer data
float x_avg, y_avg, z_avg; // Average values used for simplified filter
int step_count = 0; // Counter for the number of steps taken 
int flag = 0; // Checks whether signal goes under the threshold before crossing it again

void setup() {
  Wire.begin();
  Serial.begin(9600);

  // Initialize I2C communication between Arduino and MPU-6050
  Wire.beginTransmission(mpu_addr);
  Wire.write(0x6B); // Power Management Register (PWR_MGMT_1)
  Wire.write(0); // Set to Zero (Wake up MPU-6050)
  Wire.endTransmission(true);
  Wire.beginTransmission(mpu_addr);
  Wire.write(0x1C); // Accelerometer Sensitivity Register
  Wire.write(0x0); // Set to +/- 2g 
  Wire.endTransmission(true);

  // Calibrate Accelerometer
  calibrate_accel();
}

void loop() {
  int accel_addr = 0x3B; // Address of the starting register for accelerometer

  // Initialize which register to start reading from
  Wire.beginTransmission(mpu_addr);
  Wire.write(accel_addr);
  Wire.endTransmission(false);
  Wire.requestFrom(mpu_addr, 6, true);

  // Read each register one by one
  x_raw = (float) (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value (0x3B & 0x3C)
  y_raw = (float) (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value (0x3D & 0x3E)
  z_raw = (float) (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value (0x3F & 0x40)

  // For debugging purpose, to check if you're reading the right data
  //Serial.print(xAcc);
  //Serial.print(" ");
  //Serial.print(yAcc);
  //Serial.print(" ");
  //Serial.println(zAcc);  

  // Combine 3 dimensional accel values to one dimensional vector
  a_avg = sqrt(((x_raw - x_avg) * (x_raw - x_avg)) + ((y_raw - y_avg) * (y_raw - y_avg)) + ((z_raw - z_avg) * (z_raw - z_avg)));

  // Print combined vector to serial terminal
  //Serial.println(a_avg);
  //delay(100);

  // Step logic
  // Count as step if accelerometer value is above threshold and if crossing flag is off
  if (a_avg > threshold && flag == 0)
  {
    step_count = step_count + 1;
    flag = 1;
  }
  else if (a_avg > threshold && flag == 1)
  {
    // Do not count as step if value is above threshold but accelerometer value has not gone back to zero yet 
  }

  // If accelerometer value went back under the threshold, set turn the crossing flag off
  if (a_avg < 0.10 && flag == 1)
  {
    flag = 0;
  }

  // If at any point step count becomes negative, set it back to zero
  if (step_count < 0) {
    step_count = 0;
  }

  // Print step counter
  Serial.print("steps: ");
  Serial.println(step_count);
  delay(100);
}

void calibrate_accel()
{
  int accel_addr = 0x3B; // Address of the register
  float x_sum = 0;
  float y_sum = 0;
  float z_sum = 0;
  
  // Take 100 samples and calibrate each axis
  for (int i = 0; i < 100; i++) {
    // Initialize which register to start reading from
    Wire.beginTransmission(mpu_addr);
    Wire.write(accel_addr);
    Wire.endTransmission(false);
    Wire.requestFrom(mpu_addr, 6, true);

    // Read each register one by one
    x_cal = (float) (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value (0x3B & 0x3C)
    y_cal = (float) (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value (0x3D & 0x3E)
    z_cal = (float) (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value (0x3F & 0x40)

    // Get the sum of all samples
    x_sum = x_cal + x_sum;
    y_sum = y_cal + y_sum;
    z_sum = z_cal + z_sum;
  }

  // Take the average 
  x_avg = x_sum / 100.0;
  y_avg = y_sum / 100.0;
  z_avg = z_sum / 100.0;
  delay(100);

  // Print statements for debugging
  // Serial.println(xavg);
  // Serial.println(yavg);
  // Serial.println(zavg);
}
