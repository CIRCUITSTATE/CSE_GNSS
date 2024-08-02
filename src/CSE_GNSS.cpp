
//======================================================================================//
/**
 * @file CSE_GNSS.cpp
 * @brief Main source file for CSE_GNSS library.
 * @date +05:30 12:27:56 AM 03-08-2024, Saturday
 * @version 1.0.1
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
 * @param dataCount The number of data fields in the NMEA sentence.
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
 * @param line A valid NMEA sentence. Not checked for validity.
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
 * @brief Find the particular NMEA sentence from the given lines. The lines have to 
 * separated by a newline character. Some NMEA sentences can have multiple lines.
 * In that case, you can specify which occurrence of the NMEA sentence you want to find.
 * If the occurrence is not specified, the first occurrence will be returned.
 * 
 * @param lines The lines as a String separated by a newline character.
 * @param occurrence The occurrence of the given NMEA line to read. Default is 1 (1st).
 * @return true Data was successfully parsed and saved.
 * @return false No valid line was found.
 */
bool NMEA_0183_Data:: find (String lines, int occurrence) {
  if ((occurrence < 1) || (occurrence > (CONST_MAX_NMEA_LINES_COUNT - 1))) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): Invalid position.");
    return false;
  }

  String splittedLines [CONST_MAX_NMEA_LINES_COUNT] = {""};
  int startIndex = 0;
  int lineCount = 0;  // Total number of lines.
  int occurrenceCount = 0; // The number of instances/occurrence of the given NMEA type in the lines.
  int occurrenceIndex = -1; // The index of the given NMEA type in the lines array.

  // Split the lines to an array.
  for (int i = 0; i < lines.length(); i++) {
    if ((lines.charAt (i) == '\n') || (i == (lines.length() - 1))) {
      if (lineCount >= CONST_MAX_NMEA_LINES_COUNT) {
        break;
      }
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
      occurrenceCount += 1;
    }

    // Break if we find the line at the same occurrence we want.
    if (occurrenceCount == occurrence) {
      occurrenceIndex = i;
      break;
    }
  }

  // Return false if no valid line was found.
  if (occurrenceCount == -1) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): No valid lines found.");
    return false;
  }

  // If the occurrenceCount is not -1, but less than the position, that means there was at least one
  // line, but not enough to reach the position. For example if you wanted to get the 2nd
  // GPGSV message in the lines, but there was only one, then the occurrenceCount would be 0.
  if (occurrenceCount < occurrence) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): Not enough lines to find the required occurence.");
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): occurrenceCount: " + String (occurrenceCount));
    return false;
  }

  // If the occurrenceCount is equal to the occurrence, then we have found the line we want.
  if (occurrenceCount == occurrence) {
    GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data find(): Found " + name + " line at position " + String (occurrenceIndex));
    set (splittedLines [occurrenceIndex]);
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
  String splittedLines [CONST_MAX_NMEA_LINES_COUNT] = {""};
  int startIndex = 0;
  int lineCount = 0;
  int instanceCount = 0;

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
      instanceCount += 1;
    }
  }

  GNSS_Parent->Debug_Serial->println ("NMEA_0183_Data count(): Found " + String (instanceCount) + " " + name + " lines.");

  return instanceCount;
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

  // Add data to the dummy data object.
  static String NMEA_Sample = "$DUMMY,120556.096,V,123.456,N,123.456,E,1.23,123.45,020723,1.9,W,N,V*33";
  static String NMEA_Data_Names [] = {"Header", "UTC", "Status", "Latitude", "Latitude Direction", "Longitude", "Longitude Direction", "Speed", "Course", "Date", "Mag Variation", "Mag Variation Direction", "Mode", "Second Mode", "Checksum"};
  static String NMEA_Description = "You forgot to add NMEA_0183_Data objects. Please add at least one.";
  dummyData = new NMEA_0183_Data ("DUMMY", NMEA_Description, 15, NMEA_Data_Names, NMEA_Sample);
  dummyData->GNSS_Parent = this;
}

//======================================================================================//

// Only for software serial.
#if SOFTWARE_SERIAL_REQUIRED
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

  // Add data to the dummy data object.
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
 * By default the port is not initialized as the default value of gnssBaud and debugBaud is 0.
 * The serial ports will only be initialized if a non-zero value is set for the baudrates.
 * 
 * Regardless of how you initialize the serial port, it is important to call this function
 * to let the library know that the port was initialized.
 * 
 * @return true GNSS initialization was successful.
 * @return false GNSS initialization failed.
 */
bool CSE_GNSS:: begin() {
  if (!inited) {
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
 * @brief Reads a specified number of bytes from the GNSS module. The read bytes will be
 * saved to the `gnssDataBuffer`. `gnssDataBufferLength` will hold the number of valid
 * data bytes in the buffer. Only that much data must be read for further processing.
 * Returns the number of bytes read at the end.
 * 
 * @param byteCount The number of bytes to read.
 * @return uint16_t The number of bytes read.
 */
uint16_t CSE_GNSS:: read (int byteCount) {
  if (!inited) {
    Debug_Serial->println ("CSE_GNSS read(): GNSS module serial port is not initialized.");
    return 0;
  }

  if ((byteCount <= 0) || (byteCount > (CONST_SERIAL_BUFFER_LENGTH - 1))) { // Check if the byte count is valid.
    Debug_Serial->println ("CSE_GNSS read(): Invalid byte count.");
    return 0;
  }

  gnssDataBufferLength = GNSS_Serial->readBytes (gnssDataBuffer, byteCount); // Read the bytes from the serial port.
  // gnssDataBuffer [byteCount] = 0; // Null terminate the buffer so that it becomes a proper c-string (if used for character communication).

  Debug_Serial->print ("CSE_GNSS read(): Read ");
  Debug_Serial->print (gnssDataBufferLength);
  Debug_Serial->println (" bytes from GNSS module.");

  // return String (gnssDataBuffer, gnssDataBufferLength);
  return gnssDataBufferLength;
}


//======================================================================================//
/**
 * @brief Converts the contents of the `nmeaDataBuffer` to a String object.
 * 
 * @return String The converted string.
 */
String CSE_GNSS:: getNmeaDataString() {
  // nmeaDataBuffer [nmeaDataBufferLength] = 0;
  return String (nmeaDataBuffer, nmeaDataBufferLength);
}

//======================================================================================//
/**
 * @brief Extracts NMEA sentences from the GNSS data buffer. This removes any redundant
 * or unsupported data from the buffer. This can include mixed protocol messages, non-printable
 * chracters, etc. Valid NMEA sentences will be read and added to the buffer with each line
 * ending a single newline character (LF).
 * 
 * @return uint16_t The number of valid bytes in the NMEA data buffer.
 */
uint16_t CSE_GNSS:: extractNMEA() {
  bool dollarFound = false;
  bool gFound = false;
  bool starFound = false;
  bool c1Found = false;
  bool c2Found = false;

  Debug_Serial->print ("CSE_GNSS extractNMEA(): Extracting NMEA lines.. GNSS data buffer length is ");
  Debug_Serial->println (gnssDataBufferLength);
  
  // The following algorithm works for mixed protocol messages.
  // It can split mixed NMEA + UBX protocol messages correctly.
  int destIndex = 0;

  for (int sourceIndex = 0; sourceIndex < (gnssDataBufferLength - 1);) {
    char c = gnssDataBuffer [sourceIndex];

    if (c == '$') { // Check for the Dollar sign
      sourceIndex++;
      if (sourceIndex < gnssDataBufferLength) {
        c = gnssDataBuffer [sourceIndex]; // Read the next character

        if (c == 'G') { // Check if the next character is G
          if (!dollarFound) { // If a dollar sign is not already found
            nmeaDataBuffer [destIndex] = gnssDataBuffer [sourceIndex - 1];  // Save the dollar sign
            destIndex++;
            dollarFound = true;
            gFound = true;
            nmeaDataBuffer [destIndex] = c;  // Save the G character
            destIndex++;
            sourceIndex++;
          }
          else { // If we find multiple dollar signs before finding a star and other positions.
            dollarFound = false;  // So that a fresh line can be created.
            gFound = false;
            nmeaDataBuffer [destIndex] = '\n';  // Break the line but do not increment the source line index.
            destIndex++;
          }
          continue; // Skip the remaining steps.
        }
      }
      continue;
    }

    if ((c == 'G') && (!gFound) && (dollarFound)) {
      gFound = true;
      nmeaDataBuffer [destIndex] = c;
      destIndex++;
      sourceIndex++;
      continue;
    }

    if ((c == '*') && (!starFound) && (gFound)) {
      starFound = true;
      nmeaDataBuffer [destIndex] = c;
      destIndex++;
      sourceIndex++;
      continue;
    }

    if ((!c1Found) && (starFound)) {
      c1Found = true;
      nmeaDataBuffer [destIndex] = c;
      destIndex++;
      sourceIndex++;
      continue;
    }

    if ((!c2Found) && (c1Found)) {
      c2Found = true;
      nmeaDataBuffer [destIndex] = c;
      destIndex++;
      sourceIndex++;
      continue;
    }

    if (c2Found) {
      nmeaDataBuffer [destIndex] = '\n';
      destIndex++;
      dollarFound = false;
      gFound = false;
      starFound = false;
      c1Found = false;
      c2Found = false;
    }

    if (gFound && dollarFound) {
      nmeaDataBuffer [destIndex] = c;
      destIndex++;
    }

    sourceIndex++;
  }

  nmeaDataBufferLength = destIndex;
  
  // If the last character is not a new line character, add it.
  if (nmeaDataBuffer [nmeaDataBufferLength] != '\n') {
    nmeaDataBuffer [nmeaDataBufferLength] = '\n';
    nmeaDataBufferLength++;
  }

  Debug_Serial->println ("CSE_GNSS extractNMEA(): Removing CR character..");

  // Check if there are <CR> characters in the buffer and replace them with <LF> characters.
  for (int i = 0; i < nmeaDataBufferLength; i++) {
    if (nmeaDataBuffer [i] == '\r') {
      nmeaDataBuffer [i] = '\n';
    }
  }

  Debug_Serial->print ("CSE_GNSS extractNMEA(): Extracted ");
  Debug_Serial->print (nmeaDataBufferLength);
  Debug_Serial->println (" characters.");

  // for (int i = 0; i < nmeaDataBufferLength; i++) {
  //   if (nmeaDataBuffer [i] == '\n') {
  //     Debug_Serial->println ("<LF>");
  //   }
  //   else if (nmeaDataBuffer [i] == '\r') {
  //     Debug_Serial->print ("<CR>");
  //   }
  //   else {
  //     Debug_Serial->print (nmeaDataBuffer [i]);
  //   }
  // }

  Debug_Serial->println ("CSE_GNSS extractNMEA(): Removing non-printable characters..");

  char tempBuffer [nmeaDataBufferLength] = {0};

  // Check if each chracters is a printable ASCII character and only copy the ones that are.
  for (int i = 0, k = 0; i < nmeaDataBufferLength; i++) {
    if (((nmeaDataBuffer [i] > 0x20) && (nmeaDataBuffer [i] <= 0x7E)) || (nmeaDataBuffer [i] == '\n') || (nmeaDataBuffer [i] == '\r')) {
      tempBuffer [k] = nmeaDataBuffer [i];
      // Debug_Serial->print (tempBuffer [k]);
      k++;
    }
    if (i == (nmeaDataBufferLength - 1)) {
      nmeaDataBufferLength = k;
    }
  }

  // Copy the temporary buffer to the original buffer.
  for (int i = 0; i < nmeaDataBufferLength; i++) {
    nmeaDataBuffer [i] = tempBuffer [i];
  }

  Debug_Serial->print ("CSE_GNSS extractNMEA(): Found ");
  Debug_Serial->print (nmeaDataBufferLength);
  Debug_Serial->println (" valid characters.");

  for (int i = 0; i < nmeaDataBufferLength; i++) {
    if (nmeaDataBuffer [i] == '\n') {
      Debug_Serial->println ("<LF>");
    }
    else if (nmeaDataBuffer [i] == '\r') {
      Debug_Serial->print ("<CR>");
    }
    else {
      Debug_Serial->print (nmeaDataBuffer [i]);
    }
  }

  Debug_Serial->println();

  return nmeaDataBufferLength;
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
 * data with the given name is found, a dummy data is returned.
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

  return *dummyData;
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

  return *dummyData;
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
