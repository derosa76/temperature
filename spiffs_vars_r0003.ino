

void  spiffs_vars_begin() {
  //if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
  if (!SPIFFS.begin(true)) { // questa modifica ha risolto l'errore di mount della SPI https://github.com/espressif/arduino-esp32/issues/638
    otalog("SPIFFS Mount Failed");
    return;
  }
}

bool writeFile_(fs::FS &fs, const char *path, const char *message) {
  bool result=false;
  File file = fs.open(path, FILE_WRITE);

  if (!file) return result;
  if (file.print(message)) result=true; 
  else result=false;
  file.close();

  return result;
}

String readFile_(fs::FS &fs, const char *path) {
  String result="";
  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    return result;
  }

  while (file.available()) {
    char c=file.read();
    result=result+String(c);
  }
  file.close();

  return result;
}


bool setFileVarString(String varname, String value){
  String path = "/"+varname;
  writeFile_(SPIFFS, path.c_str(), value.c_str());
  String b = getFileVarString(varname);
  return (getFileVarString(varname).equals(value));
}

String getFileVarString(String varname){
  String path = "/"+varname;
  return readFile_(SPIFFS, path.c_str());
}

/* funzioni escluse perchè non usate, benchè fossero funzionanti
bool setFileVarInt(String varname, int value){
  String value_s=String(value);
  if(setFileVarString(varname,value_s)){ //se va a buon fine la scrittura e verifica della stringa
    return(getFileVarInt(varname)==value); //ritorno true solo se l'intero riletto dalla relativa funzione è identico
  }
  return false; //se no ricado su ritorno di false
}

int getFileVarInt(String varname){
  return getFileVarString(varname).toInt();
}

bool setFileVarDouble(String varname, double value){
  String value_s=String(value,10);
  if(setFileVarString(varname,value_s)){ //se va a buon fine la scrittura e verifica della stringa
    return(abs(getFileVarDouble(varname)-value)<10e10); //ritorno true solo se il double riletto dalla relativa funzione è sostanzialmente identico
  }
  return false; //se no ricado su ritorno di false
}

double getFileVarDouble(String varname){
  return getFileVarString(varname).toDouble();
}

bool setFileVarFloat(String varname, float value){
  String value_s=String(value,contaDecimali(value));
  if(setFileVarString(varname,value_s)){ //se va a buon fine la scrittura e verifica della stringa
    return(getFileVarDouble(varname)==value); //ritorno true solo se il double riletto dalla relativa funzione identico
  }
  return false; //se no ricado su ritorno di false
}

double getFileVarFloat(String varname){
  return getFileVarString(varname).toFloat();
}

bool setFileVarLong(String varname, long value){
  String value_s=String(value);
  if(setFileVarString(varname,value_s)){ //se va a buon fine la scrittura e verifica della stringa
    return(getFileVarLong(varname)==value); //ritorno true solo se l'intero riletto dalla relativa funzione è uguale
  }
  return false; //se no ricado su ritorno di false
}

long getFileVarLong(String varname){
  return (long)getFileVarString(varname).toDouble();
}


//Utilities
int contaDecimali(float numero) {
  int count = 0;
  while (numero != (int)numero) {
    numero *= 10;
    count++;
  }
  return count;
}

//crea stringa con tutti, e soli, i decimali del float
String floatToString(float f){
  return String(f,contaDecimali(f));
}

*/