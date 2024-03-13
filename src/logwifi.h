#ifndef _LOGWIFI_H
#define _LOGWIFI_H

#include <Arduino.h>

class logwifi {

    private:

    public:
        logwifi(int pino, int led, int Tempo ,String RedeName);
        void begin();
        void loop();
};



#endif