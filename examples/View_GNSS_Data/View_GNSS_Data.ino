
//======================================================================================//
/**
 * @file View_GNSS_Data.ino
 * @brief Reads raw data from the GNSS module and prints it on the serial monitor.
 * @date +05:30 12:37:39 AM 03-08-2024, Saturday
 * @version 1.0.1
 * @author Vishnu Mohanan (@vishnumaiea)
 * @par GitHub Repository: https://github.com/CIRCUITSTATE/CSE_GNSS
 * @par MIT License
 * 
 */
//======================================================================================//

#include <Arduino.h>
#include <CSE_GNSS.h>

//======================================================================================//

#define   PORT_GPS_SERIAL         Serial1   // GPS serial port
#define   PORT_DEBUG_SERIAL       Serial    // Debug serial port

// For RP2040
#define   PIN_GPS_SERIAL_TX       0
#define   PIN_GPS_SERIAL_RX       1

// // For ESP32
// #define   PIN_GPS_SERIAL_TX       16
// #define   PIN_GPS_SERIAL_RX       17

#define   VAL_GPS_BAUDRATE        115200
#define   VAL_DEBUG_BAUDRATE      115200

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
  PORT_DEBUG_SERIAL.begin (VAL_DEBUG_BAUDRATE);

  // // For ESP32 boards
  // PORT_GPS_SERIAL.begin (VAL_GPS_BAUDRATE, SERIAL_8N1, PIN_GPS_SERIAL_RX, PIN_GPS_SERIAL_TX);

  // For RP2040
  PORT_GPS_SERIAL.setRX (PIN_GPS_SERIAL_RX);
  PORT_GPS_SERIAL.setTX (PIN_GPS_SERIAL_TX);
  PORT_GPS_SERIAL.begin (VAL_GPS_BAUDRATE, SERIAL_8N1);

  // // For other boards.
  // PORT_GPS_SERIAL.begin (VAL_GPS_BAUDRATE);
  
  GNSS_Module.begin();  // Initialize the GNSS module.

  PORT_DEBUG_SERIAL.println();
  PORT_DEBUG_SERIAL.println ("--- CSE_GNSS [View_GNSS_Data] ---");
  delay (1000);
}

//======================================================================================//
/**
 * @brief Runs indefinitely.
 * 
 */
void loop() {
  GNSS_Module.read (1024); // Read multiple NMEA data lines from the GNSS module

  // Print the data
  for (int i = 0; i < GNSS_Module.gnssDataBufferLength; i++) {
    PORT_DEBUG_SERIAL.print (GNSS_Module.gnssDataBuffer [i]);
  }

  PORT_DEBUG_SERIAL.println();

  delay (500);
}

//======================================================================================//

