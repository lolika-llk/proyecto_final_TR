#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
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
SemaphoreHandle_t mutex;
// false significa desbloqueado, esta variable maneja el bloqueo de las variables
// credenciales del wifi
const char *ssid = "";
const char *password = "";
const char *Server_url = "http://192.168.1.68:8000/espmon/api";

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
int TIEMPO_ENTRE_PETICIONES = 3; // tiempo entre cada peticion post al servidor

// funcion que actualiza los datos:
void actualizar(void *pvParameters)
{	
	//INICIAR VARIABLES
	float t, h, p;
	while (true)
	{
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
		//mutex pa que no se peleen los procesos
		if(xSemaphoreTake(mutex, portMAX_DELAY)==pdTRUE){
		temperatura = t;
		humedad = h;
		presion = p;
		xSemaphoreGive(mutex);
		}
		vTaskDelay(pdMS_TO_TICKS(2000));
		/*esperar 2 segundos:
		El sensor es lento y suele tardar aprox 2 segundos para actualizar los datos*/
	}
}

// funcion que envia los datos al servidor:+
void enviar(void *pvParameters)
{
	while (true)
	{	
		{
			//crear cliente
			HTTPClient cliente;
			cliente.begin(Server_url);
			cliente.addHeader("Content-Type", "application/json");
			//crear documento json
			JsonDocument json_doc;
			//mutex
			if(xSemaphoreTake(mutex, portMAX_DELAY)==pdTRUE){
			json_doc["temperatura"]=temperatura;
			json_doc["humedad"]=humedad;
			json_doc["presion"]=presion;
			xSemaphoreGive(mutex);
			}
			String Json_String;
			serializeJson(json_doc, Json_String);//serialize
			int req_code =cliente.POST(Json_String);
			Serial.print("codigo:");Serial.println(req_code);

			cliente.end();
		}

		vTaskDelay(pdMS_TO_TICKS(TIEMPO_ENTRE_PETICIONES*1000)); // Esperar el tiempo configurado
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
		Serial.println("verificar que el BMP180 esta bien conectado");
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
	mutex=xSemaphoreCreateMutex();
	// crear tareas
	xTaskCreate(actualizar, "Mediciones", 2048, NULL, 2, NULL);
	xTaskCreate(enviar, "Envio", 4096, NULL, 1, NULL);
	
}

void loop()
{
	// put your main code here, to run repeatedly:
}
