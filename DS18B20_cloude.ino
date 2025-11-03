#include <OneWire.h>
#include <DallasTemperature.h>

double t[]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
//double t_filtered[]={-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // temperatura calcolata, con filtro EWMA (media mobile esponenziale) con alpha=0.1


// Classe per gestire un gruppo di sensori su una singola linea OneWire
class TemperatureSensorGroup {
  private:
    int n;                       //numero sensore: è un riferimento che passo al costruttore
    int pin;                    // Pin della linea OneWire
    OneWire* oneWire;          // Oggetto OneWire
    DallasTemperature* sensors; // Oggetto DallasTemperature
    DeviceAddress* addresses;   // Array di indirizzi dei sensori
    int sensorCount;           // Numero di sensori trovati
    float* temperatures;        // Array delle ultime temperature lette
    float lastAverage;         // Ultima media calcolata
    
  public:
    TemperatureSensorGroup() {
      n = -1;
      pin = -1;
      oneWire = NULL;
      sensors = NULL;
      addresses = NULL;
      temperatures = NULL;
      sensorCount = 0;
      lastAverage = -127;
    }
    
    //Inizializza il gruppo di sensori su un pin specifico
    bool begin(int n_, int newPin) {
      n=n_;
      pin = newPin;
      oneWire = new OneWire(pin);
      sensors = new DallasTemperature(oneWire);
      
      sensors->begin();
      sensorCount = sensors->getDeviceCount();
      
      if (sensorCount == 0) {
        return false;
      }
      
      // Alloca memoria per gli array
      addresses = new DeviceAddress[sensorCount];
      temperatures = new float[sensorCount];
      
      // Inizializza ogni sensore
      for (int i = 0; i < sensorCount; i++) {
        sensors->getAddress(addresses[i], i);
        sensors->setResolution(addresses[i], 12);
        temperatures[i] = -127;
      }
      
      return true;
    }
    
    // Libera la memoria allocata
    ~TemperatureSensorGroup() {
      if (addresses) delete[] addresses;
      if (temperatures) delete[] temperatures;
      if (sensors) delete sensors;
      if (oneWire) delete oneWire;
    }
    
    // Legge tutti i sensori e calcola la media
    float readAverageTemperature() {
      sensors->requestTemperatures();
      float sum = 0;
      int validReadings = 0;
      
      for (int i = 0; i < sensorCount; i++) {
        temperatures[i] = sensors->getTempC(addresses[i]);
        if (temperatures[i] != DEVICE_DISCONNECTED_C) {
          sum += temperatures[i];
          validReadings++;
        }
      }
      
      lastAverage = (validReadings > 0) ? sum / validReadings : -127;
      return lastAverage;
    }
    
    // Restituisce l'ultima media calcolata
    float getLastAverage() {
      return lastAverage;
    }
    
    // Restituisce il numero di sensori trovati
    int getSensorCount() {
      return sensorCount;
    }
    
    // Restituisce il pin configurato
    int getPin() {
      return pin;
    }
    
    // Restituisce il numero sensore dato al momento della creazione dell'oggetto
    int getn() {
      return n;
    }



    // Restituisce la temperatura di un sensore specifico
    float getTemperature(int index) {
      if (index >= 0 && index < sensorCount) {
        return temperatures[index];
      }
      return -127;
    }
    
    // Restituisce l'indirizzo di un sensore specifico come stringa
    String getAddressString(int index) {
      if (index >= 0 && index < sensorCount) {
        String addr = "";
        for (uint8_t i = 0; i < 8; i++) {
          if (addresses[index][i] < 16) addr += "0";
          addr += String(addresses[index][i], HEX);
        }
        return addr;
      }
      return "invalid";
    }
};

// Gestore per multipli gruppi di sensori
class SensorGroupManager {
  private:
    static const int MAX_GROUPS = 32;  // Numero massimo di gruppi (linee OneWire)
    TemperatureSensorGroup groups[MAX_GROUPS];
    int groupCount;
    
  public:
    SensorGroupManager() {
      groupCount = 0;
    }
    
    // Aggiunge un nuovo gruppo di sensori
    bool addGroup(int n_, int pin) {
      if (groupCount >= MAX_GROUPS) {
        return false;
      }
      
      if (groups[groupCount].begin(n_,pin)) {
        groupCount++;
        return true;
      }
      return false;
    }
    
    // Legge tutti i gruppi
    void readAll() {
      for (int i = 0; i < groupCount; i++) {
        groups[i].readAverageTemperature();
      }

    }
    
    // Restituisce il numero di gruppi configurati
    int getGroupCount() {
      return groupCount;
    }
    
    // Accede a un gruppo specifico
    TemperatureSensorGroup* getGroup(int index) {
      if (index >= 0 && index < groupCount) {
        return &groups[index];
      }
      return NULL;
    }
};

// Istanza globale del gestore gruppi
SensorGroupManager groupManager;




void DS18B20_setup(){
  Serial.println("DS18B20_setup()...");
  for (int i=0; i<16;i++){
    int pin=doc["gpio"][i];
    //int group_count=0;
    if (groupManager.addGroup(i,pin))
    {
      Serial.print("n="+format_int(i,2));
      Serial.print(" - Gruppo "+String(groupManager.getGroupCount())+" aggiunto sul pin "+String(pin)+" - trovati ");
      Serial.print(groupManager.getGroup(groupManager.getGroupCount()-1)->getSensorCount());
      Serial.println(" sensori");
      //group_count++;
    }
  }
}

bool first_run=true;
unsigned long last_run_time=0;

void DS18B20_run(int delta_t_seconds, bool debug){
  if (first_run || millis()>last_run_time+delta_t_seconds*1000){
    first_run=false;
    last_run_time=millis();
    // Leggi tutti i gruppi
    groupManager.readAll();
    
    // Stampa i dati di tutti i gruppi
    for (int g = 0; g < groupManager.getGroupCount(); g++) {
      TemperatureSensorGroup* group = groupManager.getGroup(g);
      if (group) {
        if (debug){
          String g_s=format_int(g,2);
          String n_s=format_int(group->getn(),2);
          Serial.print("n="+n_s+" - ");
          Serial.print("Gruppo "+g_s+" sul pin ");
          Serial.print(group->getPin());
          Serial.print(" (");
          Serial.print(group->getSensorCount());
          Serial.print(" sensori) - Media: ");
          Serial.print(group->getLastAverage());
          Serial.print("°C  {");
          // Stampa i dettagli di ogni sensore nel gruppo
          for (int s = 0; s < group->getSensorCount(); s++) {
            Serial.print("[Sensore ");
            Serial.print(s);
            Serial.print(" (");
            Serial.print(group->getAddressString(s));
            Serial.print("): ");
            Serial.print(group->getTemperature(s));
            Serial.print("°C]");
          }
          Serial.println("}");
        }


        int n=group->getn();
        t[n]=group->getLastAverage();

        //https://stats.stackexchange.com/questions/5290/moving-return-of-exponential-moving-average-choice-of-alpha
        //double tempo_smoothing_misura = 60; //s
        //double delta_t_fra_misure = 10; //s
        //double nn=tempo_smoothing_misura/delta_t_fra_misure;
        //double alpha=2.0/(nn+1.0);
        //if(t_filtered[n]==0) {t_filtered[n]=t[n];} else t_filtered[n] = alpha * t[n] + (1 - alpha) * t_filtered[n];  //se è il primo passaggio, lo impongo uguale a t[i], altrimenti calcolo Ewma 
      }
    }
    slope_matrix_push();
  }
}



