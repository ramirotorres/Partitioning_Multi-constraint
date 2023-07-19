#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <iostream>
#include <sstream>
#include <gurobi_c++.h>
#include <vector>
#include <list>
#include <queue>
#include <iostream>
#include<fstream>
#include <cmath>
#include <algorithm>

using namespace std;

///ACTIVAR PLANOS CORTANTES

const bool PLANOS_2P=true;
const bool PLANOS_CICLOS=false;
const bool PLANOS_MOCHILA=false;

///PARAMETROS GUROBI

const double TIEMPO_IP = 2000;
const double GAP=0.01;
const double EPSILON=1e-5;
const double EPS_CORTES=0.1;

///PARAMETROS PROGAMA

const string REPORTE="salidas/Reporte.csv";
const double INF = 1e+20;
const double BIGVALUE=1e+8;
const double FACTOR_VAR=2;

///COLORES

const string RESET="\033[0m";
const string  ROJO="\033[31m";      /* Red */
const string VERDE="\033[32m";     /* Green */
const string  AZUL="\033[94m";     /* Blue */
const string  BLANCO="\033[37m";      /* White */

#endif // CONSTANTES_H
