
//======================================================================================//
/**
 * @file CSE_GNSS.h
 * @brief Main header file for CSE_GNSS library.
 * @date +05:30 12:27:46 AM 03-08-2024, Saturday
 * @version 1.0.1
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

// You can expand the software serial support here.
#define SOFTWARE_SERIAL_REQUIRED defined(__AVR__) || defined(ESP8266)

#if SOFTWARE_SERIAL_REQUIRED
  #include <SoftwareSerial.h> //software serial doesn't work with Arduino Due
#endif

#define   CONST_SERIAL_BUFFER_LENGTH     4096   // The buffer size for GNSS serial data and NMEA data.
#define   CONST_MAX_NMEA_LINES_COUNT     64     // The maximum number of NMEA lines that will be scanned to find an occurrence.
#define   CONST_MAX_NMEA_FIELDS_COUNT    64     // The maximum number of fields count in a NMEA sentence.

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
    String description; // User-readable NMEA sentence description
    String sentence; // Comma separated NMEA sentence
    String sample; // NMEA sentence sample
    int dataCount; // Number of data in the NMEA sentence, including the header and separate checksum
    const static int dataMax = CONST_MAX_NMEA_FIELDS_COUNT; // Maximum number of data fields in the NMEA sentence
    String dataList [dataMax]; // NMEA data fields as an array of strings
    String dataNameList [dataMax]; // NMEA data field names as an array of strings

    NMEA_0183_Data (String name, String description, int dataCount, String dataNames[], String sample);
    bool parse(); // Parse the NMEA sentence
    bool print(); // Print the NMEA sentence
    bool set (String line); // Set the NMEA sentence
    bool check (String line); // Check if the NMEA sentence is valid
    bool find (String lines, int occurrence = 1); // Find the NMEA sentence in the lines
    int count (String lines); // Count the number of particular NMEA sentence in the lines
    int getDataIndex (String dataName); // Get the index of the data field name
};

//======================================================================================//
/**
 * @brief A generic class to read and write GNSS modules with serial interface. Supports
 * hardware serial for debug messages and either hardware/software serial for the GNSS.
 * 
 */
class CSE_GNSS {
  private:
    friend class NMEA_0183_Data;

    HardwareSerial* Debug_Serial; // The serial port of the debug monitor.

    #if SOFTWARE_SERIAL_REQUIRED
      SoftwareSerial* GNSS_Serial; // The serial port of the GNSS module.
    #else
      HardwareSerial* GNSS_Serial; // The serial port of the GNSS module.
    #endif

    uint64_t gnssBaud; // The baudrate of the GNSS module.
    uint64_t debugBaud; // The baudrate of the debug port.
    bool inited;  // True if the GNSS module serial port is initialized.
    std::vector <NMEA_0183_Data*> dataList;  // List of NMEA data objects.
    int dataCount; // The number of NMEA data objects in the dataList.


  public:
    typedef NMEA_0183_Data& NMEA_0183_Data_Ref;

    NMEA_0183_Data* dummyData; // A dummy NMEA data object to return if the requested data is not found.

    char gnssDataBuffer [CONST_SERIAL_BUFFER_LENGTH] = {0}; // A buffer to store the GNSS serial data.
    char nmeaDataBuffer [CONST_SERIAL_BUFFER_LENGTH] = {0}; // A buffer to store the NMEA data.

    uint16_t gnssDataBufferLength = 0;  // Indicates how many valid bytes are in the GNSS serial buffer.
    uint16_t nmeaDataBufferLength = 0; // Indicates how many valid lines are in the NMEA data buffer.

    // Constructor using two hardware serial ports.
    CSE_GNSS (HardwareSerial* gnssSerial, HardwareSerial* debugSerial, uint64_t gnssBaud = 0, uint64_t debugBaud = 0);

    // Constructor using one hardware serial port and one software serial port.
    #if SOFTWARE_SERIAL_REQUIRED
      CSE_GNSS (SoftwareSerial* gnssSerial, HardwareSerial* debugSerial, uint64_t gnssBaud = 0, uint64_t debugBaud = 0);
    #endif

    bool begin(); // Initialize the serial ports if necessary.
    uint16_t read (int byteCount);  // Read a specified number of bytes from the GNSS serial port.
    uint16_t extractNMEA(); // Extract NMEA data from the GNSS serial buffer. This will remove any redundant or unsupported data.
    String getNmeaDataString(); // Converts the NMEA data lines buffer to a Arduino String.

    int addData (NMEA_0183_Data* data); // Add an NMEA data object to the dataList.
    int getDataCount(); // Get the number of NMEA data objects in the dataList.
    NMEA_0183_Data_Ref getDataRef (String name);  // Get the reference of the NMEA data object.
    NMEA_0183_Data_Ref getDataRef (int index);  // Get the reference of the NMEA data object.
};

//======================================================================================//

#endif // CSE_GNSS_H