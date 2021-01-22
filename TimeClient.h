/**The MIT License (MIT)

Copyright (c) 2017 by Radim Keseg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include <ESP8266WiFi.h>

class TimeClient {

  private:
    float myUtcOffset = 0;
    long localEpoc = 0;
    long localMillisAtUpdate;
        
  public:
    TimeClient() : TimeClient(0) {}
    TimeClient(float utcOffset);
    void setTimeOffset(float utcOffset);
    void updateTime();

    String getHours();
    String getMinutes();
    String getSeconds();
    String getFormattedTime();
    long getCurrentEpoch();
    long getCurrentEpochWithUtcOffset();

    int getHoursInt();
    int getMinutesInt();
    int getSecondsInt();

};

