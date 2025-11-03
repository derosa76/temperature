String WS_helpMessage(){
  String msg="online da: "+elapsed_time_online()+"\r\n";
  msg=msg+"IP:"+WiFi.localIP().toString()+"\r\n";
  msg=msg+"DNS name = Hostname ='"+String(doc["dns_name"])+"'\r\n";
  msg=msg+"available commands:\r\n";
  msg=msg+String(doc["dns_name"])+".local/?cmd=setdns;nuovo_nome_dns\r\n";
  msg=msg+String(doc["dns_name"])+".local/?cmd=setestate;1 o 0 (inverno =0, estate=1)\r\n";
  msg=msg+String(doc["dns_name"])+".local/?cmd=setdescription;x;description \r\n";
  //msg=msg+String(doc["dns_name"])+".local/?cmd=setrnota;x;r \r\n";
  msg=msg+String(doc["dns_name"])+".local/?cmd=setgpio;do_n;gpio    // set del GPIO da associare al DO\r\n"; ////////<<<<<<<<<--------------
  // msg=msg+String(doc["dns_name"])+".local/?cmd=setnegation;do_n;negation  //negation può essere 0 (non negato) o 1 (output negato)\r\n"; ////////<<<<<<<<<--------------
  // msg=msg+"\r\n";
  // msg=msg+String(doc["dns_name"])+".local/?cmd=set_do_description;x;description \r\n";
  // //msg=msg+String(doc["dns_name"])+".local/?cmd=set_timer_seconds;x;start_time_seconds_from_midnight;duration_seconds \r\n";
  // msg=msg+"\r\n";
  // msg=msg+String(doc["dns_name"])+".local/?cmd=set_timer;x;start_time(hh:mm));duration_seconds;period_hours\r\n";
  // msg=msg+String(doc["dns_name"])+".local/?cmd=open;x;duration_seconds;period_hours (test che fa partire adesso e rimane memorizzato)"+"\r\n";
  // msg=msg+"\r\n";
  // msg=msg+String(doc["dns_name"])+".local/?cmd=reset_program --> rst programmazione. NON descr dei DO"+"\r\n";
  // msg=msg+"\r\n";
  // msg=msg+String(doc["dns_name"])+".local/log"+"\r\n";
  // msg=msg+"\r\n";
  //msg=msg+"disponibili x=0..11\r\n";
  //msg=msg+do_get_status_all();
  msg=msg+String(doc["dns_name"])+".local/?cmd=set_sp;n;val\r\n";
  msg=msg+String(doc["dns_name"])+".local/?cmd=set_kp;n;val\r\n";
  msg=msg+String(doc["dns_name"])+".local/?cmd=set_ki;n;val\r\n";
  msg=msg+String(doc["dns_name"])+".local/?cmd=set_kd;n;val\r\n";
  msg=msg+String(doc["dns_name"])+".local/?cmd=get_pv;0 --> restituisce un json es. {\"pv\": 25.3}\r\n";
  msg=msg+get_all_measurements_details();

  String s;
  serializeJsonPretty(doc,s);
  msg=msg+s+"\r\n";
  msg=msg+"\r\nfreemem: " + String(esp_get_free_heap_size()) + " bytes";
  return msg;
}


String WS_cmd_interpreter(String cmd){
  otalog("interpretazione comando:"+cmd);

  String s0=getValue(cmd,';',0);

  if (s0.equals("setdns")){
    otalog("settaggio DNS name..."); 
    String DNS_name=getValue(cmd,';',1);
    doc["dns_name"]=DNS_name;// {if(setFileVarString("dns_name", DNS_name)) {
    otalog("new DNS:"); 
    otalog(String(doc["dns_name"]));
    
    reboot=true;
    return ("cmd: "+cmd+" executed \r\n ESP reboot...");
  }

  if (s0.equals("setestate")){
    otalog("settaggio estate..."); 
    String val=getValue(cmd,';',1);
    int val_int=val.toInt();
    doc["estate"]=val_int;
    otalog("new estate value:"); 
    otalog(String(doc["estate"]));
    
    reboot=true;
    return ("cmd: "+cmd+" executed \r\n ESP reboot...");
  }

 if (s0.equals("setdescription")){
    String n_string=getValue(cmd,';',1);
    int n_int=n_string.toInt(); //If no valid conversion could be performed because the String doesn’t start with a integer number, a zero is returned.
    String description=getValue(cmd,';',2);
    doc["description"][n_int]=description;
    return ("cmd: "+cmd+" executed");
  }


   if (s0.equals("setgpio")){
    String gpio_n_string=getValue(cmd,';',1);
    int gpio_n_int=gpio_n_string.toInt(); //If no valid conversion could be performed because the String doesn’t start with a integer number, a zero is returned.
    String gpio=getValue(cmd,';',2);
    int gpio_int=gpio.toInt();
    doc["gpio"][gpio_n_int]=gpio_int;
    otalog("doc[\"dgpio\"]["+String(gpio_n_int)+"]="+String(gpio_int)+" -"+String(doc["description"][gpio_int])+" -");
    return ("cmd: "+cmd+" executed");
  }



   if (s0.equals("set_kp")){
    String n_string=getValue(cmd,';',1);
    int n_int=n_string.toInt(); //If no valid conversion could be performed because the String doesn’t start with a integer number, a zero is returned.
    String val=getValue(cmd,';',2);
    double val_double=val.toDouble();
    doc["kp"][n_int]=val_double;
    otalog("doc[\"kp\"]["+String(n_int)+"]="+String(val_double)+" -"+String(doc["description"][n_int])+" -");
    return ("cmd: "+cmd+" executed");
  }

  if (s0.equals("set_ki")){
    String n_string=getValue(cmd,';',1);
    int n_int=n_string.toInt(); //If no valid conversion could be performed because the String doesn’t start with a integer number, a zero is returned.
    String val=getValue(cmd,';',2);
    double val_double=val.toDouble();
    doc["ki"][n_int]=val_double;
    otalog("doc[\"ki\"]["+String(n_int)+"]="+String(val_double)+" -"+String(doc["description"][n_int])+" -");
    return ("cmd: "+cmd+" executed");
  }

  if (s0.equals("set_kd")){
    String n_string=getValue(cmd,';',1);
    int n_int=n_string.toInt(); //If no valid conversion could be performed because the String doesn’t start with a integer number, a zero is returned.
    String val=getValue(cmd,';',2);
    double val_double=val.toDouble();
    doc["kd"][n_int]=val_double;
    otalog("doc[\"kd\"]["+String(n_int)+"]="+String(val_double)+" -"+String(doc["description"][n_int])+" -");
    return ("cmd: "+cmd+" executed");
  }

  if (s0.equals("set_sp")){
    String n_string=getValue(cmd,';',1);
    int n_int=n_string.toInt(); //If no valid conversion could be performed because the String doesn’t start with a integer number, a zero is returned.
    String val=getValue(cmd,';',2);
    double val_double=val.toDouble();
    doc["sp"][n_int]=val_double;
    otalog("doc[\"sp\"]["+String(n_int)+"]="+String(val_double)+" -"+String(doc["description"][n_int])+" -");
    return ("cmd: "+cmd+" executed");
  }

//"http://192.168.1.37/?cmd=get_pv;0 --> restituisce un json es. {"pv": 25.3}"
  if (s0.equals("get_pv")){
    String n_string=getValue(cmd,';',1);
    int n_int=n_string.toInt(); //If no valid conversion could be performed because the String doesn’t start with a integer number, a zero is returned.
    String res="{\"pv\": "+String(pv[n_int],3)+"}";
    otalog("richiesta temperatura N."+n_string+" evasa: pv["+n_string+"] --> "+res);
    return (res);
  }

  //"http://192.168.1.37/?cmd=get_pv;0 --> restituisce un json es. {"pv": 25.3}"
  if (s0.equals("get_all")){
    JsonDocument doc_;
    JsonArray data_;
    data_ = doc_["description"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(doc["description"][i]);
    data_ = doc_["gpio"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(doc["gpio"][i]);
    data_ = doc_["kp"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(doc["kp"][i]);
    data_ = doc_["ki"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(doc["ki"][i]);
    data_ = doc_["kd"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(doc["kd"][i]);
    data_ = doc_["sp"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(doc["sp"][i]);
    data_ = doc_["pv"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(pv[i]);
    data_ = doc_["op"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(op[i]);
    data_ = doc_["err"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(err[i]);
    data_ = doc_["P"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(P[i]);
    data_ = doc_["I"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(I[i]);
    data_ = doc_["D"].to<JsonArray>(); for (int i=0;i<16;i++) data_.add(D[i]);
    String s;
    serializeJson(doc_,s);
    return (s);
  }


  return "cmd not recognized";
}
