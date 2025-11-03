#include <ESPmDNS.h>

void dns_setup(){
  //SET DNS
  //recupero il dns_name in memoria flash
  //se non c'è, non lo configuro --> per configurarlo bisogna farlo tramite http://192.168.XX.XX/cmd?cmd=dns;dns_name
  //ping esp32.local //esempio ping che deve funzionare
  //preferences_set_string("dns_name", "temperature");
  String dns_stored=String(doc["dns_name"]);
  otalog("dns_name in config:"+dns_stored);
  if (dns_stored.length()>0){
    if (!MDNS.begin(dns_stored))  {otalog("dns_setup(): Error starting mDNS:"+dns_stored);}// Serial.println(dns_stored);}
    else {otalog("dns_setup(): mDNS ok! dns_name:"+dns_stored);}// otalog(dns_stored);}
  }
  else otalog("dns_setup(): mDNS non settato :(");
}