# CSE_GNSS Library API Reference

Version 0.1.2, +05:30 12:51:09 AM 31-03-2024, Sunday

## Index

- [CSE\_GNSS Library API Reference](#cse_gnss-library-api-reference)
  - [Index](#index)
  - [Macros](#macros)
  - [Classes](#classes)
  - [Class `NMEA_0183_Data`](#class-nmea_0183_data)
    - [Member Variables](#member-variables)
    - [`NMEA_0183_Data()`](#nmea_0183_data)
    - [`parse()`](#parse)
    - [`print()`](#print)
    - [`set()`](#set)
    - [`check()`](#check)
    - [`find()`](#find)
    - [`count()`](#count)
    - [`getDataIndex()`](#getdataindex)
  - [Class `CSE_GNSS`](#class-cse_gnss)
    - [Member Variables](#member-variables-1)
    - [Types](#types)
    - [`CSE_GNSS()`](#cse_gnss)
    - [`begin()`](#begin)
    - [`read()`](#read)
    - [`addData()`](#adddata)
    - [`getDataCount()`](#getdatacount)
    - [`getDataRef()`](#getdataref)


## Macros

`SOFTWARE_SERIAL_REQUIRED` - Determines if the software serial port should be enabled based on the architecture of the microcontroller. Currently, AVR and ESP8266 support software serial. If the `SoftwareSerial` is required, the `SoftwareSerial.h` library is included automatically.

## Classes

* `NMEA_0183_Data` - A class to read, extract and print NMEA 0183 data lines.
* `CSE_GNSS` - A generic class to read and write GNSS modules with serial interface. Supports hardware serial for debug messages and either hardware/software serial for the GNSS.

## Class `NMEA_0183_Data`

A generic class to read and write GNSS modules with serial interface. Supports hardware serial for debug messages and either hardware/software serial for the GNSS.

### Member Variables

`CSE_GNSS* GNSS_Parent` - This is a pointer to the `CSE_GNSS` object that allows access to the GNSS module.

`String name` - User-readable NMEA sentence name. This is useful when printing the NMEA sentence for debugging purposes.

`String description` - User-readable NMEA sentence description. This is useful when printing the NMEA sentence for debugging purposes.

`String sentence` - This is where the actual data line will be saved after reading from the GNSS module.


`String sample` - A complete sample string of the NMEA sentence. This can be useful for debugging and testing the parser.

`int dataCount` - The number of datum in the sentence. This should be a fixed value, unless you know what you're doing. Check the datasheet of your GNSS module to determine this value.

`const static int dataMax = 64` - The maximum number of data fields in the NMEA sentence. This is fixed at arbitrary value of 64. But you can always change this.

`String dataList [dataMax]` - An array to hold the NMEA datum.

`String dataNameList [dataMax]` - An array to hold the NMEA datum names.

### `NMEA_0183_Data()`

This constructor creates a new `NMEA_0183_Data` object. The function accepts the basic parameters required for an NMEA sentence. We will use the GPRMC sentence for examples throughout this document, using an example object `NMEA_GPRMC`.

#### Syntax

```cpp
NMEA_0183_Data NMEA_GPRMC;
```

##### Parameters

* `name` : The name of the NMEA sentence in string format. eg. "GPRMC".
* `description` : The description of the NMEA sentence inn string format.
* `dataCount` : The number of data in the NMEA sentence.
* `dataNames` : An array of list of data names in the exact sequence as they appear in the sentence.
* `sample` : A sample string of the NMEA sentence.

##### Returns

`NMEA_0183_Data` object

### `parse()`

Parse/extract the data from the NMEA sentence. The sentence has to be set first with the `set()` function. The parsed data is saved to the data array.

#### Syntax

```cpp
NMEA_GPRMC.parse();
```

##### Parameters

None

##### Returns

* _`bool`_ :
  * `true` if the parsing is successful.
  * `false` if the source sentence is empty of missing the specified header.

### `print()`

Prints the NMEA data saved in the data array. You can call this function, for example, after calling the `parse()` function successfully.

#### Syntax

```cpp
NMEA_GPRMC.print();
```

##### Parameters

None

##### Returns

* _`bool`_ :
  * `true` always.

### `set()`

Sets a string as the NMEA data sentence. This string will be used for, for example, parsing the data. The data is not validated in any manner. Use the `check()` function for that after setting the sentence.

#### Syntax

```cpp
NMEA_GPRMC.set (String line);
```

##### Parameters

* `line` : The NMEA data sentence.

##### Returns

* _`bool`_ :
  * `true` always.

### `check()`

Checks the sentence set by the `set()` function. The checking involves checking the header, counting the commas, and checking the checksum.

#### Syntax

```cpp
NMEA_GPRMC.check (String line);
```

##### Parameters

* `line` : A string to check.

##### Returns

* _`bool`_ :
  * `true` if the given string is a valid NMEA sentence of the specified type.
  * `false` if the string is empty or otherwise.

### `find()`

Find the specified type of NMEA sentence from the given lines. The lines have to separated by a newline character. Some NMEA sentences can have multiple lines. In that case, you can specify which occurrence of the NMEA sentence you want to find. If the occurrence is not specified, the first occurrence will be saved. The valid line is saved to the `sentence` variable using the `set()` function.

#### Syntax

```cpp
NMEA_GPRMC.find (String lines, int occurrence);
```

##### Parameters

* `lines` : The lines as a String separated by a newline character.
* `occurrence` : The occurrence of the given NMEA line to read. Default is 0.

##### Returns

* _`bool`_ :
  * `true` if valid sentence has been found and saved.
  * `false` otherwise.

### `count()`

Counts the number of the specified NMEA sentences in the given lines.

#### Syntax

```cpp
NMEA_GPRMC.count (String lines);
```

##### Parameters

* `lines` : The lines as a String separated by a newline character.

##### Returns

* _`int`_ : The number of occurrences of the specified NMEA sentence in the set of lines.

### `getDataIndex()`

Checks if the specified data name exists in the `dataName` array and returns the index if it does. Useful for updating the data. You must make sure the returned index is valid and not `-1`, before trying to use it.

#### Syntax

```cpp
NMEA_GPRMC.getDataIndex (String dataName);
```

##### Parameters

* `dataName` : The name of the data to search for. eg. "Header".

##### Returns

* _`int`_ : The index position. `-1` if not found.

## Class `CSE_GNSS`

A generic class to read and write GNSS modules with a serial interface. Supports hardware serial for debug messages and either hardware/software serial for the GNSS.

### Member Variables

* `HardwareSerial* gnssSerial` : A pointer to the hardware serial port for the GNSS.
* `SoftwareSerial* debugSerial` : A pointer to the hardware serial port for the debug messages. Optional for devices without a second hardware serial port.
* `HardwareSerial* debugSerial` : A pointer to the hardware serial port for the debug messages.
* `uint64_t gnssBaud` : The baud rate of the GNSS port.
* `uint64_t debugBaud` : The baud rate of the debug port.
* `bool inited` : True if the GNSS module serial port is initialized.
* `std::vector <NMEA_0183_Data*> dataList` : List of NMEA data objects.
* `int dataCount` : The number of NMEA data objects in the `dataList`.

* `NMEA_0183_Data* dummyData` : A dummy NMEA data object to return if the requested data is not found.

### Types

* `typedef NMEA_0183_Data& NMEA_0183_Data_Ref` : A reference to the NMEA_0183_Data object. Useful for functions that returns a reference to the object.

### `CSE_GNSS()`

Instantiates a new `CSE_GNSS` object. There are two overloads of this function.

#### Syntax 1

This accepts a hardware serial (UART) port of `HardwareSerial` type.

```cpp
CSE_GNSS (HardwareSerial* gnssSerial, HardwareSerial* debugSerial, uint64_t gnssBaud = 0, uint64_t debugBaud = 0);
```

##### Parameters

* `gnssSerial` : A pointer to the hardware serial port for the GNSS.
* `debugSerial` : A pointer to the hardware serial port for the debug messages.
* `gnssBaud` : The baud rate of the GNSS port. Default is 0 which doesn't initialize the port. You have to manually initialize the serial port in that case.
* `debugBaud` : The baud rate of the debug port. Default is 0 which doesn't initialize the port. You have to manually initialize the serial port in that case.

##### Returns

`CSE_GNSS` object. An example object `GNSS_Module` will be used in this documentation.

#### Syntax 2

This accepts a software serial port of `SoftwareSerial` type for the GNSS module. It still needs a hardware serial port for debugging which most microcontrollers provide. This function will only be enabled for devices that don't have a second hardware serial port. This is determined by checking the `SOFTWARE_SERIAL_REQUIRED` macro which you may use to expand the compatibility.

```cpp
CSE_GNSS (SoftwareSerial* gnssSerial, HardwareSerial* debugSerial, uint64_t gnssBaud = 0, uint64_t debugBaud = 0);
```

##### Parameters

* `gnssSerial` : A pointer to the software serial port for the GNSS.
* `debugSerial` : A pointer to the hardware serial port for the debug messages.
* `gnssBaud` : The baud rate of the GNSS port. Default is 0 which doesn't initialize the port. You have to manually initialize the serial port in that case.
* `debugBaud` : The baud rate of the debug port. Default is 0 which doesn't initialize the port. You have to manually initialize the serial port in that case.

##### Returns

`CSE_GNSS` object. An example object `GNSS_Module` will be used in this documentation.

### `begin()`

Initializes the `CSE_GNSS` object type. It initializes the GNSS serial port and the debug serial port if they are already not initialized. If the baud rates are 0, the ports are not initialized. If the ports are initialized, the `inited` variable will be set to `true`.

#### Syntax

```cpp
GNSS_Module.begin();
```

##### Parameters

None

##### Returns

* _`bool`_ :
  * `true` if the serial ports were initialized successfully.
  * `false` otherwise.

### `read()`

Read the NMEA data from the GNSS module. There are two overloads of this function.

#### Syntax 1

This accepts an NMEA header name, for example "$GPRMC", and reads a complete set of lines from and between the next occurrence of the same header. The lines will be separated by a newline character.

```cpp
GNSS_Module.read (String header);
```

##### Parameters

* `header` : The NMEA header name, for example "$GPRMC". Default is "$GPRMC".

##### Returns

* _`String`_ : The read string.

#### Syntax 2

This accepts the number of lines you want to read from the GNSS module. The lines will be separated by a newline character.

```cpp
GNSS_Module.read (int lineCount);
```

##### Parameters

* `lineCount` : The number of lines to read.

##### Returns

* _`String`_ : The read string.

### `addData()`

Add a new `NMEA_0183_Data` object to the data list on the fly and returns the number of data objects in the list currently.

#### Syntax

```cpp
GNSS_Module.addData (NMEA_0183_Data* data);
```

##### Parameters

* `data` : A pointer to the `NMEA_0183_Data` object.

##### Returns

* _`int`_ : The number of data objects in the list currently.

### `getDataCount()`

Get the number of data objects in the data list.

#### Syntax

```cpp
GNSS_Module.getDataCount ();
```

##### Parameters

None

##### Returns

* _`int`_ : The number of data objects in the list currently.

### `getDataRef()`

Returns the reference to the `NMEA_0183_Data` object with the given name. If no data with the given name is found, a dummy data is returned.

There are two overloads of this function.

#### Syntax 1

This accepts the name of the sentence type, for example "GPRMC".

```cpp
GNSS_Module.getDataRef (String name);
```

##### Parameters

* `name` : The NMEA header name, for example "GPRMC".

##### Returns

* _`NMEA_0183_Data_Ref`_ : The reference to the `NMEA_0183_Data` object with the given name.

#### Syntax 2

Returns the reference to the `NMEA_0183_Data` object at the given index. The index should be from 0 to `dataCount` - 1.

```cpp
GNSS_Module.getDataRef (int index);
```

##### Parameters

* `index` : The index of the `NMEA_0183_Data` object.

##### Returns

* _`NMEA_0183_Data_Ref`_ : The reference to the `NMEA_0183_Data` object at the given index.
