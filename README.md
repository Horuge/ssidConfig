# ssidConfig
Guarda ssid y password en en EEPROM de nodemcu en tiempo de ejecución

# Liberias
ESP8266WiFi: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
ESPAsyncTCP: https://github.com/me-no-dev/ESPAsyncTCP
ESPAsyncWebServer: https://github.com/me-no-dev/ESPAsyncWebServer
SPIFFS: https://github.com/esp8266/arduino-esp8266fs-plugin/releases/

# Funcionamiento
Para acceder al modo de configuración debemos pulsar el botón de flash. Tenemos 1 segundo para pulsarlo desde que se se inicia el dispositivo o se pulsa reset.
Al entrar en modo de configuración se el LED del ESP8266 parpareada 2 veces con una duración de 2 segundos indicando que ha entrado en al modo de configuración.
La configuración crea una red baseWifi con contraseña basewifi.
Hay que conectar a esta red y acceder a la url:
http://192.168.1.120

Podemos indicar el SSID y el Password en el formulario y pulsar guardar.
Tras guardar se indicará en una página nueva que se ha grabado.
Si lo que se quiere es que el ESP8266 actue como AP hay que pulsar el botón 'Activar modo AP'. También se indicará que se ha configurado este modo.
Si no sabemos a que red conectar podermos ver las redes al alcance disponibles con el botón 'Buscar'. Esto mostrará el SSID de las redes encontradas, solamente hay que copiar el ssid en el formulario, añadir su contraseña y guardar.

ESP8266 intentará conectar un número de veces (30), mientras intenta conectar el indicador led parpadeará. Si no consigue conectarse y se produce un error el led permanerá activo. El led encendido indicará que se ha establecido un modo de operación y que está activo.
