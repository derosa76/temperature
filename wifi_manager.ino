//la pagina di configurazione wifi è all'IP 192.168.4.1

void wifi_manager_setup(){
   WiFiManager wm;
    //wm.resetSettings(); //debug //
    wm.setConnectTimeout(60);

    //setto l'hostname (visualizzato nella config. del router ecc...) con lo stesso valore del dns_name
    String dns_stored=String(doc["dns_name"]);
    int str_len = dns_stored.length() + 1; 
    char dns_stored_ca[str_len];
    dns_stored.toCharArray(dns_stored_ca, str_len);
    wm.setHostname(dns_stored_ca);

    //genero un AP name dinamico che includa il dns name/hostname
    String access_point_name=String(doc["dns_name"])+"_WiFiMGR_AutoConnAP";
    str_len = access_point_name.length() + 1; 
    char access_point_name_ca[str_len];
    access_point_name.toCharArray(access_point_name_ca, str_len);

    if(!wm.autoConnect(access_point_name_ca)) {
      otalog("wifi_manager_setup(): Failed to connect");
      ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        otalog("wifi_manager_setup(): connected...yeey :)");
    }
}