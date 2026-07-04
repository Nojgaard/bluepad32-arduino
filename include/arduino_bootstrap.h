// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 Ricardo Quesada
// http://retro.moe/unijoysticle2

#ifndef ARDUINO_BOOTSTRAP_H
#define ARDUINO_BOOTSTRAP_H

#include "sdkconfig.h"

// bluepad32-arduino handles Arduino bootstrapping itself.
// CONFIG_AUTOSTART_ARDUINO must be disabled to use this component.
#if CONFIG_AUTOSTART_ARDUINO
#error "bluepad32-arduino requires CONFIG_AUTOSTART_ARDUINO to be disabled (set CONFIG_AUTOSTART_ARDUINO=n in sdkconfig)"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void arduino_bootstrap();

#ifdef __cplusplus
}
#endif

#endif  // ARDUINO_BOOTSTRAP_H
