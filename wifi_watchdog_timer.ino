
volatile unsigned long interruptCounter;  //for counting interrupt
volatile int offline_time_to_restart=3; //3 triggers (in questo caso ogni minuto)-->3 minuti (se si parte dal restart). Se invece la connessione "va via" prima del restart passano dai 3 ai 4 minuti
hw_timer_t * timer = NULL;      //H/W timer defining (Pointer to the Structure)
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
unsigned int wifi_watchdog_timer_print_info_last_passage=0;
unsigned int wifi_watchdog_timer_run_last_passage=0;

 
void IRAM_ATTR onTimer() {      //Defining Inerrupt function with IRAM_ATTR for faster access
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  if (interruptCounter>=offline_time_to_restart) ESP.restart();
  portEXIT_CRITICAL_ISR(&timerMux);
}

void wifi_watchdog_timer_setup(int interrupt_seconds) {
 timer = timerBegin(1000000);           	//timer = timerBegin(0, 80, true);  //  timer 0, prescalar: 80, UP counting
 timerAttachInterrupt(timer, &onTimer); 	// timerAttachInterrupt(timer, &onTimer, true); //Attach interrupt
 timerAlarm(timer, interrupt_seconds*1000000, true, 0);  	// timerAlarmWrite(timer, interrupt_seconds*1000000, true);	// Match value= 1000000 for 1 sec. delay.
 //timerAlarmEnable(timer);           			// Enable Timer with interrupt (Alarm Enable)
 otalog("wifi_watchdog_timer_setup() eseguita");
}

void  wifi_watchdog_timer_run(unsigned int intervallo_ms){
    if (millis()>wifi_watchdog_timer_run_last_passage+intervallo_ms){
      wifi_watchdog_timer_run_last_passage=millis();
      if (WiFi.status() == WL_CONNECTED) interruptCounter=0;
    }
}

void wifi_watchdog_timer_print_info(unsigned int intervallo_ms){
  if (millis()>wifi_watchdog_timer_print_info_last_passage+intervallo_ms){
    wifi_watchdog_timer_print_info_last_passage=millis();
    if (WiFi.status() == WL_CONNECTED) {otalog("wifi_watchdog_timer_print_info(): wifi_watchdog: WiFi connected! - "+WiFi.localIP().toString());}
    if (WiFi.status() != WL_CONNECTED) otalog("wifi_watchdog_timer_print_info(): wifi_watchdog: Wifi NOT connected since "+String(interruptCounter)+" min");
  }

}


