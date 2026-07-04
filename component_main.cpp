// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Bluepad32 Project
//
// This file provides a default app_main() for projects using the
// bluepad32-arduino component. It bootstraps BTstack and Bluepad32.
//
// Projects that need custom initialization can override this by
// defining their own app_main() in their main component, since
// this implementation uses weak linkage.

#include "sdkconfig.h"

#include <stddef.h>

// BTstack related
#include <btstack_port_esp32.h>
#include <btstack_run_loop.h>
#include <btstack_stdio_esp32.h>

// Bluepad32 related
#include <arduino_platform.h>
#include <uni.h>

// Weak linkage so projects can override this with their own app_main()
extern "C" __attribute__((weak)) int app_main(void) {
    // Don't use BTstack buffered UART. It conflicts with the console.
#ifndef CONFIG_ESP_CONSOLE_UART_NONE
#ifndef CONFIG_BLUEPAD32_USB_CONSOLE_ENABLE
    btstack_stdio_init();
#endif  // CONFIG_BLUEPAD32_USB_CONSOLE_ENABLE
#endif  // CONFIG_ESP_CONSOLE_UART_NONE

    // Configure BTstack for ESP32 VHCI Controller
    btstack_init();

    // Must be called before uni_init()
    uni_platform_set_custom(get_arduino_platform());

    // Init Bluepad32.
    // This triggers arduino_on_init_complete() which calls
    // arduino_bootstrap(), spawning setup()/loop() as a task.
    uni_init(0 /* argc */, NULL /* argv */);

    // Does not return.
    btstack_run_loop_execute();
    return 0;
}
