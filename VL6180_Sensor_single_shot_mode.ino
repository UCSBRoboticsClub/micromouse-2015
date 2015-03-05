
#include <Wire.h>

int address = 0x29;
int reading = 0;
int VL6180X_ADDRESS = address;

void writeByte (int address, unsigned int subAddress, unsigned char data)
{
	Wire.beginTransmission(address);      // Initialize the Tx buffer
	Wire.write((subAddress >> 8) & 0xFF); // Put MSB of 16-bit slave register address in Tx buffer
	Wire.write(subAddress & 0xFF);        // Put LSB of 16-bit slave register address in Tx buffer
	Wire.write(data);                     // Put data in Tx buffer
	Wire.endTransmission();               // Send the Tx buffer
}

void setup()
{
pinMode(13, OUTPUT); 
Serial.begin(9600);
Wire.begin();
digitalWrite(13,HIGH);
delay(500);
digitalWrite(13,LOW);
delay(500);
digitalWrite(13,HIGH);
delay(500);
digitalWrite(13,LOW);
delay(500);
digitalWrite(13,HIGH);
delay(500);

			writeByte(VL6180X_ADDRESS, 0x0207, 0x01);
			writeByte(VL6180X_ADDRESS, 0x0208, 0x01);
			writeByte(VL6180X_ADDRESS, 0x0096, 0x00);
			writeByte(VL6180X_ADDRESS, 0x0097, 0xFD);
			writeByte(VL6180X_ADDRESS, 0x00e3, 0x00);
			writeByte(VL6180X_ADDRESS, 0x00e4, 0x04);
			writeByte(VL6180X_ADDRESS, 0x00e5, 0x02);
			writeByte(VL6180X_ADDRESS, 0x00e6, 0x01);
			writeByte(VL6180X_ADDRESS, 0x00e7, 0x03);
			writeByte(VL6180X_ADDRESS, 0x00f5, 0x02);
			writeByte(VL6180X_ADDRESS, 0x00d9, 0x05);
			writeByte(VL6180X_ADDRESS, 0x00db, 0xce);
			writeByte(VL6180X_ADDRESS, 0x00dc, 0x03);
			writeByte(VL6180X_ADDRESS, 0x00dd, 0xf8);
			writeByte(VL6180X_ADDRESS, 0x009f, 0x00);
			writeByte(VL6180X_ADDRESS, 0x00a3, 0x3c);
			writeByte(VL6180X_ADDRESS, 0x00b7, 0x00);
			writeByte(VL6180X_ADDRESS, 0x00bb, 0x3c);
			writeByte(VL6180X_ADDRESS, 0x00b2, 0x09);
			writeByte(VL6180X_ADDRESS, 0x00ca, 0x09);
			writeByte(VL6180X_ADDRESS, 0x0198, 0x01);
			writeByte(VL6180X_ADDRESS, 0x01b0, 0x17);
			writeByte(VL6180X_ADDRESS, 0x01ad, 0x00);
			writeByte(VL6180X_ADDRESS, 0x00ff, 0x05);
			writeByte(VL6180X_ADDRESS, 0x0100, 0x05);
			writeByte(VL6180X_ADDRESS, 0x0199, 0x05);
			writeByte(VL6180X_ADDRESS, 0x01a6, 0x1b);
			writeByte(VL6180X_ADDRESS, 0x01ac, 0x3e);
			writeByte(VL6180X_ADDRESS, 0x01a7, 0x1f);
			writeByte(VL6180X_ADDRESS, 0x0030, 0x00);

}


void loop()
{
 //delay(250);                  // wait a bit since people have to read the output :)
 //Serial.println(1000000000);
  
  Wire.beginTransmission(address); // starts a reading
  Wire.write(byte(0x00)); 
  Wire.write(byte(0x18)); // 16 bit register address      
  Wire.write(byte(0x01)); // singleshot mode 
  
  Wire.endTransmission();      

  //wait for readings to happen
  delay(70);
  
  Wire.beginTransmission(address); // send pointer to range result register
  Wire.write(byte(0x00));
  Wire.write(byte(0x62));      
  Wire.endTransmission();    

  Wire.requestFrom(address, 1);    // request 1 byte

  if(Wire.available())    // if byte is receieved
  {
    reading = Wire.read();  // receive byte
    Serial.println(reading);   // print the reading
  }
//  Wire.beginTransmission(address); // send pointer to range result register
//  Wire.write(byte(0x00));
//  Wire.write(byte(0x1A));      
//  Wire.endTransmission();    
//
//  Wire.requestFrom(address, 1);    // request 1 byte
//
//  if(Wire.available())    // if byte is receieved
//  {
//    reading = Wire.read();  // receive byte
//    Serial.println(reading);   // print the reading
//  }

//  Wire.beginTransmission(address); // send pointer to range error codes register
//  Wire.write(byte(0x00));
//  Wire.write(byte(0x4D));
//  Wire.endTransmission();
//
//
//  Wire.requestFrom(address, 1);    // request 1 byte
//
//  if(Wire.available())    // if byte is receieved
//  {
//    reading = Wire.read();  // receive byte
//    Serial.println(reading);   // print the reading
//  }
  
  Wire.beginTransmission(address); // resets interrupt status flags
  Wire.write(byte(0x00));
  Wire.write(byte(0x15));
  Wire.write(byte(0x00));
  Wire.endTransmission(); 
  
}
