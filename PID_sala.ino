//#include <PID_v1.h>

//https://www.unife.it/ing/lm.meccanica/insegnamenti/dinamica-controllo-diagnosi-di-sistemi-b/materiale-didattico/Controllori_PID.pdf

double pv[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double dpv[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  //instead of de(t) for stability

double op[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};




//double action[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; // 1 = direct, -1 = reverse  --> errore = SP-PV -->direct= errore positivo --> azione positiva controllore (per temperature sono tutti diretti)

double err[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // last run error
double P[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // last run proportional term
double I[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // last run integral term
double D[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // last run derivative term

unsigned long pid_run_interval[]={120000,120000,120000,120000,120000,120000,120000,120000,120000,120000,120000,120000,120000,120000,120000,120000};
double pid_last_run_time[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


double op_gpwm1[]={0,0,0,0,0,0,0,0,0,0,0,0};
double op_gpwm2[]={0,0,0,0,0,0,0,0,0,0,0,0};

double op_gpwm1_update_time[]={0,0,0,0,0,0,0,0,0,0,0,0};
double op_gpwm2_update_time[]={0,0,0,0,0,0,0,0,0,0,0,0};


void update_pv() {for (int c=0; c<16;c++) pv[c]=estrap_lin(c); return;}
void update_dpv(){for (int c=0; c<16;c++) dpv[c]=slope_lin(c); return;}
// void update_PID_parameters(){
//   for (int c=0; c<16;c++){
//     Kp[c]=doc["kp"][c];
//     Ki[c]=doc["ki"][c];
//     Kd[c]=doc["kd"][c];
//   }
//   return;
// }


void pid_run(){
  double action[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; // 1 = direct, -1 = reverse  --> errore = SP-PV -->direct= errore positivo --> azione positiva controllore (per temperature sono tutti diretti)
  if (doc["estate"] == 1) for (int i = 0; i < 16; i++) action[i] = -1; //se è estate inverto l'azione
  update_pv();
  update_dpv();
  //update_PID_parameters(); //sarà meglio omettere il passaggio inutile agli array, che quindi non servono.....
  for(int i=0;i<16;i++){
    if (millis()>=pid_last_run_time[i]+pid_run_interval[i]){
      //Serial.println("run PID "+String(i));
      double tempo_base_ = 3600*1000;
      double min_op=0;
      double max_op=100;
      

      err[i]=(double)doc["sp"][i]-pv[i];

      P[i]=(double)doc["kp"][i]*err[i]; //Kp[i]*err[i];

      double dI=(double)doc["ki"][i]*err[i]*(millis()-pid_last_run_time[i])/tempo_base_; //tempo in ore
      I[i]=I[i]+dI;

      D[i]=-(double)doc["kd"][i]*dpv[i];

      //anti wind-up. Se l'uscita del PID esce dai limiti dell'output, non incremento/decremento il termine integrale.
      double op_request=action[i]*(P[i]+I[i]+D[i]); //calcolo l'ipotetico output
      if(abs(op_request-min(max(min_op,op_request),max_op))>10e-5) I[i]=I[i]-dI; //se abs(output-possibile)10e-5, tolgo l'incremento dell'integrale

      op[i]=max(min_op,min(max_op,action[i]*(P[i]+I[i]+D[i]))); //con anti-windup questo sta sempre nei limiti stabiliti --> cmq limito fra 0 e 100



      pid_last_run_time[i]=millis();

      if (i==15) Serial.println(pid_string());// valve_updates_gpwm1();} // temporaneo per debug
    }
  }
  control_strategy_calculations_run();
  valve_updates_gpwm1();
  valve_updates_gpwm2();
  return;
}

//questa converte le uscite dei controllori, calcolando le effettive % alle valvole
void control_strategy_calculations_run(){

  //Strategia split range piano terra
  //termoarredo[0] (0-5) --> 
  //ingresso[1],bagno[2], cucina[3] (5-20) --> 
  //Sala tv[4], centro[5], giovanna[6] (20-100)
  if (op[0]<5)                  {op_gpwm1[0]=op[0]*100/5;    op_gpwm1[1]=0;                     op_gpwm1[2]=0;                     op_gpwm1[3]=0;                     op_gpwm1[4]=0;                       op_gpwm1[5]=0;            op_gpwm1[6]=0;}
  if (op[0]>=5 && op[0]<20)     {op_gpwm1[0]=100;            op_gpwm1[1]=(op[0]-5)*100/(20-5);  op_gpwm1[2]=(op[0]-5)*100/(20-5);  op_gpwm1[3]=(op[0]-5)*100/(20-5);  op_gpwm1[4]=0;                       op_gpwm1[5]=0;            op_gpwm1[6]=0;}
  if (op[0]>=20)                {op_gpwm1[0]=100;            op_gpwm1[1]=100;                   op_gpwm1[2]=100;                   op_gpwm1[3]=100;                   op_gpwm1[4]=(op[0]-20)*100/(100-20); op_gpwm1[5]=op_gpwm1[4];  op_gpwm1[6]=op_gpwm1[4];}

  
  op_gpwm2[2]=op[7]; // camera sara
  op_gpwm2[3]=op[8]; // camera stella
  op_gpwm2[1]=op[2]; //studio

  //PID cam matr.
  op_gpwm2[5]=op[6]; //cabina armadio
  op_gpwm2[7]=op[6]; //circuito 1 cam matr.
  op_gpwm2[8]=op[6]; //circuito 2 cam matr.


  //bagno bimbe
  if (op[3]<5)  {op_gpwm2[0]=op[3]*100/5; op_gpwm2[4]=0;                    }//da OP 0% a OP 5% termoarredo rampa subito al 100%
  if (op[3]>=5) {op_gpwm2[0]=100;         op_gpwm2[4]=(op[3]-5)*100/(100-5);} //poi il pavimento linearmente da 5% a 100%


  //bagno padronale
  if (op[4]<5)  {op_gpwm2[9]=op[4]*100/5; op_gpwm2[6]=0;                    }//da OP 0% a OP 5% termoarredo rampa subito al 100%
  if (op[4]>=5) {op_gpwm2[9]=100;         op_gpwm2[6]=(op[4]-5)*100/(100-5);} //poi il pavimento linearmente da 5% a 100%

  return;
}



void valve_updates_gpwm1(){
  bool proceed=true;
  for(int i=0;i<12;i++) {if (millis()-op_gpwm1_update_time[i]<250) proceed=false;} //blocco la chiamata se è passato troppo poco tempo dalla precedente chiamata
  if (proceed){
    for(int i=0;i<12;i++){
      if (op_gpwm1_update_time[i]<pid_last_run_time[i]){
        String url="http://gpwm1.local/?cmd=set_gpwm;"+String(i)+";"+String(op_gpwm1[i])+";1800";
        //gpwm1.local/?cmd=set_gpwm;x;percentage;cycle_time(s)
        String reply = http_get(url);
        Serial.println("gpwm1 " +reply);
        if (reply.indexOf("Error")>=0) Serial.println("gpwm1 " + String(i)+" update to "+String(op_gpwm1[i])+" % --> FAILED (url="+url+") , (reply="+reply+")");
        //if (!reply.lastIndexOf("executed")>0) Serial.println("gpwm1 " + String(i)+" update to "+String(op_gpwm1[i])+" % --> FAILED (url="+url+") , (reply="+reply+")");
        op_gpwm1_update_time[i]=millis();
        return;
      }
    }
  }
  return;
}

void valve_updates_gpwm2(){
  bool proceed=true;
  for(int i=0;i<12;i++) {if (millis()-op_gpwm2_update_time[i]<250) proceed=false;} //blocco la chiamata se è passato troppo poco tempo dalla precedente chiamata
  if (proceed){
    for(int i=0;i<12;i++){
      if (op_gpwm2_update_time[i]<pid_last_run_time[i]){
        String url="http://gpwm2.local/?cmd=set_gpwm;"+String(i)+";"+String(op_gpwm2[i])+";1800";
        //gpwm2.local/?cmd=set_gpwm;x;percentage;cycle_time(s)
        String reply = http_get(url);
        Serial.println("gpwm2 " +reply);
        if (reply.indexOf("Error")>=0) Serial.println("gpwm2 " + String(i)+" update to "+String(op_gpwm2[i])+" % --> FAILED (url="+url+") , (reply="+reply+")");
        //if (!reply.lastIndexOf("executed")>0) Serial.println("gpwm2 " + String(i)+" update to "+String(op_gpwm2[i])+" % --> FAILED (url="+url+") , (reply="+reply+")");
        op_gpwm2_update_time[i]=millis();
        return;
      }
    }
  }
  return;
}

String pid_string(){
  String res="\r\n";
  res=res+"\r\n sp     :       "; for (int c=0; c<16;c++) res=res+tronca(allunga(String(doc["description"][c]),15," "),15)+"|";
  res=res+"\r\n sp     :       "; for (int c=0; c<16;c++) res=res+allunga(String(doc["sp"][c]),15," ")+"|";
  res=res+"\r\n pv     :       "; for (int c=0; c<16;c++) res=res+allunga(String(pv[c]),15," ")+"|";
  res=res+"\r\n dpv    :       "; for (int c=0; c<16;c++) res=res+allunga(String(dpv[c]),15," ")+"|";
  res=res+"\r\n err    :       "; for (int c=0; c<16;c++) res=res+allunga(String(err[c]),15," ")+"|";
  res=res+"\r\n P      :       "; for (int c=0; c<16;c++) res=res+allunga(String(P[c]),15," ")+"|";
  res=res+"\r\n I      :       "; for (int c=0; c<16;c++) res=res+allunga(String(I[c]),15," ")+"|";
  res=res+"\r\n D      :       "; for (int c=0; c<16;c++) res=res+allunga(String(D[c]),15," ")+"|";
  res=res+"\r\n op     :       "; for (int c=0; c<16;c++) res=res+allunga(String(op[c]),15," ")+"|";
  return res;
}

String get_all_measurements_details(){
  String res="i       descrizione                 pv      sp      op      err     dpv     P       I       D\r\n";
  for (int i=0;i<16;i++){
    res=res+allunga(format_int(i,2),8," ");
    res=res+allunga(String(doc["description"][i]),28," ");
    res=res+allunga(String(pv[i],2),8," ");
    res=res+allunga(String(doc["sp"][i]),8," ");
    res=res+allunga(String(op[i],1),8," ");
    res=res+allunga(String(err[i],1),8," ");
    res=res+allunga(String(dpv[i],1),8," ");
    res=res+allunga(String(P[i],1),8," ");
    res=res+allunga(String(I[i],1),8," ");
    res=res+allunga(String(D[i],1),8," ");
    //res=res+allunga(String(t_filtered[i],4),8," ");
    res=res+"\r\n";
  }
  return res;
}



