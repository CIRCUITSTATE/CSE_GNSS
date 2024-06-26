
//======================================================================================//
/**
 * @file View_GNSS_Data.ino
 * @brief Reads the NMEA output from the GNSS module, extract the data and prints them on
 * the serial monitor as key-value pairs.
 * @date +05:30 09:26:31 PM 30-03-2024, Saturday
 * @version 0.1.2
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
  String GNSS_Data = GNSS_Module.read ("$GPRMC"); // Read multiple NMEA data lines from the GNSS module and print them directly.
  delay (10);
}

//======================================================================================//

