
![CSE_GNSS](https://socialify.git.ci/CIRCUITSTATE/CSE_GNSS/image?description=1&font=KoHo&forks=1&issues=1&logo=https%3A%2F%2Fwww.circuitstate.com%2Fwp-content%2Fuploads%2F2024%2F05%2FCIRCUITSTATE-R-Emblem-20052024-2.svg&name=1&pattern=Circuit%20Board&pulls=1&stargazers=1&theme=Auto)

# CSE_GNSS

**CSE_GNSS** is a generic Arduino library from [*CIRCUITSTATE Electronics*](https://www.circuitstate.com/) to read and write any GPS/GNSS module that supports **NMEA** output over UART. You can define the NMEA sentence in any custom formats and extract the data coming from the GNSS module.

This library is in pre-release stage. Any feedback is highly appreciated.

# Installation

This library is available from the official **Arduino Library Manager**. Open the Arduino IDE, search for `CSE_GNSS` and install the latest version of the library.

Additionally, you can download the latest release package from the GitHub repository and install it manually. To do so, open the Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library…`​ and select the downloaded file.

Another method is to clone the GitHub repository directly into your libraries folder. The development branch will have the latest features, bug fixes and other changes. To do so, navigate to your `libraries` folder (usually located at `Documents/Arduino/libraries` on Windows and `~/Documents/Arduino/libraries` on macOS) and execute the following command:

```
git clone https://github.com/CIRCUITSTATE/CSE_GNSS.git
```

[**Git**](https://git-scm.com) should be installed on your computer.

The library can also be installed via [**PlatformIO**](https://platformio.org). All officially listed Arduino listed libraries are automatically fetched by PlatformIO. Use the [`lib_deps`](https://docs.platformio.org/en/latest/projectconf/sections/env/options/library/lib_deps.html) search option to install the library.

## Dependencies

This library depends on the following libraries:

# Examples

Two example sketches are included with this library which you can find inside the `examples` folder.

- [**Print_GPRMC**](/examples/Print_GPRMC/) - Reads the NMEA output from the GNSS module and extracts the GPRMC sentence and prints it on the serial monitor.
- [**View_GNSS_Data**](/examples/View_GNSS_Data/) - Directly reads raw NMEA output from the GNSS module and prints it to the serial monitor.

# Tutorial

A complete tutorial on GPS/GNSS is available on the CIRCUITSTATE website - [What is GPS/GNSS & How to Interface u-blox NEO-6M GPS Module with Arduino](https://www.circuitstate.com/tutorials/what-is-gps-gnss-how-to-interface-ublox-neo-6m-gps-module-with-arduino/). This tutorial uses the **u-blox NEO-6M GY-NEO6MV2** GPS module wired with a **FireBeetle-ESP32E** board.

# API Documentation

The API documentation is available at [API.md](/docs/API.md).

# References

1. [What is GPS/GNSS & How to Interface u-blox NEO-6M GPS Module with Arduino - CIRCUITSTATE Electronics](https://www.circuitstate.com/tutorials/what-is-gps-gnss-how-to-interface-ublox-neo-6m-gps-module-with-arduino/)

