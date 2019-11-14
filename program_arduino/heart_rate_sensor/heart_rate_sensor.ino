#define heartratePin A1
#include "DFRobot_Heartrate.h"

DFRobot_Heartrate heartrate(DIGITAL_MODE);

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    uint8_t rateVal;
    heartrate.getValue(A1);
    rateVal = heartrate.getRate();
    if(rateVal) {
        Serial.println(rateVal);
    }
    delay(10);
}
