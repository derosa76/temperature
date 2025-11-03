#include <ArduinoOTA.h> // questo non si può spostare nella scheda ota.ino ... altrimenti non va

//queste devono essere definite qui ed in questo ordine, se no fanno casino (errori)
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h> //https://github.com/me-no-dev/ESPAsyncWebServer/tree/master

AsyncWebServer server(80);

#include <ESP32Time.h>
ESP32Time time_object(0); // GMT

//per spiffs_vars
#include "FS.h"
#include "SPIFFS.h"
#define FORMAT_SPIFFS_IF_FAILED true

//per json
#include <ArduinoJson.h>
JsonDocument doc; //questo documento contiene la configurazione. Serializzazione da oggetto a stringa--> file e viceversa viene fatta in json.ino


bool reboot=false;

int boot_time=0;//time_object.getEpoch()

//double t_estr_lin[16]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup(void)
{
  wifi_watchdog_timer_setup(60); // questa è la funzione da chiamare in setup x forza. Questa innesca la chiamata a funzione ISR (interrupt ... Routine) ogni intervallo di secondi qui definito
  Serial.begin(115200);
  otalog("-------START-------");
  wifi_manager_setup();
  spiffs_vars_begin();//per spiffs_vars
  ota_setup();
  json_file_setup(false);//primo caricamento: true, per azzerare il file. Poi false.
  dns_setup(); // questo va messo per forza dopo OTA setup, se no non funziona!! E anche dopo il caricamento della configurazione dal file json json_file_setup(...)
  wserver_setup();

  //do_setup();

  run_time_sync(0);
  delay(5000);
  boot_time=time_object.getEpoch();

  //misure_setup();//**********
  DS18B20_setup();

  
}

//ogni funzione richiamata in loop() è fatta in modo da non bloccare mai il flusso con dei delay
//ma per essere certo di avere sempre la possibiliotà OTA, eseguo le altre funzioni (che tipicamente ci impiegano max 2.6 ms) ogni tot
int all_func_dt=500;//ms
unsigned long all_func_run_time=0;
void loop()
{
  ArduinoOTA.handle();
  DS18B20_run(10,false);
  
  //run_misure(5000,20,5*60*1000);//run_misure2(unsigned long delta_t_fra_misure, unsigned long tempo_attesa_stabilizzazione_tensione, unsigned long tempo_smoothing_misura)
  if (millis()>all_func_run_time+all_func_dt){
    all_func_run_time=millis(); 
    all_other_functions();
    //otalog(String(micros()-all_func_run_time*1000)); // tipicamente max 2600 micros = 2.6 ms
  }
}

void all_other_functions(){
    wifi_watchdog_timer_run(1000); //obbligatoria per azzerare il counter se c'è Wifi
    wifi_watchdog_timer_print_info(24*60*60*1000); // questa è la funzione opzionale, che fornisce un po' di debug se necessario
    //dayly_programming_run();
    run_time_sync(60*60*1000);
    json_file_update(false);
    pid_run();
    printfreemem(60000);
    if(reboot) {json_file_update(true);delay(5000);ESP.restart();}
}

double freemem_millis=-3600000;
void printfreemem(double ms_interval){
  if(millis()>freemem_millis+ms_interval){
    freemem_millis=millis();
    Serial.println("freemem: " + String(esp_get_free_heap_size()) + " bytes");
  }
  return;
}

  
