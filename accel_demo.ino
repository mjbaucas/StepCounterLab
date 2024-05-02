#include <Wire.h> // Library to interface with MPU-6050

const int mpu_addr = 0x68; // Address pointing to MPU-6050 module
float x_raw, y_raw, z_raw; // Accelerometer data

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
  Serial.print(x_raw);
  Serial.print(" ");
  Serial.print(y_raw);
  Serial.print(" ");
  Serial.println(z_raw);   
  delay(100);
}
