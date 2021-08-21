/*
 Copyright 2021 Peter Harrison - Helicron

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      https://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include <Arduino.h>
#include <BasicEncoder.h>
#include <TimerOne.h>

BasicEncoder encoder(2, 3);

/****************************************************************/
void timer_service() {
  encoder.service();
}

void setup() {
  Serial.begin(115200);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timer_service);
  encoder.set_reverse();
}

void loop() {
  int encoder_change = encoder.get_change();
  if (encoder_change) {
    Serial.println(encoder.get_count());
  }
  
}
