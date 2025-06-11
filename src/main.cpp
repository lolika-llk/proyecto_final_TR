#include <Arduino.h>
// librerias sensor
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
// librerias conectividad
#include "WiFi.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// variables globbales:
float temperatura, humedad, presion;
bool bloqueado = false;
// false significa desbloqueado, esta variable maneja el bloqueo de las variables
// credenciales del wifi
const char *ssid = "INFINITUM3706";
const char *password = "h7CAAXkhyz";
const char *Server_url = "http://192.168.1.68/espmon/api";

// constantes
#define DHTPIN 4	  // pin de datos
#define DHTTYPE DHT11 // tipo de sensor
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085 bmp;
/*el bmp se conecta de la siguiente forma:
	Vin - 3.3V
	GND - GND
	SCL - GPIO 22
	SDA - GPIO 21
	*/

// relay
int rpin = 33, ison = 0;
int TIEMPO_ENTRE_PETICIONES = 5; // tiempo entre cada peticion post al servidor

// funcion que actualiza los datos:
void actualizar(void *pvParameters)
{	
	//INICIAR VARIABLES
	float t, h, p;
	while (true)
	{
		while (bloqueado == true)
		{
		}; // esperar a que se desbloqueen las variables
		bloqueado = true; // bloquear las variables

		{//realizar mediciones
			do
			{
				Serial.println("lt");
				t = dht.readTemperature();
				delay(200);

			} while (isnan(t));
			do
			{	
				Serial.println("lh");
				h = dht.readHumidity();
				delay(200);
			} while (isnan(h));
			do
			{	
				Serial.println("lp");
				p = bmp.readPressure();
				delay(200);
			} while (isnan(p));
		}
		//actualizar variables
		Serial.println(t);
		Serial.println(h);
		Serial.println(p);


		temperatura = t;
		humedad = h;
		presion = p;
		bloqueado = false;
		vTaskDelay(pdMS_TO_TICKS(TIEMPO_ENTRE_PETICIONES * 1000));
		/*esperar 2 segundos:
		El sensor es lento y suele tardar aprox 2 segundos para actualizar los datos*/
	}
}

// funcion que envia los datos al servidor:+
void enviar(void *pvParameters)
{
	HTTPClient cliente; // cliente que realiza las peticiones
	while (true)
	{
		while (bloqueado == true)
		{
		}
		bloqueado = true;
		{
			// crear documento
			JsonDocument json_Doc;
			String json_String;
			//adjuntar datos
			json_Doc["temperatura"] = temperatura;
			json_Doc["humedad"] = humedad;
			json_Doc["presion"] = presion;
			//serializar documento
			serializeJson(json_Doc, json_String);
			// enviar documento
			cliente.begin(Server_url);
			cliente.addHeader("Content-Type", "application/json");
		}

		bloqueado = false;
		vTaskDelay(pdMS_TO_TICKS(2000)); // Esperar el tiempo configurado
																		 // segundos
	}
}

void setup()
{
	// monitor serial
	Serial.begin(115200);
	// iniciar sensores
	dht.begin();
	if(!bmp.begin()){
		Serial.println("Could not find a valid BMP085 sensor, check wiring!");
		while (1) {}
	}
	// conexion wifi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println("Connecting to WiFi..");
	}
	Serial.println(WiFi.localIP()); // hasta aqui funciona correctamente
	// crear tareas
	xTaskCreate(actualizar, "Mediciones", 2048, NULL, 2, NULL);
	xTaskCreate(enviar, "Envio", 2048, NULL, 1, NULL);
}

void loop()
{
	// put your main code here, to run repeatedly:
}
