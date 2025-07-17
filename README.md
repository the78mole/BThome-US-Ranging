# BThome-US-Ranging

BThome US ranging sensor e.g. for water or oil level measurments

NOTE: For the Packet ID and Firmware Version, the latest sources from the BTHomeV2-Arduino library [Pull Request #1](https://github.com/deeja/BTHomeV2-Arduino/pull/1) are required. The current version in PlatformIO is not sufficient. 

To make use of it, step into the `lib` directory and run:

```bash
cd lib
git clone git@github.com:the78mole/BTHomeV2-Arduino.git
git pull origin adding_fwversion_packetid_device_id
```

You can also find a story on the "why" on my [blog](https://the78mole.de/the-great-flood-how-i-turned-my-living-room-into-a-high-tech-sump-pit/).

![hardware-inserted-in-case](img/hardware-and-bottom-case.jpg)

You can easily insert a 50mm tube (HT DN50 in Germany), to not detect sourrounding edges. If you can not access DN50 tubes, you can easily print one (50mm outer and 45mm inner diameter) with a 3D printer. Drill a small hole in the upper part to relief air, when water raises...

![tube-extension](img/case-with-tube.jpg)

To make it work with Home Assistant, you need a bluetooth proxy somewhere around in range. This can be a newer Shelly Pro or Plus or an ESPhome device with [bluetooth proxy](https://esphome.io/components/bluetooth_proxy.html) enabled.

## Development Hardware

To develop this project, I used a [clone](https://mischianti.org/vcc-gnd-studio-yd-esp32-s3-devkitc-1-clone-high-resolution-pinout-and-specs/) of the [ESP32-S3-DevKitC-1](https://docs.espressif.com/projects/esp-idf/en/v4.4.3/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html).

Excessive Documentation is available on [michianti.org](https://mischianti.org/vcc-gnd-studio-yd-esp32-s3-devkitc-1-clone-high-resolution-pinout-and-specs/).

There is also a [Datasheet of the ESP32-S3-WROOM-1 Module](https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf) available. The [ESP32-S3 Technical Reference Manual](https://www.espressif.com/documentation/esp32-s3_technical_reference_manual_en.pdf) has detailed description of the SoC. The WROOM-variant on my DevKit is the N16R8, which incorporates 16MB of Flash and 8MB of PSRAM.

Its pinout is as follows:

![ESP32-S3-DevKitC-1 Pinout](img/esp32-s3-devkitc-1-pin-layout.png)
![ESP32-S3-DevKitC-1 Clone Pinout](img/esp32-s3-devkitc-1-clone-pinout.jpg)

There is also a schematic available for the [ESP32-S3-DevKitC-1](info/yd-eso32-s3-sch-v1.4.pdf).
