
//======================================================================================//
/**
 * @file CSE_GNSS.cpp
 * @brief Main source file for CSE_GNSS library.
 * @date +05:30 04:34:13 PM 10-07-2023, Monday
 * @version 0.1.0
 * @author Vishnu Mohanan (@vishnumaiea)
 * @par GitHub Repository: https://github.com/CIRCUITSTATE/CSE_GNSS
 * @par MIT License
 * 
 */
//======================================================================================//

#include "CSE_GNSS.h"

//======================================================================================//
/**
 * @brief NMEA_0183_Data constructor.
 * 
 * @param name The name of the NMEA sentence.eg. "GPRMC".
 * @param dataCount The number of data in the NMEA sentence.
 * @param sample A sample string of the NMEA sentence.
 * @return NMEA_0183_Data:: 
 */
NMEA_0183_Data:: NMEA_0183_Data (String name, String description, int dataCount, String dataNames[], String sample) : 
  name (name),
  description (description), 
  sample (sample),
  dataCount (dataCount) {
    for (int i = 0; i < dataMax; i++) {
      dataList [i] = "";
      dataNameList [i] = "";
    }

    for (int i = 0; i < dataCount; i++) {
      dataNameList [i] = dataNames [i];
    }
}

//======================================================================================//
/**
 * @brief Parse/extract the data from the NMEA sentence. The sentence has to be set first
 * with the set() function.
 * 
 * @return true If parsing was successful.
 * @return false If parsing was not successful.
 */
bool NMEA_0183_Data:: parse() {
  // Check if the NMEA sentence is valid.
  if (check (sentence) == false) {
    GNSS_Parent->Debug_Serial->print ("NMEA_0183_Data parse(): ");
    GNSS_Parent->Debug_Serial->println ("Invalid NMEA sentence.");
    return false;
  }
  
  GNSS_Parent->Debug_Serial->print ("NMEA_0183_Data parse(): ");
  GNSS_Parent->Debug_Serial->println (sentence);

  int startIndex = 0;
  int dataIndex = 0;

  while (startIndex < sentence.length()) {
    int commaIndex = sentence.indexOf (',', startIndex);
    int asteriskIndex = sentence.indexOf ('*', startIndex);

    if ((commaIndex == -1) && (asteriskIndex == -1)) {  // no more commas or asterisks found
      dataList [dataIndex++] = sentence.substring (startIndex);
      startIndex = sentence.length();  // stop the loop
    }
    else if ((commaIndex != -1) && ((commaIndex < asteriskIndex) || (asteriskIndex == -1))) {
      dataList [dataIndex++] = sentence.substring (startIndex, commaIndex);
      startIndex = commaIndex + 1;  // move past the comma
    }
    else if ((asteriskIndex != -1) && ((asteriskIndex < commaIndex) || (commaIndex == -1))) {
      // Extract the data before the asterisk
      dataList [dataIndex++] = sentence.substring (startIndex, asteriskIndex);
      // Extract the checksum after the asterisk
      dataList [dataIndex++] = sentence.substring (asteriskIndex + 1);
      startIndex = sentence.length();  // stop the loop
    }
  }

  GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data parse(): Completed.");
  return true;
}

//======================================================================================//
/**
 * @brief Set the NMEA sentence.
 * 
 * @param line A valid NMEA sentence.
 * @return true If setting was successful.
 * @return false If setting was not successful.
 */
bool NMEA_0183_Data:: set (String line) {
  GNSS_Parent->Debug_Serial->print ("NMEA_0183_Data set(): ");
  GNSS_Parent->Debug_Serial->println (line);
  sentence = line;
  return true;
}

//======================================================================================//
/**
 * @brief Prints the GNSS data saved in the object.
 * 
 * @return true 
 * @return false 
 */
bool NMEA_0183_Data:: print() {
  GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data print(): ");

  GNSS_Parent->Debug_Serial->print (name);
  GNSS_Parent->Debug_Serial->print (": ");
  GNSS_Parent->Debug_Serial->println (description);
  GNSS_Parent->Debug_Serial->print ("  ");

  for (int i = 0; i < dataCount; i++) {
    GNSS_Parent->Debug_Serial->print (dataNameList [i]);
    GNSS_Parent->Debug_Serial->print (": ");
    GNSS_Parent->Debug_Serial->println (dataList [i]);
    GNSS_Parent->Debug_Serial->print ("  ");
  }
  GNSS_Parent->Debug_Serial->println();
  return true;
}

//======================================================================================//
/**
 * @brief Checks the given line if it is a valid NMEA sentence.
 * 
 * @param line A line to check.
 * @return true Line is a valid NMEA sentence.
 * @return false Line is not a valid NMEA sentence.
 */
bool NMEA_0183_Data:: check (String line) {
  GNSS_Parent->Debug_Serial->print ("NMEA_0183_Data check(): ");
  GNSS_Parent->Debug_Serial->println (line);

  // Check if the NMEA header is valid.
  if ((!line.startsWith (name) && (!line.startsWith (String ("$") + name)))) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data check(): Invalid header.");
    return false;
  }

  // Check if the comma count is valid.
  int commaCount = 0;

  for (int i = 0; i < line.length(); i++) {
    if (line.charAt (i) == ',') {
      commaCount++;
    }
  }

  // Return false if the comma count is not valid.
  // The last datum of the NMEA sentence is a checksum and a parameter combined with an asterisk.
  // So the total comma count will be 2 less.
  if (commaCount != (dataCount - 2)) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data check(): Invalid comma count.");
    return false;
  }

  // Check if the a valid checksum is present.
  if (line.indexOf ('*') == -1) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data check(): No checksum found.");
    return false;
  }

  // Return true if the NMEA sentence is valid.
  GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data check(): Valid " + name + " sentence.");
  return true;
}

//======================================================================================//
/**
 * @brief Find the NMEA sentence in the given lines. Only the first found NMEA of the given
 * type is read. If your GNSS module outputs 10 lines every second, then you should at least
 * read 11 line so that you get all complete lines at least once. Do not remove the newline
 * character from the lines. After finding a valid line, the data is extracted and saved in
 * the object.
 * 
 * The position of the given NMEA line can be used to specify which line to read if there are
 * multiple consecutive lines of the same type. This is useful for reading lines like GPGSA.
 * 
 * @param lines The lines as a String.
 * @param position The position of the given NMEA line to read. Default is 0.
 * @return true Data was successfully parsed and saved.
 * @return false No valid line was found.
 */
bool NMEA_0183_Data:: find (String lines, int position) {
  if ((position < 0) || (position > 63)) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): Invalid position.");
    return false;
  }

  String splittedLines [64] = {""};
  int startIndex = 0;
  int lineCount = 0;  // Total number of lines.
  int instanceCount = -1; // The number of instances of the given NMEA type.
  int instanceIndex = -1; // The index of the given NMEA type in the lines array.

  // Split the lines to an array.
  for (int i = 0; i < lines.length(); i++) {
    if ((lines.charAt (i) == '\n') || (i == (lines.length() - 1))) {
      // The LF character will be dropped here.
      splittedLines [lineCount] = lines.substring (startIndex, i);
      startIndex = i + 1;
      lineCount++;
    }
  }

  // // Print the string array with special characters converted to <CR> and <LF>.
  // GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): splittedLines = ");
  // for (int i = 0; i < lineCount; i++) {
  //   for (int j = 0; j < splittedLines [i].length(); j++) {
  //     if (splittedLines [i].charAt (j) == '\r') {
  //       GNSS_Parent->Debug_Serial->print ("<CR>");
  //     }
  //     else if (splittedLines [i].charAt (j) == '\n') {
  //       GNSS_Parent->Debug_Serial->print ("<LF>");
  //     }
  //     else {
  //       GNSS_Parent->Debug_Serial->print (splittedLines [i].charAt (j));
  //     }
  //   }
  //   GNSS_Parent->Debug_Serial->println();
  // }

  // for (int i = 0; i < lineCount; i++) {
  //   GNSS_Parent->Debug_Serial->println (splittedLines [i]);
  // }

  GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): Lines to search = " + String (lineCount));

  // Check if any lines starts with the given NMEA ID.
  for (int i = 0; i < lineCount; i++) {
    if ((splittedLines [i].startsWith (name)) || (splittedLines [i].startsWith (String ("$") + name))) {
      instanceCount += 1;
    }

    // Break if we find the line at the position we want.
    if (instanceCount == position) {
      instanceIndex = i;
      break;
    }
  }

  // Return false if no valid line was found.
  if (instanceCount == -1) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): No valid line found.");
    return false;
  }

  // If the instanceCount is not -1, but less than the position, that means there was at least one
  // line, but not enough to reach the position. For example if you wanted to get the 2nd
  // GPGSV message in the lines, but there was only one, then the instanceCount would be 0.
  if (instanceCount < position) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): Not enough lines to reach the position.");
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): instanceCount: " + String (instanceCount));
    return false;
  }

  // If the instanceCount is equal to the position, then we have found the line we want.
  if (instanceCount == position) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): Found " + name + " line at position " + String (instanceCount));
    set (splittedLines [instanceIndex]);
    return parse();
  }

  return false;
}

//======================================================================================//
/**
 * @brief Counts the number of particular NMEA sentences in the given lines.
 * 
 * @param lines GNSS data lines separated by newline characters.
 * @return int The number of lines that starts with the given NMEA ID.
 */
int NMEA_0183_Data:: count (String lines) {
  String splittedLines [64] = {""};
  int startIndex = 0;
  int lineCount = 0;
  int instanceCountCount = 0;

  // Split the lines to an array.
  for (int i = 0; i < lines.length(); i++) {
    if ((lines.charAt (i) == '\n') || (i == (lines.length() - 1))) {
      // The LF character will be dropped here.
      splittedLines [lineCount] = lines.substring (startIndex, i);
      startIndex = i + 1;
      lineCount++;
    }
  }

  // Check if any lines starts with the given NMEA ID.
  for (int i = 0; i < lineCount; i++) {
    if ((splittedLines [i].startsWith (name)) || (splittedLines [i].startsWith (String ("$") + name))) {
      instanceCountCount += 1;
    }
  }

  GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data count(): Found " + String (instanceCountCount) + " " + name + " lines.");

  return instanceCountCount;
}

//======================================================================================//
/**
 * @brief Checks if a data named "dataName" exists in the dataName array and returns
 * the index if it does. Useful for updating the data. You must make sure the returned
 * index is valid and not -1, before trying to use it.
 * 
 * @param dataName The name of the data to search for. Eg. "Header".
 * @return int The index position. -1 if not found.
 */
int NMEA_0183_Data:: getDataIndex (String dataName) {
  for (int i = 0; i < dataCount; i++) {
    if (dataNameList [i] == dataName) {
      return i;
    }
  }

  return -1;
}

//======================================================================================//
/**
 * @brief Construct a new CSE_GNSS object with hardware serial ports.The baudrates are
 * optional. The ports will be initialized only if a non-zero baudrate is provided.
 * So for manual port initialization, you can provide 0 for the baudrates.
 * 
 * @param gnssSerial Hardare serial port for the GNSS module.
 * @param debugSerial Hardware serial port for debug messages.
 * @param gnssBaud Baude rate for the GNSS module. Optional. Default is 0.
 * @param debugBaud Baud rate for debug messages. Optional. Default is 0.
 * @return CSE_GNSS:: 
 */
CSE_GNSS:: CSE_GNSS (HardwareSerial* gnssSerial, HardwareSerial* debugSerial, uint64_t gnssBaud, uint64_t debugBaud) {
  GNSS_Serial = gnssSerial;
  Debug_Serial = debugSerial;
  this->gnssBaud = gnssBaud;
  this->debugBaud = debugBaud;

  static String NMEA_Sample = "$DUMMY,120556.096,V,123.456,N,123.456,E,1.23,123.45,020723,1.9,W,N,V*33";
  static String NMEA_Data_Names [] = {"Header", "UTC", "Status", "Latitude", "Latitude Direction", "Longitude", "Longitude Direction", "Speed", "Course", "Date", "Mag Variation", "Mag Variation Direction", "Mode", "Second Mode", "Checksum"};
  static String NMEA_Description = "You forgot to add NMEA_0183_Data objects. Please add at least one.";
  dummyData = new NMEA_0183_Data ("DUMMY", NMEA_Description, 15, NMEA_Data_Names, NMEA_Sample);
  dummyData->GNSS_Parent = this;
}

//======================================================================================//

// Only for software serial.
#if defined(__AVR__) || defined(ESP8266)
  /**
   * @brief Construct a new CSE_GNSS object with software serial port for GNSS module.
   * Debug serial port is still hardware serial. The baudrates are optional. The ports
   * will be initialized only if a non-zero baudrate is provided. So for manual port
   * initialization, you can provide 0 for the baudrates.
   * 
   * @param gnssSerial Software serial port for GNSS module.
   * @param debugSerial Hardware serial port for debug messages.
   * @param gnssBaud Baudrate for GNSS module. Optional. Default is 0.
   * @param debugBaud Debug port baudrate. Optional. Default is 0.
   * @return CSE_GNSS:: 
   */
  CSE_GNSS:: CSE_GNSS (SoftwareSerial* gnssSerial, HardwareSerial* debugSerial, uint64_t gnssBaud, uint64_t debugBaud) {
  GNSS_Serial = gnssSerial;
  Debug_Serial = debugSerial;
  this->gnssBaud = gnssBaud;
  this->debugBaud = debugBaud;

  static String NMEA_Sample = "$DUMMY,120556.096,V,123.456,N,123.456,E,1.23,123.45,020723,1.9,W,N,V*33";
  static String NMEA_Data_Names [] = {"Header", "UTC", "Status", "Latitude", "Latitude Direction", "Longitude", "Longitude Direction", "Speed", "Course", "Date", "Mag Variation", "Mag Variation Direction", "Mode", "Second Mode", "Checksum"};
  static String NMEA_Description = "You forgot to add NMEA_0183_Data objects. Please add at least one.";
  dummyData = new NMEA_0183_Data ("DUMMY", NMEA_Description, 15, NMEA_Data_Names, NMEA_Sample);
  dummyData->GNSS_Parent = this;
}
#endif

//======================================================================================//
/**
 * @brief Initializes the GNSS module serial port. You should have already set the baudrate
 * and the serial port before calling this function.
 * 
 * @return true GNSS initialization was successful.
 * @return false GNSS initialization failed.
 */
bool CSE_GNSS:: begin() {
  if (!inited) {
    // By default, the baudrates will be 0. If they are not 0, then we will initialize the serial ports.
    // So if you supply a valid baudrate, the serial port will be initialized here.
    // Otherwise, you can initialize the serial ports yourself.
    if (gnssBaud != 0) {
      GNSS_Serial->begin (gnssBaud);
    }

    if (debugBaud != 0) {
      Debug_Serial->begin (debugBaud);
    }
  }
  inited = true;
  return true;
}

//======================================================================================//
/**
 * @brief Reads a complete set of NMEA data lines from the GNSS module.
 * 
 * @return String The data read from the GNSS module, separated by newline characters.
 */
String CSE_GNSS:: read (String header) {
  if (!inited) {
    Debug_Serial->println ("CSE_GNSS read(): GNSS module serial port is not initialized.");
    return "";
  }

  String dataLines = "";
  bool readComplete = false;
  int lineCount = 0;

  // Read the specified number of lines.
  while (!readComplete) {
    if (GNSS_Serial->available() > 0) {
      // NMEA lines end with '\r\n'. The readStringUntil() will include the '\r' character,
      // but not the '\n' character.
      String line = GNSS_Serial->readStringUntil ('\n');

      // Find and save the first GPRMC line.
      if ((lineCount == 0) && (line.startsWith (header))) {
        // Replace the last character ('\r') with a newline character.
        line.setCharAt (line.length() - 1, '\n');
        dataLines += (line); // Add the line to the data.
        lineCount++;
      }

      // Stop reading after we find a second instance of GPRMC line.
      else if ((lineCount > 0) && (line.startsWith (header))) {
        readComplete = true;
      }

      // Read and save all other lines until the next GPRMC line.
      else if (lineCount > 0) {
        // Replace the last character ('\r') with a newline character.
        line.setCharAt (line.length() - 1, '\n');
        dataLines += (line); // Add the line to the data.
        lineCount++;
      }
    }
    delay (10);
  }

  // Remove any incomplete lines that do not start with '$'.
  while (!dataLines.startsWith ("$")) {
    int index = dataLines.indexOf ('\n');
    dataLines = dataLines.substring (index + 1);
  }

  Debug_Serial->println ("CSE_GNSS read(): Read " + String (lineCount) + " lines.");

  Debug_Serial->println ("CSE_GNSS read(): GNSS Data: ");

  for (int j = 0; j < dataLines.length(); j++) {
    if (dataLines.charAt (j) == '\r') {
      Debug_Serial->print ("<CR>");
    }
    else if (dataLines.charAt (j) == '\n') {
      Debug_Serial->print ("<LF>\n");
    }
    else {
      Debug_Serial->print (dataLines.charAt (j));
    }
  }

  Debug_Serial->println();

  return dataLines;
}

//======================================================================================//
/**
 * @brief Read the specified number of lines from the GNSS module serial port.
 * 
 * @param lineCount The number of lines to read.
 * @return String The data read from the GNSS module serial port, separated by newline.
 */
String CSE_GNSS:: read (int lineCount) {
  if (!inited) {
    Debug_Serial->println ("CSE_GNSS getData(): GNSS module serial port is not initialized.");
    return "";
  }

  String dataLines = "";

  // Read the specified number of lines.
  while (lineCount > 0) {
    if (GNSS_Serial->available() > 0) {
      // NMEA lines end with '\r\n'. The readStringUntil() will include the '\r' character,
      // but not the '\n' character.
      String line = GNSS_Serial->readStringUntil ('\n');

      // Replace the last character ('\r') with a newline character.
      line.setCharAt (line.length() - 1, '\n');
      dataLines += (line); // Add the line to the data.
      lineCount--;
    }
    delay (10);
  }

  // Remove any incomplete lines that do not start with '$'.
  while (!dataLines.startsWith ("$")) {
    int index = dataLines.indexOf ('\n');
    dataLines = dataLines.substring (index + 1);
  }

  // Debug_Serial->println ("GNSS Data: ");

  // for (int j = 0; j < dataLines.length(); j++) {
  //   if (dataLines.charAt (j) == '\r') {
  //     Debug_Serial->print ("<CR>");
  //   }
  //   else if (dataLines.charAt (j) == '\n') {
  //     Debug_Serial->print ("<LF>\n");
  //   }
  //   else {
  //     Debug_Serial->print (dataLines.charAt (j));
  //   }
  // }

  return dataLines;
}

//======================================================================================//
/**
 * @brief Add a new NMEA_0183_Data object to the data list on the fly and returns the
 * number of data objects in the list currently.
 * 
 * @param data The reference of the NMEA_0183_Data object to be added.
 * @return int The number of data objects in the list currently.
 */
int CSE_GNSS:: addData (NMEA_0183_Data* data) {
  dataList.push_back (data);
  dataList [dataCount]->GNSS_Parent = this;  // Set parent GNSS object.
  dataCount++;
  return dataCount;
}

//======================================================================================//
/**
 * @brief Returns the reference to the NMEA_0183_Data object with the given name. If no
 * data with the given name is found, an exception is thrown.
 * 
 * @param name The name of the NMEA_0183_Data object. For example, "GPRMC".
 * @return CSE_GNSS::NMEA_0183_Data_Ref 
 */
CSE_GNSS::NMEA_0183_Data_Ref CSE_GNSS:: getDataRef (String name) {
  if (dataCount == 0) {
    return *dummyData;
  }

  for (int i = 0; i < dataCount; i++) {
    if (dataList [i]->name == name) {
      return *dataList [i];
    }
  }
}

//======================================================================================//
/**
 * @brief Returns the reference to the NMEA_0183_Data object at the given index. The index
 * should be from 0 to dataCount - 1.
 * 
 * @param index The index of the NMEA_0183_Data object in the list. Index starts at 0.
 * @return CSE_GNSS::NMEA_0183_Data_Ref 
 */
CSE_GNSS::NMEA_0183_Data_Ref CSE_GNSS:: getDataRef (int index) {
  if (dataCount == 0) {
    return *dummyData;
  }

  if ((index >= 0) && (index < dataCount)) {
    return *dataList [index];
  }
}

//======================================================================================//
/**
 * @brief Returns the number of NMEA_0183_Data objects in the list.
 * 
 * @return int 
 */
int CSE_GNSS:: getDataCount() {
  return dataCount;
}

//======================================================================================//
