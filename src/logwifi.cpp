#include "logwifi.h"
#include <Preferences.h>
#include "ESPAsyncWebServer.h"
#include <Arduino_JSON.h>              //Biblioteca para Fazer os Arquivos JSON       https://github.com/arduino-libraries/Arduino_JSON
#include <AsyncTCP.h>  //6.19.4
#include <WiFi.h>

Preferences preferences;
AsyncWebServer server(80);
JSONVar board;

boolean modoServer = false;
bool buttonPressed = false;
unsigned long buttonPressedTime = 0;
int _pino,_LED,_Time;
String _Rede;

logwifi::logwifi(int pino, int LED, int Tempo ,String RedeName){
    pinMode(pino, INPUT_PULLUP);
    pinMode(LED,OUTPUT);
    _pino = pino;
    _LED = LED;
    _Rede = RedeName;
    _Time = Tempo;
}
String generateSavedConfigHTML() {
  String html = "<!DOCTYPE html><html><head><title>Configuracoes Salvas</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f2f2f2; }";
  html += ".header { background-color: #4CAF50; color: white; text-align: center; padding: 20px; }";
  html += ".container { width: 300px; margin: auto; background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2); text-align: center; }";
  html += "</style>";
  html += "</head><body>";
  html += "<div class='header'><h2>Configuracoes Salvas</h2></div>";
  html += "<div class='container'>";
  html += "<p>Dados de WiFi Salvos. (Caso o Led Azul Acenda Ja foi Conectado)</p>";
  html += "</div></body></html>";

  return html;
}

String generateHTML() {
  String html = "<!DOCTYPE html><html><head><title>Configuracao Wi-Fi</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f2f2f2; }";
  html += ".header { background-color: #4CAF50; color: white; text-align: center; padding: 20px; }";
  html += ".container { width: 300px; margin: auto; background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2); }";
  html += "input[type=text], input[type=password] { width: 100%; padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; }";
  html += "input[type=submit] { width: 100%; background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 4px; cursor: pointer; }";
  html += "</style>";
  html += "</head><body>";
  html += "<div class='header'><h2>Configuracao Wi-Fi</h2></div>";
  html += "<div class='container'>";
  html += "<form action='/save' method='post'>";
  html += "<label for='ssid'>SSID:</label>";
  html += "<input type='text' id='ssid' name='ssid'>";
  html += "<label for='password'>Senha:</label>";
  html += "<input type='password' id='password' name='password'>";
  html += "<input type='submit' value='Salvar'>";
  html += "</form></div></body></html>";

  return html;
}

void handleSave(AsyncWebServerRequest *request) {
  // Verifica se o corpo da solicitação contém dados
  if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
    // Lê os parâmetros do formulário
    String ssid = request->getParam("ssid", true)->value();
    String password = request->getParam("password", true)->value();

    // Cria um objeto JSON
    JSONVar json;
    json["ssid"] = ssid;
    json["password"] = password;
    //Serial.println("SSID recebido: " + ssid);
    //Serial.println("Senha recebida: " + password);
    // Salva os valores nas preferências
    preferences.putString("ssid", ssid.c_str());
    preferences.putString("password", password.c_str());
    preferences.putBool("modoServer", true);
    
    // Envia uma resposta de sucesso
    request->send(200, "text/html", generateSavedConfigHTML());
    delay(200);
    ESP.restart();

  } else {
    // Envia uma resposta de erro
    request->send(400, "text/plain", "Dados ausentes na solicitação");
  }
}

void logwifi::begin(){
    
    digitalWrite(_LED,0);
    preferences.begin("parametros", false);
    modoServer = preferences.getBool("modoServer", false);
    char ssid[50];
    if(!modoServer){
    WiFi.mode(WIFI_AP_STA);
    const char* rede = _Rede.c_str();
    sprintf(ssid,"%s",rede);
    WiFi.softAP(ssid);
    // Serial.println("Servidor WebService Criado");
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", generateHTML());
    });
    server.on("/save", HTTP_POST, handleSave);
    server.begin();
    while (WiFi.status() != WL_CONNECTED){
    }
  }
  else {
    const String ssidString = preferences.getString("ssid", "");
    const String passwordString = preferences.getString("password", "");  
    //Serial.println(ssidString);
    //Serial.println(passwordString);
    WiFi.begin(ssidString.c_str(),passwordString.c_str());
  
    while (WiFi.status() != WL_CONNECTED) {
        //Serial.println("Conectando ao WiFi...");
        digitalWrite(_LED,0);
        delay(150);
        digitalWrite(_LED,1);
        delay(150);
      if (digitalRead(_pino) == LOW) {
    
        if (!buttonPressed) {
            buttonPressed = true;
            buttonPressedTime = millis();
      //Serial.println("Botão pressionado");
    }
    
    else if (millis() - buttonPressedTime >= _Time) {
     // Serial.println("Botão pressionado por 5 segundos!");
      preferences.putBool("modoServer", false);
      ESP.restart();

    }

    }

  }
  digitalWrite(_LED,1);
}
}



void logwifi::loop(){
    
  if (digitalRead(_pino) == LOW) {
    
    if (!buttonPressed) {
      buttonPressed = true;
      buttonPressedTime = millis();
      Serial.println("Botão pressionado");
    }
    
    else if (millis() - buttonPressedTime >= _Time) {
     // Serial.println("Botão pressionado por 5 segundos!");
      preferences.putBool("modoServer", false);
      ESP.restart();

    }
    }

  if (WiFi.status() != WL_CONNECTED) {
  digitalWrite(LED,0);
  const String ssidString = preferences.getString("ssid", "");
  const String passwordString = preferences.getString("password", "");  
  WiFi.begin(ssidString.c_str(),passwordString.c_str()); // Conecta na rede WI-FI
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(100);
    Serial.print(".");
    attempts++;
    if (attempts == 30){
      WiFi.begin(ssidString.c_str(),passwordString.c_str());
    }
    digitalWrite(LED,0);
    delay(150);
    digitalWrite(LED,1);
    delay(150);
  }
  if (attempts == 40) {
    Serial.println("Falha ao conectar à rede Wi-Fi após várias tentativas.");
    ESP.restart();
  } else {
  digitalWrite(LED,1);
  }
  }
}
