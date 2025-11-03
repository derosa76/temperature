void ota_setup(){
    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
    })
    .onEnd([]() {
    })
    .onProgress([](unsigned int progress, unsigned int total) {
    })
    .onError([](ota_error_t error) {
    })
    ;

  ArduinoOTA.begin();
  otalog("ota_setup():executed");
}