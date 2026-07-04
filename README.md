# bluepad32-arduino — ESP-IDF Component

[![Apache 2.0 License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](LICENSE)

An **ESP-IDF component** that packages [Bluepad32](https://github.com/ricardoquesada/bluepad32) (multi-platform gamepad support) together with [Arduino-ESP32](https://github.com/espressif/arduino-esp32) framework bootstrapping, allowing you to write gamepad-enabled firmware using the familiar Arduino `setup()` / `loop()` pattern.

## Overview

This component handles the low-level integration between Bluepad32, BTstack, and the Arduino framework so your project only needs to provide `setup()` and `loop()` — no `app_main()` boilerplate required.

### Key features

- **Automatic Arduino bootstrapping** — `arduino_bootstrap()` creates a FreeRTOS task that calls your `setup()` and `loop()`, triggered automatically during `uni_init()`.
- **Default `app_main()` provided** — the component includes a weak `app_main()` that initializes BTstack, sets the Arduino platform, and runs Bluepad32. Your project can override it if needed.
- **Compile-time safety** — enforces `CONFIG_AUTOSTART_ARDUINO=n` with a clear `#error` if misconfigured.

## How it works

```text
app_main()                           [component_main.cpp — weak]
  ├─ btstack_init()
  ├─ uni_platform_set_custom()
  ├─ uni_init()
  │    └─ arduino_on_init_complete()
  │         └─ arduino_bootstrap()   [arduino_bootstrap.cpp]
  │              └─ FreeRTOS task → setup() / loop()
  └─ btstack_run_loop_execute()      [never returns]
```

## Usage

### 1. Add the component to your project

In your project's `main/idf_component.yml` (or `idf_component.yml` at the project root), add:

```yml
dependencies:
  Nojgaard/bluepad32-arduino:
    git: https://github.com/Nojgaard/bluepad32-arduino
```

This tells ESP-IDF's dependency manager to clone the component directly from GitHub. The component's own `idf_component.yml` will automatically pull in its sub-dependencies (`bluepad32`, `btstack`, and `espressif/arduino-esp32`).

### 2. Configure `sdkconfig.defaults`

```ini
# Required by bluepad32-arduino
CONFIG_AUTOSTART_ARDUINO=n

# Bluetooth
CONFIG_BT_ENABLED=y
CONFIG_BT_CONTROLLER_ONLY=y
CONFIG_BTDM_CTRL_MODE_BTDM=y
CONFIG_BTDM_CTRL_BR_EDR_MAX_ACL_CONN=5
CONFIG_BTDM_CTRL_BLE_MAX_CONN=5
CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN=0

# Bluepad32
CONFIG_BLUEPAD32_PLATFORM_CUSTOM=y
CONFIG_BLUEPAD32_MAX_DEVICES=4
CONFIG_BLUEPAD32_USB_CONSOLE_ENABLE=n
CONFIG_BLUEPAD32_ENABLE_VIRTUAL_DEVICE_BY_DEFAULT=n

# Arduino
CONFIG_AUTOSTART_ARDUINO=n
CONFIG_ARDUINO_SELECTIVE_COMPILATION=y
```

### 3. Write your sketch

Create `main/sketch.cpp` (or similar) in your project's main component:

```cpp
#include <Arduino.h>
#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            myControllers[i] = ctl;
            Serial.printf("Controller connected at index %d\n", i);
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            myControllers[i] = nullptr;
            Serial.printf("Controller disconnected from index %d\n", i);
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);
    BP32.setup(&onConnectedController, &onDisconnectedController, true);
    BP32.forgetBluetoothKeys();
}

void loop() {
    BP32.update();
    for (auto ctl : myControllers) {
        if (ctl && ctl->isConnected() && ctl->hasData()) {
            Serial.printf("dpad: 0x%02x, buttons: 0x%04x\n",
                          ctl->dpad(), ctl->buttons());
        }
    }
    delay(150);
}
```

### 4. Configure `main/CMakeLists.txt`

```cmake
idf_component_register(
    SRC_DIRS "."
    INCLUDE_DIRS "."
    REQUIRES "bluepad32-arduino")
```

That's it. The component's weak `app_main()` will be linked automatically.

> **Note:** If you need custom initialization (e.g., to configure hardware before Bluepad32 starts), simply provide your own `app_main()` in your main component — the weak symbol will be overridden.

## Example project

See the full working example in [`examples/basic_gamepad`](examples/basic_gamepad), which uses PlatformIO with the ESP-IDF framework:

```ini
[env:esp32dev]
platform = https://github.com/pioarduino/platform-espressif32/releases/download/55.03.39/platform-espressif32.zip
board = esp32dev
framework = espidf
```

## Acknowledgments

This component is inspired by and uses code from:

- **[esp-idf-arduino-bluepad32-template](https://github.com/ricardoquesada/esp-idf-arduino-bluepad32-template)** by [Ricardo Quesada](https://github.com/ricardoquesada) — the original template that brings together ESP-IDF, Arduino, and Bluepad32.
- **[Bluepad32](https://github.com/ricardoquesada/bluepad32)** — multi-platform gamepad library.
- **[BTstack](https://github.com/bluekitchen/btstack)** — Bluetooth stack.
- **[Arduino Core for ESP32](https://github.com/espressif/arduino-esp32)** — Arduino framework for ESP32.

## License

Licensed under the Apache License, Version 2.0. See [LICENSE](LICENSE) for details.

