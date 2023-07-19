#ifndef PROBLEMAMIP_H
#define PROBLEMAMIP_H

#include "constantes.h"
#include "funciones.h"


class Modelo2_x
{
private:
  // Puntero al Solver
GRBEnv* env_modelo;
GRBModel*  modelo_ip;
    // Vector de (punteros a las) variables
  vector< vector<GRBVar> >  _x;
  vector< vector<bool> >  Ady_x;

public:
  // Constructor y destructor
  Modelo2_x();
  ~Modelo2_x();

//ingresar parametros al GUROBI: no incluir cortes
void incluir_parametros(bool desactivar_cuts);
  // Definir variables

void crear_vars(vector< vector<double> > Datos, vector<double> pesos, vector<double> espera,int card_V, int grupos);
void crear_restricciones(vector<double>  pesos, vector<double> espera, int card_V, int grupos,vector<vector<double> > WL,vector<vector<double> >WU);
void Pre_procesamiento(vector<double> pesos, int card_V, int grupos);

ostream&  presentar_sol(ostream& os, string nombre,int card_V, int grupos);

void write_MIP(string archivo);
double Obt_funcion_obj();
double Obt_tiempo_IP();
double Obt_gap();

void resolver();
void resolver_con_planos(int& cuts_2p,int& cuts_ciclos, int& cuts_sg,int& cuts_sgg,int & nodecnt,vector<vector<double> > w_pesos, vector<vector<double> > WL,vector<vector<double> > WU);

void  Obt_solucion_vars_Y(bool &optima,vector<vector<vector<int> > > &y_sol);
void Fijar_Vars_Y(vector<vector<vector<int> > > y_sol);
void Cargar_sol_inicial_modelo_Y(vector<vector<vector<int> > > y_sol);

double Lower()
{
    return modelo_ip->get(GRB_DoubleAttr_ObjBound);
}

int ObtN_Vars()
{
    return modelo_ip->get(GRB_IntAttr_NumVars);
}
int ObtN_Restr()
{
    return modelo_ip->get(GRB_IntAttr_NumConstrs);
}

double Obt_Obj()
{
    return modelo_ip->get(GRB_DoubleAttr_ObjVal);
}

double Obt_tiempo()
{
    return modelo_ip->get(GRB_DoubleAttr_Runtime);
}

double Obt_Brecha()
{
    return 100*modelo_ip->get(GRB_DoubleAttr_MIPGap);
}
int status()
{
    return modelo_ip->get(GRB_IntAttr_Status);
}
int Obt_N_Nodos()
{
    return modelo_ip->get(GRB_DoubleAttr_NodeCount);
}

};


class Modelo1_xy
{
private:
  // Puntero al Solver
GRBEnv* env_modelo;
GRBModel*  modelo_ip;
    // Vector de (punteros a las) variables
  vector< vector<GRBVar> >  _x;
  vector< vector<bool> >  Ady_x;
  vector<vector< GRBVar > > _y;

public:
  // Constructor y destructor
  Modelo1_xy();
  ~Modelo1_xy();

//ingresar parametros al GUROBI: no incluir cortes
void incluir_parametros(bool desactivar_cuts);
  // Definir variables

void crear_vars(vector< vector<double> > Datos, vector<double> pesos, vector<double> espera,int card_V, int grupos);
void crear_restricciones(vector<double>  pesos, vector<double> espera, int grupos, int card_V,vector<vector<double> > WL,vector<vector<double> >WU);
void Pre_procesamiento(vector<double> pesos, int card_V, int grupos);
ostream&  presentar_sol(ostream& os, string nombre,int card_V, int grupos);

void write_MIP(string archivo);
double Obt_funcion_obj();
double Obt_tiempo_IP();
double Obt_gap();

void resolver_con_planos(int& cuts_2p,int& cuts_ciclos, int& cuts_sg,int& cuts_sgg,int & nodecnt,vector<vector<double> > w_pesos, vector<vector<double> > WL,vector<vector<double> > WU);
void resolver();


void  Obt_solucion_vars_Y(bool &optima,vector<vector<vector<int> > > &y_sol);
void Fijar_Vars_Y(vector<vector<vector<int> > > y_sol);
void Cargar_sol_inicial_modelo_Y(vector<vector<vector<int> > > y_sol);

double Lower()
{
    return modelo_ip->get(GRB_DoubleAttr_ObjBound);
}

int ObtN_Vars()
{
    return modelo_ip->get(GRB_IntAttr_NumVars);
}
int ObtN_Restr()
{
    return modelo_ip->get(GRB_IntAttr_NumConstrs);
}

double Obt_Obj()
{
    return modelo_ip->get(GRB_DoubleAttr_ObjVal);
}

double Obt_tiempo()
{
    return modelo_ip->get(GRB_DoubleAttr_Runtime);
}

double Obt_Brecha()
{
    return 100*modelo_ip->get(GRB_DoubleAttr_MIPGap);
}

int status()
{
    return modelo_ip->get(GRB_IntAttr_Status);
}

int Obt_N_Nodos()
{
    return modelo_ip->get(GRB_DoubleAttr_NodeCount);
}

};


///***************************************************************
///*****CALLBACKS************************************************
///**************************************************************

//Clase para la función callback de los 2 modelos

class planos_xy : public GRBCallback
{
public:
    vector< vector<GRBVar> >  Xvars;///VARIABLES X_ij
    vector<vector< GRBVar > > Yvars;///VAARIABLES Y_ic
    vector< vector<bool> >  Ady_Xvars;/// MATRIZ DE ADYACENCIA VARIABLES X_ij


	vector< vector<double> > Xreal_ij; /// valores de la variable X_ij (xvars)
	vector< vector<double> > Yreal_ci; /// valores de la variable Y_ci (xvars)

    int& cuts_2p;///NUMERO DE CORTE 2P
    int& cuts_ciclos;///NUMERO DE CORTES POR CICLOS
    int& cuts_subg;///NUMERO DE CORTES POR SUBGRAFOS
    int& cuts_mochila;///CORTES POR MOCHILA
	int& nodecnt;///Número de nodos de Branch and Cut explorados en la optimización más reciente.;
///pesos
    vector<vector<double> > wp;///MATRIZ DE PESOS
    vector<vector<double> > WL_cb;///MATRIZ DE COTAS INFERIORES
    vector<vector<double> > WU_cb;///MATRIZ DE COTAS SUPERIORES


	planos_xy(vector<vector<bool> >& _Ady, vector<vector<GRBVar> >& _x,vector<vector<GRBVar> > & _y, int& c2p, int& cc,int& csg,int& cknp, int& nodecntx,vector<vector<double> >& pesos,vector<vector<double> >& WL,vector<vector<double> >& WU):
	Ady_Xvars(_Ady),Xvars(_x), Yvars(_y),cuts_2p(c2p),cuts_ciclos(cc),cuts_subg(csg),cuts_mochila(cknp),nodecnt(nodecntx),wp(pesos),WL_cb(WL),WU_cb(WU)
    {

		cuts_2p=0;
		cuts_ciclos=0;
        cuts_subg=0;
        cuts_mochila=0;

        cout<<"... Creando callback modelo XY"<<endl<<endl;

		Xreal_ij.resize(_x.size());

		for (unsigned int i = 0; i<_x.size(); i++)
		{
			Xreal_ij[i].resize(_x[i].size(),0);
		}


		Yreal_ci.resize(_y.size());
		for (unsigned int c = 0; c<_y.size(); c++)
		{
			Yreal_ci[c].resize(_y[c].size(),0);
		}
    }

protected:
	//Planos cortantes
	void callback();
};




class planos_proyeccion : public GRBCallback
{
public:
    vector< vector<GRBVar> >  Xvars;///VARIABLES X_ij
	vector< vector<double> > Xreal_ij; /// valores de la variable X_ij (xvars)
    vector< vector<bool> >  Ady_Xvars;/// MATRIZ DE ADYACENCIA VARIABLES X_ij

    int& cuts_2p;///NUMERO DE CORTE 2P
    int& cuts_ciclos;///NUMERO DE CORTES POR CICLOS
    int& cuts_subg;///NUMERO DE CORTES POR SUBGRAFOS
    int& cuts_mochila;///CORTES POR SUBGRAFOS GLOBALES
	int& nodecnt;///Número de nodos de Branch and Cut explorados en la optimización más reciente.;
///pesos
    vector<vector<double> > wp;///MATRIZ DE PESOS
    vector<vector<double> > WL_cb;///MATRIZ DE COTAS INFERIORES
    vector<vector<double> > WU_cb;///MATRIZ DE COTAS SUPERIORES


    planos_proyeccion(vector<vector<bool> >& _Ady, vector<vector<GRBVar> >& _x, int& c2p, int& cc,int& csg,int& cknp, int& nodecntx,vector<vector<double> >& pesos,vector<vector<double> >& WL,vector<vector<double> >& WU):
	Ady_Xvars(_Ady),Xvars(_x),cuts_2p(c2p),cuts_ciclos(cc),cuts_subg(csg),cuts_mochila(cknp),nodecnt(nodecntx),wp(pesos),WL_cb(WL),WU_cb(WU)
    {

		cuts_2p=0;
		cuts_ciclos=0;
        cuts_subg=0;
        cuts_mochila=0;

        cout<<"... Creando callback modelo XY"<<endl<<endl;

		Xreal_ij.resize(_x.size());

		for (unsigned int i = 0; i<_x.size(); i++)
		{
			Xreal_ij[i].resize(_x[i].size(),0);
		}

    }

protected:
	//Planos cortantes
	void callback();
};


#endif  PROBLEMAMIP_H
