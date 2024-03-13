#include <logwifi.h>

#define BotaoResetWifi 21
#define LEDWifi 2
#define TempoReset 5000

String NomeDaRede = "Rede Wifi";
logwifi lg(BotaoResetWifi,LEDWifi,TempoReset,NomeDaRede);

void setup() {

  lg.begin();

}

void loop() {

  lg.loop();

}
