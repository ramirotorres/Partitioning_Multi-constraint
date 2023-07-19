#ifndef FUNCIONES_H
#define FUNCIONES_H

#include "constantes.h"

double aleatorio_entre(double a, double b);
double aleatorio_01();
void generar_instancia_aleatoria(int n, int k);
double media(vector<double>x);
double desviacion(vector<double>x);
void escribir_matriz(vector<vector<double> > A);
void leer_instancia(string Nombre_Instancia,vector<double>&pesos, vector<double>& esperas, vector<vector<double> >&Distancias, int &n, int &k);

vector<double> leer_vector_archivo(string NOMBRE);
vector<vector<double> > leer_matriz_archivo(string NOMBRE);

void generar_instancia_aleatoria(int n, int k, int numero);
void generar_instancia_aleatoria_generales(double prob_arco, int n, int k, int numero);
void generar_instancia_reducidas(int n, int k,int numero);
#endif // FUNCIONES_H
