
//======================================================================================//
/**
 * @file CSE_GNSS.h
 * @brief Main header file for CSE_GNSS library.
 * @date +05:30 04:34:13 PM 10-07-2023, Monday
 * @version 0.1.0
 * @author Vishnu Mohanan (@vishnumaiea)
 * @par GitHub Repository: https://github.com/CIRCUITSTATE/CSE_GNSS
 * @par MIT License
 * 
 */
//======================================================================================//

#ifndef CSE_GNSS_H
#define CSE_GNSS_H

#include <Arduino.h>
#include <vector>

#if defined(__AVR__) || defined(ESP8266)
  #include <SoftwareSerial.h> //software serial doesn't work with Arduino Due
#endif

//======================================================================================//
// Forward declarations.

class CSE_GNSS;

//======================================================================================//
/**
 * @brief A class to read, extract and print NMEA 0183 data.
 * 
 */
class NMEA_0183_Data {
  public:
    CSE_GNSS* GNSS_Parent; // The parent GNSS object

    String name; // NMEA sentence type/name
    String description; // NMEA sentence description
    String sentence; // Comma separated NMEA sentence
    String sample; // NMEA sentence sample
    int dataCount; // Number of data in the NMEA sentence, including the header and separate checksum
    const static int dataMax = 64; // Maximum number of data in the NMEA sentence
    String dataList [dataMax]; // NMEA sentence data
    String dataNameList [dataMax]; // NMEA sentence data name

    NMEA_0183_Data (String name, String description, int dataCount, String dataNames[], String sample);
    bool parse(); // Parse the NMEA sentence
    bool print(); // Print the NMEA sentence
    bool set (String line); // Set the NMEA sentence
    bool check (String line); // Check if the NMEA sentence is valid
    bool find (String lines, int position = 0); // Find the NMEA sentence in the lines
    int count (String lines); // Count the number of particular NMEA sentence in the lines
    int getDataIndex (String dataName); // Get the index of the dataName
};

//======================================================================================//

class CSE_GNSS {
  private:
    friend class NMEA_0183_Data;

    HardwareSerial* Debug_Serial; // The serial port of the debug monitor.

    #if defined(__AVR__) || defined(ESP8266)
      SoftwareSerial* GNSS_Serial; // The serial port of the GNSS module.
    #else
      HardwareSerial* GNSS_Serial; // The serial port of the GNSS module.
    #endif

    uint64_t gnssBaud; // The baudrate of the GNSS module.
    uint64_t debugBaud; // The baudrate of the GNSS module.
    bool inited;  // True if the GNSS module serial port is initialized.
    std::vector <NMEA_0183_Data*> dataList;  // List of NMEA data objects.
    int dataCount; // The number of NMEA data objects in the dataList.
  
  public:
    typedef NMEA_0183_Data& NMEA_0183_Data_Ref;

    NMEA_0183_Data* dummyData; // A dummy NMEA data object to return if the requested data is not found.

    // Constructor using two hardware serial ports.
    CSE_GNSS (HardwareSerial* gnssSerial, HardwareSerial* debugSerial, uint64_t gnssBaud = 0, uint64_t debugBaud = 0);

    // Constructor using one hardware serial port and one software serial port.
    #if defined(__AVR__) || defined(ESP8266)
      CSE_GNSS (SoftwareSerial* gnssSerial, uint64_t gnssBaud, HardwareSerial* debugSerial, uint64_t debugBaud);
    #endif

    bool begin();
    String read (String header = "$GPRMC");
    String read (int lineCount);
    int addData (NMEA_0183_Data* data);
    int getDataCount();
    NMEA_0183_Data_Ref getDataRef (String name);
    NMEA_0183_Data_Ref getDataRef (int index);
};

//======================================================================================//

#endif // CSE_GNSS_H