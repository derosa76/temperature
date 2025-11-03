//ad ogni misura, quindi ad intervalli regolari, viene aggiornate vettore tempi e matrice misure, quindi vengono calcolate : regressione lineare e regressione quadratica (non usata al momento, ma n'si sa mai...)
//in maniera più frequente, operazione veloce, vengono calcolate le estrapolazioni lineari al tempo attuale, usate dai controllori


#include <BasicLinearAlgebra.h>
using namespace BLA;


const int n_rows=60; 
const int matrix_n_cols=16;


double tempi[60];// = new double[n_rows]; //tempo
double matrix[60][16];// = new double[n_rows][matrix_n_cols]; //valori temperature


 double mm[16];
 double qq[16];

 double aa[16];
 double bb[16];
 double cc[16];

 const int tempo_base=3600000; //ms --> per i calcoli regressione, derivate ecc... il tempo è in ore

int nUsedRows = 0;




//void update_t_estr_lin(){if (nUsedRows>2) for (int c=0; c<16;c++) t_estr_lin[c]=estrap_lin(c); return;}

void slope_matrix_push(){ //aggiunge in fondo una riga alla matrice delle temperature (e al vettore dei tempi). Se la matrice è già piena la scorre verso sopra
  if (nUsedRows<n_rows) nUsedRows++;
  else{ // nUsedRows==n_rows
    for (int r=0;r<n_rows-1;r++){
      tempi[r]=tempi[r+1]; //shift vector up
      for (int c=0; c<matrix_n_cols;c++) matrix[r][c]=matrix[r+1][c]; //shift matrix up
    }
  }
  tempi[nUsedRows-1]=millis(); //fill last used value of time vector
  for (int c=0; c<matrix_n_cols;c++) matrix[nUsedRows-1][c]=t[c]; // fill last used row of matrix
  if (nUsedRows>2) run_regressions();
  //Serial.println(slope_string()); // per debug
  //Serial.println("freemem: " + String(esp_get_free_heap_size()) + " bytes");
  return;
}


String slope_string(){
  String res="\r\n";
  for (int r=0;r<nUsedRows;r++){
    res=res+allunga(String(tempi[r]),14," ");
    for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(matrix[r][c]),10," ");
    res=res+"\r\n";
  }
  res=res+"\r\nm     :       "; if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(mm[c]),10," ");
  res=res+"\r\nq     :       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(qq[c]),10," ");
  res=res+"\r\na     :       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(aa[c]),10," ");
  res=res+"\r\nb     :       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(bb[c]),10," ");
  res=res+"\r\nc     :       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(cc[c]),10," ");
  res=res+"\r\nsl_lin:       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(slope_lin(c)),10," ");
  res=res+"\r\nsl_qua:       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(slope_quad(c)),10," ");
  res=res+"\r\nest_li:       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(estrap_lin(c)),10," ");
  res=res+"\r\nest_qu:       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(estrap_quad(c)),10," ");
  //res=res+"\r\nel now:       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(t_estr_lin[c]),10," ");
  //res=res+"\r\nsp    :       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(sp[c]),10," ");
  //res=res+"\r\nop    :       ";if (nUsedRows>2) for (int c=0; c<matrix_n_cols;c++) res=res+allunga(String(op[c]),10," ");
  return res;
}






//matrice per la regressione polinomiale:
//https://muthu.co/maths-behind-polynomial-regression/#:~:text=Polynomial%20regression%20is%20a%20process,is%20a%20set%20of%20coefficients.

//fa le regressioni lineare e quadratica in una sola funzione e va a mettere parametri lineari, parametri quadratici, relative slopes lineare e quadratica e relativi valori calcolati al punto x[n] 
void run_regressions(){
  unsigned long t0=micros();
  int n = nUsedRows;
  double x[n_rows];
  for (int r=0;r<n;r++) x[r]=(tempi[r]-tempi[0])/tempo_base; //genero nuovo vettore tempi, traslato a zero e scalato
  for (int c=0;c<16;c++){
    // Inizializzazione delle sommatorie
    double S1 = 0;  // Σx
    double S2 = 0;  // Σx²
    double S3 = 0;  // Σx³
    double S4 = 0;  // Σx⁴
    double Sy = 0;  // Σy
    double Sxy = 0; // Σxy
    double Sx2y = 0; // Σx²y

    // Calcolo delle sommatorie
    for(int i = 0; i < n; i++) {
      double xi = x[i];
      double yi = matrix[i][c];
      double xi2 = xi * xi;
      double xi3 = xi * xi * xi;
      double xi4 = xi * xi * xi * xi;
      double xy  = xi * yi;
      double x2y = xi * xi * yi;
      
      S1 += xi;
      S2 += xi2;
      S3 += xi3;
      S4 += xi4;
      Sy += yi;
      Sxy += xy;
      Sx2y += x2y;
    }

  //------------------------regressione lineare------------------------//
    Matrix<2, 2> AA;
    AA(0,0)=S2; AA(0,1)=S1;
    AA(1,0)=S1; AA(1,1)=n;

    Matrix<2> vv;
    vv(0)=Sxy;
    vv(1)=Sy;

    Matrix<2, 2> AA_inv=AA;
    Invert(AA_inv);

    Matrix<2> res_lin = AA_inv * vv;

    mm[c]=res_lin(0);
    qq[c]=res_lin(1);

  //------------------------regressione parabolica------------------------//

    Matrix<3, 3> A;
    A(0,0)=S4; A(0,1)=S3; A(0,2)=S2;
    A(1,0)=S3; A(1,1)=S2; A(1,2)=S1;
    A(2,0)=S2; A(2,1)=S1; A(2,2)=n;

    Matrix<3> v;
    v(0)=Sx2y;
    v(1)=Sxy;
    v(2)=Sy;

    Matrix<3, 3> A_inv=A;
    Invert(A_inv);

    Matrix<3> res = A_inv * v;

    aa[c]=res(0);
    bb[c]=res(1);
    cc[c]=res(2);

  }
  //Serial.println("tempo esecuzione calcoli regressione:"+String((micros()-t0)/1000.0)+" ms");
  return;
}

double estrap_lin(int c){
  double p=(millis()-tempi[0])/tempo_base;//x[n-1];
  return mm[c]*p+qq[c];
}


double slope_lin(int c){
  return mm[c];
}

double estrap_quad(int c){
  double p=(millis()-tempi[0])/tempo_base;
  return aa[c]*p*p + bb[c]*p + cc[c];
}

double slope_quad(int c){
  double p=(millis()-tempi[0])/tempo_base; //0,075
  return 2*aa[c]*p + bb[c];
}
