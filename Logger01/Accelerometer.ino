// datasheet: http://www.analog.com/static/imported-files/data_sheets/ADXL345.pdf
// userguide: http://www.analog.com/static/imported-files/eval_boards/UG065_2-9-10.pdf

#include <stdio.h>
#include <SD.h>
#include <Wire.h> 		// I2C library, gyroscope

// Accelerometer ADXL345
#define ACC (0x53)		//ADXL345 ACC address
#define A_TO_READ (6)	//num of bytes we are going to read each time (two bytes for each axis)

char tempString[512];
float rawAcc[3];		// projection of normalized gravitation force vector on x/y/z axis, as measured by accelerometer

const unsigned int accelFlushInterval = 5000; // milliseconds
unsigned long nextAccelFlush = accelFlushInterval;

const unsigned int accelCheckInterval = 200; // milliseconds
unsigned long nextAccelCheck = 0;

char accelFilename[] = "00000000.imu";
static File accelFile;

void accelSetup() {
	Wire.begin();
	
	initAcc();

#ifdef SERIAL_ENABLED
	Serial.println("Accelerometer initialized.");
#endif
}

void accelLoop() {
	if (!gpsHasInitialized) {
		return;
	}
	
	if (!accHasInitialized) {
		accHasInitialized = true;
		
		sprintf(accelFilename, "%08lu.imu", gpsTime);
		accelFile = SD.open(accelFilename, FILE_WRITE);
		
#ifdef SERIAL_ENABLED      
      Serial.print("imuFile = ");
      Serial.println(accelFile);
			
			if (accelFile) {
				Serial.print("Accelerometer log file is ");
				Serial.println(accelFile ? "open." : "closed.");
			}
#endif
	}
	
	if (nextAccelFlush < millis()) {
#ifdef SERIAL_ENABLED
		Serial.println("Flushing Accelerometer SD file.");
#endif
		nextAccelFlush = millis() + accelFlushInterval;
		accelFile.flush();
	}

	if (nextAccelCheck > millis()) {
		return;
	}
	
	nextAccelCheck = millis() + accelCheckInterval;

	int acc[3];
	// int gyro[4];
	
	getAccelerometerData(acc);
	rawAccToG(acc, rawAcc);
			
	char output_acc0[64];
	char output_acc1[64];
	char output_acc2[64];
	
	dtostrf(rawAcc[0], 6, 2, output_acc0);
	dtostrf(rawAcc[1], 6, 2, output_acc1);
	dtostrf(rawAcc[2], 6, 2, output_acc2);

	sprintf(tempString, "%s:%s,%s,%s", gpsTime, output_acc0, output_acc1, output_acc2); 

	accelFile.print(tempString);

//#ifdef SERIAL_ENABLED	
//	Serial.println(tempString);
//#endif
}

void initAcc() {
  // Turning on the ADXL345
  writeTo(ACC, 0x2D, 0);
  writeTo(ACC, 0x2D, 16);
  writeTo(ACC, 0x2D, 8);
  
  // mikk: turn on 16g support
  writeTo(ACC, 0x31, 11);
  
  // mikk: for 16g set bits 0, 1, and 3 of register 0x31 to 1
}

void getAccelerometerData(int * result) {
  int regAddress = 0x32;	// first axis-acceleration-data register on the ADXL345
  byte buff[A_TO_READ];
  readFrom(ACC, regAddress, A_TO_READ, buff); // read the acceleration data from the ADXL345

  //each axis reading comes in 10 bit resolution, ie 2 bytes. Least Significat Byte first!!
  //thus we are converting both bytes in to one int
  result[0] = (((int)buff[1]) << 8) | buff[0];   
  result[1] = (((int)buff[3]) << 8) | buff[2];
  result[2] = (((int)buff[5]) << 8) | buff[4];
}

void rawAccToG(int * raw, float * rawAcc) {
	rawAcc[0] = ((float) raw[0]) / 256.0;
	rawAcc[1] = ((float) raw[1]) / 256.0;
	rawAcc[2] = ((float) raw[2]) / 256.0;
}

void serialPrintFloatArr(float * arr, int length) {
  for(int i=0; i<length; i++) {
    serialFloatPrint(arr[i]);
    Serial.print(",");
  }
}

void serialFloatPrint(float f) {
  byte * b = (byte *) &f;
  Serial.print("f:");
  for(int i=0; i<4; i++) {
    
    byte b1 = (b[i] >> 4) & 0x0f;
    byte b2 = (b[i] & 0x0f);
    
    char c1 = (b1 < 10) ? ('0' + b1) : 'A' + b1 - 10;
    char c2 = (b2 < 10) ? ('0' + b2) : 'A' + b2 - 10;
    
    Serial.print(c1);
    Serial.print(c2);
  }
}

//---------------- Functions
//Writes val to address register on ACC
void writeTo(int DEVICE, byte address, byte val) {
	Wire.beginTransmission(DEVICE);	// start transmission to ACC 
	Wire.write(address);        		// send register address
	Wire.write(val);        			// send value to write
	Wire.endTransmission(); 			// end transmission
}
	
	//reads num bytes starting from address register on ACC in to buff array
void readFrom(int DEVICE, byte address, int num, byte buff[]) {
	Wire.beginTransmission(DEVICE);	//start transmission to ACC 
	Wire.write(address);				//sends address to read from
	Wire.endTransmission();			//end transmission
	
	Wire.beginTransmission(DEVICE);	//start transmission to ACC
	Wire.requestFrom(DEVICE, num);	// request 6 bytes from ACC
	
	int i = 0;
	while(Wire.available()) {			//ACC may send less than requested (abnormal)
		buff[i] = Wire.read();			// receive a byte
		i++;
	}
	
	Wire.endTransmission();
}