
String jsonFileVarName = "jfv";
String file_mirror="";

unsigned int json_doc_check_time_interval=500;// ms
unsigned long json_doc_check_time_last=0;


int gpios[] = {15, 16, 17, 21, 22, 32, 25, 27, 23, 19, 18, 26, 33, 13, 14, 4};//{15, 16, 17, 21, 22, 32, 25, 27, 23, 19, 18, 26, 34, 33, 35, 39}; //messi come in eeprom da interfaccia
String descr[]={"Sala","Bagno PT","Studio","Bagno bimbe","Bagno padronale","Cabina armadio","Camera matrim.","Cam. bimbe strada","Cam. bimbe giardino","coll_p1_in","coll_p1_out","coll_P0_in","coll_P0_out","ricircolo PT","ricircolo P1","-"};

void json_file_setup(boolean erase_config){
  String file_content=getFileVarString(jsonFileVarName);
  if ((!erase_config) && (file_content.length()>0)) deserialize_from_file();
  else{
    JsonArray data;
    doc["dns_name"] = "";
    doc["estate"] = 0;
    data = doc["description"].to<JsonArray>(); for (int i=0;i<16;i++) data.add(descr[i]);
    data = doc["gpio"].to<JsonArray>(); for (int i=0;i<16;i++) data.add(gpios[i]);
    data = doc["kp"].to<JsonArray>(); for (int i=0;i<16;i++) data.add(0);
    data = doc["ki"].to<JsonArray>(); for (int i=0;i<16;i++) data.add(0);
    data = doc["kd"].to<JsonArray>(); for (int i=0;i<16;i++) data.add(0);
    data = doc["sp"].to<JsonArray>(); for (int i=0;i<16;i++) data.add(0);
    serialize_to_file();
  }
}

//travasa il json nel file se l'oggetto è differente dal file (quindi se è stato modificato)
void json_file_update(bool force_now){
  if (force_now || (millis()>=json_doc_check_time_last+json_doc_check_time_interval)) {
    json_doc_check_time_last=millis();
    String s;
    serializeJson(doc,s);
    if(!file_mirror.equals(s)) serialize_to_file();
  }
}

//travasa l'oggetto json nel file
void serialize_to_file(){
  String s;
  serializeJson(doc,s);
  otalog("->file:"+s);
  setFileVarString(jsonFileVarName, s);
  file_mirror=s;
}

void deserialize_from_file(){
  String s=getFileVarString(jsonFileVarName);
  DeserializationError error = deserializeJson(doc, s);
  if (error) {Serial.print(F("deserialize_from_file() - deserializeJson() failed: ")); Serial.println(error.f_str());}
  else {
    file_mirror=s;
    otalog("<-file:"+s);
  }
}
