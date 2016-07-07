These service programs help to init the sensor.
InitEeprom  - writes initial values that allow reading raw data from the sensor.
Calibration - calculates transformation matrix for Accelerometer and Gyroscope using 6-point calibration algorithm
CalibrationGyroOffset - calculates gyroscope offset and writes it into EEPROM. The resulting transformation matrix 
              performs only offet data correction