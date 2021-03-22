#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h> 
#include <EEPROM.h>

#define SIZE_EEPROM 25
#define MAX_TRY_CONN 30

int cont = 0;

char ssid[SIZE_EEPROM];      
char pass[SIZE_EEPROM];

const char *ssidName = "baseWifi";
const char *passWord = "basewifi";

bool modeAPState = false;

IPAddress ip(192, 168, 1, 120);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// WiFiClient espClient;
AsyncWebServer server(80);

void setup_wifi() {
  // Conexión WIFI

  Serial.print("modeAPState ");
  Serial.println(modeAPState);
  if(modeAPState == 0) {
    Serial.println("Modo STA");
    WiFi.mode(WIFI_STA);
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    WiFi.config(ip, gateway, subnet);
    // Intenta conectar MAX_TRY_CONN veces
    while (WiFi.status() != WL_CONNECTED && cont < MAX_TRY_CONN) {
      cont++;
      delay(250);
      Serial.print(".");
      digitalWrite(2, HIGH);
      delay(250);
      digitalWrite(2, LOW);
    }

  } else {
    Serial.println("Modo AP");
    WiFi.mode(WIFI_AP);
    Serial.print("SSID: ");
    Serial.println(ssidName);
    Serial.print("Password: ");
    Serial.println(passWord);
    WiFi.begin(ssidName, passWord);
    WiFi.softAPConfig(ip, gateway, subnet);
    while (!WiFi.softAP(ssidName, passWord) && cont < MAX_TRY_CONN) {
      cont++;
      delay(250);
      Serial.print(".");
      digitalWrite(2, LOW);
      delay(250);
      digitalWrite(2, HIGH);
    }
  }
  
  // Si el número de intentos de conexion es menor de MAX_TRY_CONN ha conectado
  if (cont <MAX_TRY_CONN) {   
      Serial.println("");
      Serial.print("Iniciado STA:\t");
      Serial.println(ssid);
      Serial.print("IP address:\t");
      Serial.println(ip);
      // LED encendido indicado que esta conectado y funcionando
      digitalWrite(2, LOW);  

      /** Inicialización del servidor **/
      server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

      server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(400, "text/html", "Not found");
      });

      /**
       * Añádir aqui servicios a utilizar.
       */

      server.begin();
      
  } else { 
      Serial.println("Error de conexion");
      // LED apagado indicando error
      digitalWrite(2, HIGH);
  }
}

//--------------------------------------------------------------

void modeConf() {

  Serial.println("Modo configuracion start");
  for (size_t i = 0; i < 2; i++)
  {
    delay(500);
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
  }

  WiFi.mode(WIFI_AP);
  Serial.println("Intentando conectar modo AP");
  Serial.println(ssidName);
  Serial.println(passWord);
  while(!WiFi.softAP(ssidName, passWord))
  {
    Serial.println(".");
    delay(100);
  }

  WiFi.softAPConfig(ip, gateway, subnet);

  Serial.println("");
  Serial.print("Iniciado AP:\t");
  Serial.println(ssidName);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
 
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("config.html");

  server.on("/guardar", HTTP_POST, handleGuardar);

  server.on("/buscar", HTTP_GET, buscar);

  server.on("/app", HTTP_POST, guardarModoAP);

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(400, "text/html", "Not found");
  });
  
  server.begin();
}

void guardarModoAP(AsyncWebServerRequest *request) {
  grabarModoAP(true);
  String inner ="<p>Modo AP activado<br><a href=\"/\">Return to Home Page</a></p>";
  request->send(200, "text/html", inner);
}


void handleGuardar(AsyncWebServerRequest *request) {
  String ssidPost = request->arg("ssid");
  String passPost = request->arg("pass");

  guardarEEPROM(ssidPost, passPost);

  String inner ="<p>ssid Guardado<br><a href=\"/\">Return to Home Page</a></p>";

  request->send(200, "text/html", inner);
}

/**
 * Lla a guardar ssid y pass en la EEPROM
 */
void guardarEEPROM(String ssid, String pass) {
  grabar(0,ssid);
  grabar(SIZE_EEPROM,pass);
  Serial.println("Modo AP off");
  grabarModoAP(false);
}

/*
* Graba en la EEPROM
* int addr posición inicial EEPROM
* String a String a guardar
*/
void grabar(int addr, String a) {
  int len = a.length(); 
  char inchar[SIZE_EEPROM]; 
  Serial.print("Guardando en Eeprom ");
  a.toCharArray(inchar, len+1);
  for (int i = 0; i < len; i++) {
    Serial.print(inchar[i]);
    EEPROM.write(addr+i, inchar[i]);
  }
  for (int i = len; i < SIZE_EEPROM; i++) {
    EEPROM.write(addr+i, 255);
  }
  EEPROM.commit();
}

/*
* Graba el modo de operación en la EEPROM 
*/
void grabarModoAP(bool modeAPState) {
  Serial.print("Guardando en EEPROM Modo Operación: ");
  Serial.println(modeAPState);
  EEPROM.write(SIZE_EEPROM * 2, modeAPState);
  EEPROM.commit();
}

/*
* Lee de la EEPROM
*/
String readEEPROM(int addr) {
   byte read;
   String strRead;
   for (int i = addr; i < (addr + SIZE_EEPROM); i++) {
      read = EEPROM.read(i);
      if (read != 255) {
        strRead += (char)read;
      }
   }
   return strRead;
}

/**
 * Buscar redes.
 */
void buscar(AsyncWebServerRequest *request) {
  Serial.println("Buscando redes");  
  String inner = "<p>No se han econtrado redes</p>";
  // Número de redes encontradas
  int n = WiFi.scanComplete();
  if(n == -2){
    WiFi.scanNetworks(true);
  } else if(n){
    inner = "";
    for (int i = 0; i < n; ++i){
      inner = (inner) + "<p>" + String(i + 1) + ": <strong>" + WiFi.SSID(i) + "</strong> (" + WiFi.RSSI(i) + ") Canal: " + WiFi.channel(i) + " Encriptación: " + encriptacion(WiFi.encryptionType(i)) + " </p>\r\n";      
      delay(10);
    }
    Serial.println(inner);
  }
    WiFi.scanDelete();
    if(WiFi.scanComplete() == -2){
      WiFi.scanNetworks(true);
    }
    Serial.println("numero de redes " + n);

  request->send(200, "text/html", inner);
}

/**
 * Devuelve tipo de encriptación de la red
 */
String encriptacion(int enc) {
  String res = "";
  switch (enc)
    {
    case 2:
      res = "WPA/PSK";
      break;
    case 4:
      res = "WPA2/PSK";
      break;
    case 5:
      res = "WEP";
      break;
    case 7:
      res = "N/A";
      break;
    case 8:
      res = "WPA/WPA2/PSK";
      break;
    default:
      res = "None";
      break;
    }

  return res;
}

void setup() {

  pinMode(2, OUTPUT); 
  
  // Inicia Serial
  Serial.begin(115200);
  SPIFFS.begin();
  Serial.println("");

  EEPROM.begin(512);

  pinMode(0, INPUT);
  
  int cont = 0;
  int digitRead = 0;
  while(cont < 10 && !digitRead) {
    cont++;
    if(digitalRead(0) == 0)  {
      digitRead = 1;
    }
    delay(100);    
  }

  // Comprobamos si ejecutamos modo AP o no
  modeAPState = EEPROM.read(SIZE_EEPROM * 2);

  if (digitRead) {
    Serial.println("Modo configuracion ON");
    modeConf();
  } else {
    readEEPROM(0).toCharArray(ssid, SIZE_EEPROM);
    readEEPROM(SIZE_EEPROM).toCharArray(pass, SIZE_EEPROM);

    Serial.println("Modo wifi ON");
    Serial.print("Modo de operación: ");
    Serial.println(modeAPState);
    setup_wifi();
  }

}

void loop() {

  // Tu código aqui.
}
