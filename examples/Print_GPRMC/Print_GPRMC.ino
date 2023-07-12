
//======================================================================================//
/**
 * @file Print_GPRMC.ino
 * @brief Reads the NMEA output from the GNSS module and prints it on the serial monitor.
 * @date +05:30 11:24:49 PM 12-07-2023, Wednesday
 * @version 0.1.0
 * @author Vishnu Mohanan (@vishnumaiea)
 * @par GitHub Repository: https://github.com/CIRCUITSTATE/CSE_GNSS
 * @par MIT License
 * 
 */
//======================================================================================//

#include <Arduino.h>
#include <CSE_GNSS.h>

//======================================================================================//

#define   PORT_GPS_SERIAL       Serial1 // GPS serial port
#define   PORT_DEBUG_SERIAL     Serial // Debug serial port

//======================================================================================//
// Forward declarations

void setup();
void loop();

//======================================================================================//

// Set the serial ports and the baudrate for the GNSS module.
// Both ports have to be manually initialized through begin() call.
CSE_GNSS GNSS_Module (&PORT_GPS_SERIAL, &PORT_DEBUG_SERIAL);

// GPRMC (No Second Mode indicator)
String GPRMC_Sample = "$GPRMC,165729.00,A,0902.595184,N,07632.860862,E,0.0,,031022,1.9,W,A*00";
String GPRMC_Data_Names [] = {"Header", "UTC", "Status", "Latitude", "Latitude Direction", "Longitude", "Longitude Direction", "Speed", "Course", "Date", "Mag Variation", "Mag Variation Direction", "Mode", "Checksum"};
String GPRMC_Description = "Recommended Minimum Specific GNSS Data";
NMEA_0183_Data NMEA_GPRMC ("GPRMC", GPRMC_Description, 14, GPRMC_Data_Names, GPRMC_Sample);

//======================================================================================//
/**
 * @brief Setup the serial ports and pins.
 * 
 */
void setup() {
  PORT_DEBUG_SERIAL.begin (115200);

  // For ESP32 boards
  PORT_GPS_SERIAL.begin (9600, SERIAL_8N1, 17, 16);

  // // For other boards.
  // PORT_GPS_SERIAL.begin (9600);
  
  GNSS_Module.begin();
  GNSS_Module.addData (&NMEA_GPRMC);

  PORT_DEBUG_SERIAL.println();
  PORT_DEBUG_SERIAL.println ("--- CSE_GNSS View_GNSS_Data ---");
  delay (1000);
}

//======================================================================================//
/**
 * @brief Runs indefinitely.
 * 
 */
void loop() {
  String GNSS_Data = GNSS_Module.read ("$GPRMC");
  GNSS_Module.getDataRef ("GPRMC").find (GNSS_Data, 0);
  GNSS_Module.getDataRef ("GPRMC").print();
  delay (10);
}

//======================================================================================//

