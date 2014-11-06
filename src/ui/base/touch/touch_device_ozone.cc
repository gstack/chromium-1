// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/touch/touch_device.h"

namespace ui {

bool IsTouchDevicePresent() {
  // TODO(sadrul@chromium.org): Support evdev hotplugging.
  return true;
}

int MaxTouchPoints() {
  return kMaxTouchPointsUnknown;
}

}  // namespace ui
