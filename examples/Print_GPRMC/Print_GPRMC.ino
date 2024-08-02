
//======================================================================================//
/**
 * @file Print_GPRMC.ino
 * @brief Reads the NMEA output from the GNSS module and prints it on the serial monitor.
 * @date +05:30 12:28:39 AM 03-08-2024, Saturday
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

// Following is how we define the format of the GPRMC header (No Second Mode indicator).
String GPRMC_Sample = "$GPRMC,165729.00,A,0902.595184,N,07632.860862,E,0.0,,031022,1.9,W,A*00"; // A sample reference line
String GPRMC_Data_Names [] = {"Header", "UTC", "Status", "Latitude", "Latitude Direction", "Longitude", "Longitude Direction", "Speed", "Course", "Date", "Mag Variation", "Mag Variation Direction", "Mode", "Checksum"}; // The palce keys
String GPRMC_Description = "Recommended Minimum Specific GNSS Data"; // A human readable description of the data.

// Format: Name, Description, Data Count, Data Names, Sample
NMEA_0183_Data NMEA_GPRMC ("GPRMC", GPRMC_Description, 14, GPRMC_Data_Names, GPRMC_Sample); // An object to save and handle the data.

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
  GNSS_Module.addData (&NMEA_GPRMC); // Add the data object to the GNSS module.

  PORT_DEBUG_SERIAL.println();
  PORT_DEBUG_SERIAL.println ("--- CSE_GNSS [Print_GPRMC] ---");
  delay (1000);
}

//======================================================================================//
/**
 * @brief Runs indefinitely.
 * 
 */
void loop() {
  GNSS_Module.read (1024);
  GNSS_Module.extractNMEA();
  String GNSS_Data = GNSS_Module.getNmeaDataString();

  GNSS_Module.getDataRef ("GPRMC").find (GNSS_Data); // Find the GPRMC sentences in the read data
  GNSS_Module.getDataRef ("GPRMC").print(); // Print the GNRMC sentences in preformatted format
  delay (10);
}

//======================================================================================//

