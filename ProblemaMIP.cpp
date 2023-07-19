#include "ProblemaMIP.h"


using namespace std;

Modelo1_xy::Modelo1_xy()
{
env_modelo = new GRBEnv();
modelo_ip = new GRBModel(*env_modelo);
modelo_ip->set(GRB_StringAttr_ModelName, "Modelo Particionamiento PII");
modelo_ip->set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
}

Modelo1_xy::~Modelo1_xy()
{
      // liberar variables
 for(unsigned int i=0; i<_y.size(); i++)
  {
    _y[i].clear();
  }
_y.clear();


  for(unsigned int i=0; i<_x.size(); i++)
  {
    _x[i].clear();
    Ady_x[i].clear();
  }
_x.clear();
Ady_x.clear();


delete modelo_ip;
delete env_modelo;
}

void  Modelo1_xy::incluir_parametros(bool desactivar_cuts)
{
    modelo_ip->getEnv().resetParams();

    if(desactivar_cuts==true)
    {
    cout << "Desactivación de planos del gurobi" << endl;

    modelo_ip->getEnv().set(GRB_IntParam_CliqueCuts, 0);
    modelo_ip->getEnv().set(GRB_IntParam_FlowCoverCuts, 0);
    modelo_ip->getEnv().set(GRB_IntParam_FlowPathCuts, 0);
    modelo_ip->getEnv().set(GRB_IntParam_ImpliedCuts, 0);
    modelo_ip->getEnv().set(GRB_IntParam_MIPSepCuts, 0);
    modelo_ip->getEnv().set(GRB_IntParam_NetworkCuts, 0);
    ///modelo_ip->getEnv().set(GRB_IntParam_ModKCuts, 0);
   /// modelo_ip->getEnv().set(GRB_IntParam_GUBCoverCuts, 0);
    ///modelo_ip->getEnv().set(GRB_IntParam_SubMIPCuts, 0);

    modelo_ip->getEnv().set(GRB_IntParam_MIPFocus, 1);
    modelo_ip->getEnv().set(GRB_IntParam_Symmetry, 2);
    modelo_ip->getEnv().set(GRB_DoubleParam_NodefileStart, 0.5);

    /// Permite habilitar los cortes
	modelo_ip->getEnv().set(GRB_IntParam_PreCrush, 1);
	///Llamado de los planos cortantes
    modelo_ip->getEnv().set(GRB_IntParam_LazyConstraints, 1);

    }
	//modelo_ip->getEnv().set(GRB_IntParam_CoverCuts,0);
    //modelo_ip->getEnv().set(GRB_IntParam_MIRCuts,0);
	//modelo_ip->getEnv().set(GRB_IntParam_GomoryPasses,0);
    ///model->getEnv().set(GRB_IntParam_Cuts, 2); // sin cortes

    modelo_ip->getEnv().set(GRB_IntParam_Seed, 1);
    modelo_ip->getEnv().set(GRB_DoubleParam_TimeLimit, TIEMPO_IP);//limita el tiempo de ejecucion del modelo
    modelo_ip->getEnv().set(GRB_DoubleParam_MIPGap, GAP);

    modelo_ip->update();
}


//Creación de las variables para el modelo
void Modelo1_xy::crear_vars(vector< vector<double> > Datos, vector<double> pesos, vector<double> espera,int card_V, int grupos)
{
	//x[l][i][j]: variable binaria que indica que es 1 si la arista (i,j) pertenece al clique l, 0 caso contrario

    _x.resize(card_V);
    Ady_x.resize(card_V);

    for(unsigned int i=0;i<_x.size();i++)
    {
        _x[i].resize(card_V);
        Ady_x[i].resize(card_V,false);
    }
    _y.resize(grupos);
    for(unsigned int i=0;i<_y.size();i++)
    {
        _y[i].resize(card_V);
    }


		for (int i = 0; i< card_V; i++)
		{
			for (int j = i+1; j < card_V; j++)
			{
                if(Datos[i][j]>0)
                {
                    ostringstream nombre_var;
                    nombre_var << "x_"<< i << "_" << j;
                    _x[i][j] = modelo_ip->addVar(0.0, 1.0, Datos[i][j], GRB_BINARY, nombre_var.str().c_str());
                    Ady_x[i][j]=Ady_x[j][i]=true;
                }

			}
		}

	cout << "Creada variable x" << endl;
	//y[c][i]: variable binaria, cuyas filas representan los cliques y las columnas los nodos.
	//		  y[c][i]=1 si el nodo i pertenece al clique c; o caso contrario

	for (int l = 0; l<grupos; l++)
	{
		for (int i = 0; i<card_V; i++)
		{
			ostringstream nombre_var2;
			nombre_var2 << "y^" << l << "_" << i;
			_y[l][i] = modelo_ip->addVar(0.0, 1.0, 0.0, GRB_BINARY, nombre_var2.str().c_str()); // guardar puntero en matriz _vars
		}
	}
	cout << "Creada variable y" << endl;
	modelo_ip->update();

}

void Modelo1_xy::Pre_procesamiento(vector<double> pesos, int card_V, int grupos)
{
	///PROCESO DE PRE-PROCESAMIENTO
	///SE GENERA UN PRE-ASIGNACION DE LOS NODOS CON MAYOR PESO
	///nodo con mayor peso a la clique 1
	///el segundo con mayor peso, a la clique 1 o 2, ....

  ///definiendo prioridades de ramificación


	vector<double> pre(pesos.size());
	vector<int> asignacion(grupos);

	pre=pesos;
	int mg;
	double max_peso;
	for(int g=0;g<grupos;g++)
	{
	max_peso=-INF;
	mg=-1;
        for(unsigned int i=0;i<pre.size();i++)
        {
            if(pre[i]>max_peso)
            {
                max_peso=pre[i];
                mg=i;
            }
        }
    asignacion[g]=mg;
    pre[mg]=-INF;
    }

    for (int l = 0; l<grupos; l++)
	{
		for (int i = 0; i<card_V; i++)
		{
	        _y[l][i].set(GRB_IntAttr_BranchPriority,2);
		}
	}

    _y[0][asignacion[0]].set(GRB_DoubleAttr_LB,1.0);
    for(unsigned int i=0;i<asignacion.size();i++)
    {
        if(i>0)
        {
            for(int l=0;l<=i;l++)
            {
                _y[l][asignacion[i]].set(GRB_IntAttr_BranchPriority,4);
            }
        }
        for(int l=i+1;l<grupos;l++)
        {
            _y[l][asignacion[i]].set(GRB_DoubleAttr_UB,0.0);
        }
    }


    for (int i = 0; i< card_V; i++)
		{
			for (int j = i+1; j < card_V; j++)
			{
                if(Ady_x[i][j]==true)
                    _x[i][j].set(GRB_IntAttr_BranchPriority,0);
			}
		}


    modelo_ip->update();
}

//Creación de las restricciones para el modelo
void Modelo1_xy::crear_restricciones(vector<double>  pesos, vector<double> espera, int grupos, int card_V,vector<vector<double> > WL,vector<vector<double> >WU)
{

///restriccion de asignacion de un nodo a un grupo

	for (int i = 0; i < card_V; i++)
	{
		ostringstream restriccion2;
		restriccion2 << "Rest_1_" << i;
		GRBLinExpr res2 = 0;
		for (int c = 0; c < grupos; c++)
		{
			res2 += _y[c][i];
		}
		modelo_ip->addConstr(res2 == 1.0, restriccion2.str());
	}

///restriccion de acoplamiento

    for(unsigned int i=0;i<_x.size();i++)
    {
        for(unsigned int j=i+1;j<_x[i].size();j++)
        {
            for (int c = 0; c < grupos; c++)
            {
                if(Ady_x[i][j]==true)
                {
                    ostringstream restriccion3;
                    restriccion3 << "Rest_2_"<<c<<"_"<<i<<"_"<<j;
                    modelo_ip->addConstr(_y[c][i]+_y[c][j]-_x[i][j]<=1.0, restriccion3.str());
                }
			}
        }
    }


	///restriccion4

	for (int c = 0; c < grupos; c++)
	{
		ostringstream restriccion4up, restriccion4low,restriccion5up, restriccion5low;
		restriccion4up << "Rest4_up_" << c;
		restriccion4low << "Rest4_low_" << c;
        restriccion5up << "Rest5_up_" << c;
		restriccion5low << "Rest5_low_" << c;

		GRBLinExpr res4 = 0,res5=0;
		for (int i = 0; i < card_V; i++)
		{
			res4 += pesos[i] * _y[c][i];
			res5 += espera[i] * _y[c][i];
		}
		modelo_ip->addConstr(res4>=WL[0][c], restriccion4low.str());
		modelo_ip->addConstr(res4<= WU[0][c], restriccion4up.str());

		modelo_ip->addConstr(res5>=WL[1][c], restriccion5low.str());
		modelo_ip->addConstr(res5<= WU[1][c], restriccion5up.str());
	}

	modelo_ip->update();

}

void Modelo1_xy::resolver()
{
        modelo_ip->reset();
		modelo_ip->setCallback(NULL);
		modelo_ip->optimize();
}

void Modelo1_xy::resolver_con_planos(int& cuts_2p,int& cuts_ciclos, int& cuts_sg,int& cuts_sgg,int & nodecnt,vector<vector<double> > w_pesos, vector<vector<double> > WL,vector<vector<double> > WU)
{

    modelo_ip->getEnv().set(GRB_IntParam_Method,2);
    modelo_ip->getEnv().set(GRB_IntParam_PreCrush, 1);
    cout << "... Incluyendo Planos Cortantes " << endl;
    nodecnt=0, cuts_2p=cuts_ciclos=cuts_sg=cuts_sgg=0;
    planos_xy cb_xy = planos_xy (Ady_x,_x,_y,cuts_2p,cuts_ciclos,cuts_sg,cuts_sgg, nodecnt,w_pesos,WL,WU);
    modelo_ip->setCallback(&cb_xy);
    modelo_ip->update();
    modelo_ip->optimize();

    cout<<"CONTADORES FINALES:"<<endl
        <<"CORTES: "<<cuts_2p<<endl
        <<"NODOS: "<<nodecnt<<endl<<endl;
// do IIS
    int status = modelo_ip->get(GRB_IntAttr_Status);
    if(status==GRB_INFEASIBLE)
    {
    //write_MIP("Horarios_Fase1.lp");
    cout << "MODELO INFACTIBLE... "<<endl;
    string iis;
    cout<<"CALCULAR IIS?(s/n):";
    //cin>>iis;
    iis="n";
    if((iis=="s")or(iis=="S"))
    {
    cout<<"CALCULANDO IIS" << endl;
    modelo_ip->computeIIS();
    cout << "LAS SIGUIENTES RESTRICCIONES GENERAN INFACTIBILIDAD.. "<<endl;
    ofstream f_inf("Generan_infactibilidad_xy.txt");
    GRBConstr* c=0;
    c = modelo_ip->getConstrs();
        for (int i = 0; i < modelo_ip->get(GRB_IntAttr_NumConstrs); ++i)
        {
          if (c[i].get(GRB_IntAttr_IISConstr) == 1)
          {
            f_inf << c[i].get(GRB_StringAttr_ConstrName) << endl;
          }
        }
    f_inf.close();
    cout<<"archivo generado"<<endl;
     }
     }
}


void Modelo1_xy::write_MIP(string archivo)
{
    modelo_ip->write(archivo);
}

//Presentación de los parámetros dados para el modelo
ostream&  Modelo1_xy::presentar_sol(ostream& os, string nombre,int card_V, int grupos)
{

	int st = modelo_ip->get(GRB_IntAttr_Status);

    os<<endl<<endl<< nombre << endl;

	if ((st == 2) || (st >= 7) || (modelo_ip->get(GRB_DoubleAttr_MIPGap)<1e+20))
	{
		os  << "status=" << st << endl
			<< "F objetivo=" << modelo_ip->get(GRB_DoubleAttr_ObjVal) << endl
			<< "Variables: " << modelo_ip->get(GRB_IntAttr_NumVars) << endl
			<< "Restricciones: " << modelo_ip->get(GRB_IntAttr_NumConstrs) << endl
			<< "Tiempo=" << modelo_ip->get(GRB_DoubleAttr_Runtime) << endl
			<< "Gap=" << 100 * modelo_ip->get(GRB_DoubleAttr_MIPGap) << endl;

		os << "\n\n";
		for (int c = 0; c < grupos; c++)
		{
			for (int i = 0; i < card_V; i++)
			{
				double val = 0.0;
				val = _y[c][i].get(GRB_DoubleAttr_X);
				if (val > 0.1)
				{
					string s = _y[c][i].get(GRB_StringAttr_VarName);
					os << s << ":   " << val << endl;
				}
			}
		}
		os << endl << "....." << endl;


			for (int i = 0; i < card_V; i++)
			{
				for (int j = i + 1; j < card_V; j++)
				{
                    if(Ady_x[i][j]==true)
                    {
                        double val = 0.0;
                        val = _x[i][j].get(GRB_DoubleAttr_X);
                        if (val > 0.1)
                        {
                            string s = _x[i][j].get(GRB_StringAttr_VarName);
                            os << s << ":   " << val << endl;
                        }
                    }
				}
			}
			os << endl << "....." << endl;


	}

	else
		os << "sin solución...." << endl;

return os;

}


///MODELO 2:PROYECCION

Modelo2_x::Modelo2_x()
{
env_modelo = new GRBEnv();
modelo_ip = new GRBModel(*env_modelo);
modelo_ip->set(GRB_StringAttr_ModelName, "Modelo Particionamiento Proyección");
modelo_ip->set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
}

Modelo2_x::~Modelo2_x()
{

  for(unsigned int i=0; i<_x.size(); i++)
  {
    _x[i].clear();
    Ady_x[i].clear();
  }
_x.clear();
Ady_x.clear();

delete modelo_ip;
delete env_modelo;
}

void  Modelo2_x::incluir_parametros(bool desactivar_cuts)
{
    modelo_ip->getEnv().resetParams();

    if(desactivar_cuts==true)
    {
    cout << "Desactivación de planos del gurobi" << endl;

    modelo_ip->getEnv().set(GRB_IntParam_CliqueCuts, 0);
    modelo_ip->getEnv().set(GRB_IntParam_FlowCoverCuts, 0);
    modelo_ip->getEnv().set(GRB_IntParam_FlowPathCuts, 0);
    modelo_ip->getEnv().set(GRB_IntParam_ImpliedCuts, 0);
    modelo_ip->getEnv().set(GRB_IntParam_MIPSepCuts, 0);
    modelo_ip->getEnv().set(GRB_IntParam_NetworkCuts, 0);
    ///modelo_ip->getEnv().set(GRB_IntParam_ModKCuts, 0);
    ///modelo_ip->getEnv().set(GRB_IntParam_GUBCoverCuts, 0);
    ///modelo_ip->getEnv().set(GRB_IntParam_SubMIPCuts, 0);

    modelo_ip->getEnv().set(GRB_IntParam_MIPFocus, 1);
    modelo_ip->getEnv().set(GRB_IntParam_Symmetry, 2);
    modelo_ip->getEnv().set(GRB_DoubleParam_NodefileStart, 0.5);

    /// Permite habilitar los cortes
	modelo_ip->getEnv().set(GRB_IntParam_PreCrush, 1);
	///Llamado de los planos cortantes
    modelo_ip->getEnv().set(GRB_IntParam_LazyConstraints, 1);

    }
    /// model->getEnv().set(GRB_IntParam_Cuts, 2); // sin cortes
    /// modelo_ip->getEnv().set(GRB_IntParam_Threads,1); // solo un procesador de los 4 disponibles en mi maquina
    /// modelo_ip->getEnv().set(GRB_IntParam_Presolve, 0);// se apaga el presolve

    modelo_ip->getEnv().set(GRB_IntParam_Seed, 1);
    modelo_ip->getEnv().set(GRB_DoubleParam_TimeLimit, TIEMPO_IP);//limita el tiempo de ejecucion del modelo
    modelo_ip->getEnv().set(GRB_DoubleParam_MIPGap, GAP);

    modelo_ip->update();
}


//Creación de las variables para el modelo
void Modelo2_x::crear_vars(vector< vector<double> > Datos, vector<double> pesos, vector<double> espera,int card_V, int grupos)
{
	//x[l][i][j]: variable binaria que indica que es 1 si la arista (i,j) pertenece al clique l, 0 caso contrario

int n_nodos=card_V+grupos;
    _x.resize(n_nodos);
    Ady_x.resize(n_nodos);

    for(unsigned int i=0;i<_x.size();i++)
    {
        _x[i].resize(n_nodos);
        Ady_x[i].resize(n_nodos,false);
    }


    for (int i = 0; i< n_nodos; i++)
	{
		for (int j = i+1; j < n_nodos; j++)
		{
            ostringstream nombre_var;
            if(i<card_V)
            {
                if(j<card_V)
                {
                    if(Datos[i][j]>0)
                    {
                        nombre_var << "x_"<< i << "_" << j;
                        _x[i][j] = modelo_ip->addVar(0.0, 1.0, Datos[i][j], GRB_BINARY, nombre_var.str().c_str());
                        Ady_x[i][j] = Ady_x[j][i] = true;
                    }
                    else
                    {
                        Ady_x[i][j] = Ady_x[j][i] =false;
                    }

                }

                else
                {
                    nombre_var << "x_"<< i << "_d_" << j;
                    _x[i][j] = modelo_ip->addVar(0.0, 1.0, 0.0, GRB_BINARY, nombre_var.str().c_str());
                    Ady_x[i][j] =Ady_x[j][i] = true;
                }
            }
            else
            {
                    nombre_var << "xd_"<< i << "_d_" << j;
                    _x[i][j] = modelo_ip->addVar(0.0, 1.0, BIGVALUE, GRB_BINARY, nombre_var.str().c_str());
                    Ady_x[i][j] = Ady_x[j][i] = true;

            }
        }
    }

	cout << "Creada variable x" << endl;
	modelo_ip->update();

}

void Modelo2_x::Pre_procesamiento(vector<double> pesos, int card_V, int grupos)
{

	///PROCESO DE PRE-PROCESAMIENTO
	///SE GENERA UN PRE-ASIGNACION DE LOS NODOS CON MAYOR PESO
	///nodo con mayor peso a la clique 1
	///el segundo con mayor peso, a la clique 1 o 2, ....

  ///definiendo prioridades de ramificación


	vector<double> pre(pesos.size());
	vector<int> asignacion(grupos);

	pre=pesos;
	int mg;
	double max_peso;
	for(int g=0;g<grupos;g++)
	{
	max_peso=-INF;
	mg=-1;
        for(unsigned int i=0;i<pre.size();i++)
        {
            if(pre[i]>max_peso)
            {
                max_peso=pre[i];
                mg=i;
            }
        }
    asignacion[g]=mg;
    pre[mg]=-INF;
    }

    for (int l = 0; l<grupos; l++)
	{
		for (int i = 0; i<card_V; i++)
		{
            if(Ady_x[i][card_V+l]==true)
                _x[i][card_V+l].set(GRB_IntAttr_BranchPriority,2);
		}
	}

    _x[ asignacion[0] ][card_V].set(GRB_DoubleAttr_LB,1.0);

    for(unsigned int i=0;i<asignacion.size();i++)
    {
        if(i>0)
        {
            for(int l=0;l<=i;l++)
            {
                if(Ady_x[asignacion[i]][card_V+l]==true)
                    _x[asignacion[i]][card_V+l].set(GRB_IntAttr_BranchPriority,4);
            }
        }
        for(int l=i+1;l<grupos;l++)
        {
            if(Ady_x[asignacion[i]][card_V+l]==true)
                _x[asignacion[i]][card_V+l].set(GRB_DoubleAttr_UB,0.0);
        }
    }


    for (int i = 0; i< card_V; i++)
		{
			for (int j = i+1; j < card_V; j++)
			{
                if(Ady_x[i][j]==true)
                    _x[i][j].set(GRB_IntAttr_BranchPriority,0);
			}
		}


    modelo_ip->update();
}


//Creación de las restricciones para el modelo
void Modelo2_x::crear_restricciones(vector<double>  pesos, vector<double> espera, int card_V, int grupos,vector<vector<double> > WL,vector<vector<double> >WU)
{

///restriccion de asignacion de un nodo a un grupo
/*
for (int i = 0; i < card_V+grupos-2; i++)
    for (int j = i+1; j < card_V+grupos-1; j++)
        for (int l = j+1; l < card_V+grupos; l++)
        {
            ostringstream restr11,restr12,restr13;
            restr11 << "Rest_11_" <<i<<"_"<<j<<"_"<<l;
            restr12 << "Rest_12_" <<i<<"_"<<j<<"_"<<l;
            restr13 << "Rest_13_" <<i<<"_"<<j<<"_"<<l;
            modelo_ip->addConstr(_x[i][j]+_x[j][l]-_x[i][l] <= 1.0, restr11.str());
            modelo_ip->addConstr(_x[i][j]-_x[j][l]+_x[i][l] <= 1.0, restr12.str());
            modelo_ip->addConstr(-_x[i][j]+_x[j][l]+_x[i][l] <= 1.0, restr13.str());
        }
*/


///REDUCCION i,j en card_V, y l nodos dummies.
for (int i = 0; i < card_V-1; i++)
    for (int j = i+1; j < card_V; j++)
        if(Ady_x[i][j]==true)
        {
            for (int l = card_V; l < card_V+grupos; l++)
            {
                ostringstream restr11,restr12,restr13;
                restr11 << "Rest_11_" <<i<<"_"<<j<<"_"<<l;
                restr12 << "Rest_12_" <<i<<"_"<<j<<"_"<<l;
                restr13 << "Rest_13_" <<i<<"_"<<j<<"_"<<l;
                modelo_ip->addConstr(_x[i][j]+_x[j][l]-_x[i][l] <= 1.0, restr11.str());
                modelo_ip->addConstr(_x[i][j]-_x[j][l]+_x[i][l] <= 1.0, restr12.str());
                modelo_ip->addConstr(-_x[i][j]+_x[j][l]+_x[i][l] <= 1.0, restr13.str());
            }
        }

	for (int i = 0; i < card_V; i++)
	{
		ostringstream restriccion2;
		restriccion2 << "Rest_2_" << i;
		GRBLinExpr res2 = 0;
		for (int c = 0; c < grupos; c++)
		{
			res2 += _x[i][card_V+c];
		}
		modelo_ip->addConstr(res2 == 1.0, restriccion2.str());
	}
	//restriccion4
	double WLp, WUp,WLe,WUe;

	for (int c = 0; c < grupos; c++)
	{
		ostringstream restriccion4up, restriccion4low,restriccion5up, restriccion5low;
		restriccion4up << "Rest4_up_" << c;
		restriccion4low << "Rest4_low_" << c;
        restriccion5up << "Rest5_up_" << c;
		restriccion5low << "Rest5_low_" << c;

		GRBLinExpr res4 = 0,res5=0;
		for (int i = 0; i < card_V; i++)
		{
			res4 += pesos[i] * _x[i][card_V+c];
			res5 += espera[i] * _x[i][card_V+c];
		}
		modelo_ip->addConstr(res4>=WL[0][c], restriccion4low.str());
		modelo_ip->addConstr(res4<= WU[0][c], restriccion4up.str());

		modelo_ip->addConstr(res5>=WL[1][c], restriccion5low.str());
		modelo_ip->addConstr(res5<= WU[1][c], restriccion5up.str());
	}

	modelo_ip->update();

}

void Modelo2_x::resolver()
{
        modelo_ip->reset();
		modelo_ip->setCallback(NULL);
		modelo_ip->optimize();
}

void Modelo2_x::resolver_con_planos(int& cuts_2p,int& cuts_ciclos, int& cuts_sg,int& cuts_knp,int & nodecnt,vector<vector<double> > w_pesos, vector<vector<double> > WL,vector<vector<double> > WU)
{
    modelo_ip->getEnv().set(GRB_IntParam_Method,2);
    modelo_ip->getEnv().set(GRB_IntParam_PreCrush, 1);
    cout << "... Incluyendo Planos Cortantes " << endl;
    nodecnt=0, cuts_2p=cuts_ciclos=cuts_sg=cuts_knp=0;
    planos_proyeccion cb_proy = planos_proyeccion (Ady_x,_x,cuts_2p,cuts_ciclos,cuts_sg,cuts_knp, nodecnt,w_pesos,WL,WU);
    modelo_ip->setCallback(&cb_proy);
    modelo_ip->update();
    modelo_ip->optimize();

    cout<<"CONTADORES FINALES:"<<endl
        <<"CORTES: "<<cuts_2p<<endl
        <<"NODOS: "<<nodecnt<<endl<<endl;
// do IIS
    int status = modelo_ip->get(GRB_IntAttr_Status);
    if(status==GRB_INFEASIBLE)
    {
    //write_MIP("Horarios_Fase1.lp");
    cout << "MODELO INFACTIBLE... "<<endl;
    string iis;
    cout<<"CALCULAR IIS?(s/n):";
    ///cin>>iis;
    iis="n";
    if((iis=="s")or(iis=="S"))
    {
    cout<<"CALCULANDO IIS" << endl;
    modelo_ip->computeIIS();
    cout << "LAS SIGUIENTES RESTRICCIONES GENERAN INFACTIBILIDAD.. "<<endl;
    ofstream f_inf("Generan_infactibilidad_xy.txt");
    GRBConstr* c=0;
    c = modelo_ip->getConstrs();
        for (int i = 0; i < modelo_ip->get(GRB_IntAttr_NumConstrs); ++i)
        {
          if (c[i].get(GRB_IntAttr_IISConstr) == 1)
          {
            f_inf << c[i].get(GRB_StringAttr_ConstrName) << endl;
          }
        }
    f_inf.close();
    cout<<"archivo generado"<<endl;
     }
     }
}

void Modelo2_x::write_MIP(string archivo)
{
    modelo_ip->write("Modelo_proy.lp");
}

//Presentación de los parámetros dados para el modelo
ostream&  Modelo2_x::presentar_sol(ostream& os, string nombre,int card_V, int grupos)
{

	int st = modelo_ip->get(GRB_IntAttr_Status);

os<< nombre << endl;
	if ((st == 2) || (st >= 7) || (modelo_ip->get(GRB_DoubleAttr_MIPGap)<1e+20))
	{

		os << "status=" << st << endl
			<< "F objetivo=" << modelo_ip->get(GRB_DoubleAttr_ObjVal) << endl
			<< "Variables: " << modelo_ip->get(GRB_IntAttr_NumVars) << endl
			<< "Restricciones: " << modelo_ip->get(GRB_IntAttr_NumConstrs) << endl
			<< "Tiempo=" << modelo_ip->get(GRB_DoubleAttr_Runtime) << endl
			<< "Gap=" << 100 * modelo_ip->get(GRB_DoubleAttr_MIPGap) << endl;

		os << "\n\n";

			for (int i = 0; i < card_V+grupos; i++)
			{
				for (int j = i + 1; j < card_V+grupos; j++)
				{
                    if(Ady_x[i][j]==true)
                    {
                        double val = 0.0;
                        val = _x[i][j].get(GRB_DoubleAttr_X);
                        if (val > 0.1)
                        {
                            string s = _x[i][j].get(GRB_StringAttr_VarName);
                            os << s << ":   " << val << endl;
                        }
                    }
				}
			}
			os << endl << "....." << endl;


	}

	else
		os << "sin solución...." << endl;
return os;

}

//Creación de las desigualdades triangulares


///****************************************************************************
///******CALLBACKS PARA LOS MODELOS *******************************************
///****************************************************************************

void planos_xy::callback()
{
		//Terminación implementado para detener la optimización del modelo cuando se satisface la condición de que
		//nodecnt % 100 == 1
		if (where == GRB_CB_MIP)
		{
			//Callback general para branch-and-bound
			///callback codes
			// IN MIP (actualización del número de nodos) - Cuando se navega en el árbol
			nodecnt = (int)getDoubleInfo(GRB_CB_MIP_NODCNT); //obtener la información de la cuenta explorada actual del nodo.
            int vl=GRB_CB_MIP_CUTCNT;
			if (nodecnt % 100 == 1) // que nos dice esta restricción¿?{
			{
				//cout << "Lazy constraints added: " << lazys_added << endl;
				cout <<AZUL<<" | 2P="<<cuts_2p<<" | CICLOS="<< cuts_ciclos<<"| SUB="<<cuts_subg<<" | KNP="<<cuts_mochila
                     <<" | NODOS="<<nodecnt<<endl<<RESET;
            }
        }

    else if (where == GRB_CB_MIPNODE)
    {

            if (getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL)
            {


                for (unsigned int  i= 0; i < Xreal_ij.size(); i++)
                {
					for (unsigned int  j= i+1; j < Xreal_ij[i].size(); j++)
					{
                        if(Ady_Xvars[i][j]==true)
                        {
                            Xreal_ij[i][j] =Xreal_ij[j][i]= getNodeRel(Xvars[i][j]);
                            ///cout<<"X i="<<i<<"  j="<<j<<"="<<Xreal_ij[i][j]<<endl;
                        }
                    }
                }

                for (unsigned int  c= 0; c < Yreal_ci.size(); c++)
					{
						for (unsigned int  i= 0; i < Yreal_ci[c].size(); i++)
						{
                            Yreal_ci[c][i] = getNodeRel(Yvars[c][i]);
                           /// if((Yreal_ci[c][i]>0)and(Yreal_ci[c][i]<1))
                             ///   cout<<"Y c="<<c<<" i="<<i<<" = "<<Yreal_ci[c][i]<<endl;
						}
                    }

        bool desigualdad_t1=false;

///***************************************************************************************
///*******************CORTES TEOREMA 4.2 *************************************************
///***************************************************************************************

if(PLANOS_2P==true)
{
    if(desigualdad_t1==false)
    {
        double valor=0;


        for (int c = 0; c < Yvars.size(); c++)///conjunto R
        {
		/// Creación del conjunto W
        list<int> conj_W;
            for (int w =0; w < Yvars[c].size(); w++)
			{
                if ((Yreal_ci[c][w] > EPSILON) and (Yreal_ci[c][w] < 1.0 - EPSILON))
                {
                    if(Yreal_ci[c][w]<0.5)
                        conj_W.push_back(w);
                    else
                        conj_W.push_front(w);
                }

            }
            ///cout << "Grupo W creado card(W)=" <<conj_W.size()<< endl;
            ///Creación del conjunto T
			list<int> conj_S;

			if (conj_W.size() >= 3)
            { /// generalmente | W | > = 3 es suficiente, pero es preferible más variables fraccionales (menos profundidad en el árbol)
                conj_S.clear();
                conj_S.push_back(conj_W.front());
				conj_W.pop_front();

				for(list<int>::iterator itw=conj_W.begin();itw!=conj_W.end();itw++)
                {
                    double sumax=0;
                    int i,j;
                    for(list<int>::iterator its=conj_S.begin();its!=conj_S.end();its++)
                    {

                        i=*itw;
                        j=*its;
                        sumax+=Xreal_ij[i][j];

                    }
                    if(sumax<=EPSILON)
                    {
                          conj_S.push_front(i);
                    }
                }

            /// cout<<"Conjunto S: "<<conj_S.size()<<endl;
            if(conj_S.size()>1)
            {
                double x_c_S=0;
                list<int>::iterator its,its1;
                for(its=conj_S.begin();its!=conj_S.end();its++)
                {
                     x_c_S=x_c_S+Yreal_ci[c][*its];
                }

                if(x_c_S>1+EPS_CORTES)
                {
                      GRBLinExpr cut_2p=0.0;
                      double Desig_2p=0;
                      ///Y[c:S]
                      for(its=conj_S.begin();its!=conj_S.end();its++)
                      {
                           Desig_2p=Desig_2p+Yreal_ci[c][*its];
                           cut_2p=cut_2p+Yvars[c][*its];
                      }
                        ///-x[E[S]]
                      for(its=conj_S.begin();its!=conj_S.end();its++)
                      {
                           for(its1=its;its1!=conj_S.end();its1++)
                           {
                                if((*its1!=*its)and(Ady_Xvars[*its][*its1]==true))
                                {
                                    Desig_2p=Desig_2p-Xreal_ij[*its][*its1];
                                    if(*its<*its1)
                                        cut_2p=cut_2p-Xvars[*its][*its1];
                                    if(*its>*its1)
                                        cut_2p=cut_2p-Xvars[*its1][*its];
                                }
                            }
                        }

                          if(Desig_2p>1+EPS_CORTES)
                          {
                                addCut(cut_2p<=1);
                                cuts_2p++;
                                desigualdad_t1=true;
                          }

                    }
               }
            }
        }
    }
}

///*************************************************************************
///********CORTES DE CICLOS ************************************************
///*************************************************************************


if(desigualdad_t1==false)
{
double sumawp;
int n_nodos=wp[0].size();
int arcos_ciclo=0;
bool desig_ciclo,desig_sub;


for(unsigned int u1=0;u1<n_nodos-3;u1++)
{
   for(unsigned int u2=u1+1;u2<n_nodos-2;u2++)
   {
      for(unsigned int u3=u2+1;u3<n_nodos-1;u3++)
      {
            if(PLANOS_CICLOS==true)
            {
                int atributos=0;
                for(unsigned int c=0;c<WL_cb[0].size();c++)
                {
                    for(unsigned int t=0;t<WL_cb.size();t++)
                    {
                        if(wp[t][u1]+wp[t][u2]+wp[t][u3]>WU_cb[t][c])
                            atributos++;
                    }
                }

                desig_ciclo=false;
                if(atributos>=WL_cb[0].size()*WL_cb.size())///ES UN CUBRIEMIENTO GLOBAL
                {

                ///SUBGRAFOS 4.5

                   int arcos_subgrafo,grado1,grado2,grado3,gmin;
                   desig_sub=false;

                   sumawp=Xreal_ij[u1][u2]+Xreal_ij[u1][u3]+Xreal_ij[u2][u3];
                   arcos_subgrafo=Ady_Xvars[u1][u2]+Ady_Xvars[u1][u3]+Ady_Xvars[u2][u3];
                   grado1=Ady_Xvars[u1][u2]+Ady_Xvars[u1][u3];
                   grado2=Ady_Xvars[u2][u1]+Ady_Xvars[u2][u3];
                   grado3=Ady_Xvars[u3][u1]+Ady_Xvars[u3][u2];
                   gmin=min(grado1,grado2);
                   gmin=min(gmin,grado3);


                   if(sumawp>arcos_subgrafo-gmin+EPS_CORTES)
                   {

                        GRBLinExpr cut_sub=0;
                        if(Ady_Xvars[u1][u2]==true)
                            cut_sub=cut_sub+Xvars[u1][u2];

                        if(Ady_Xvars[u1][u3]==true)
                            cut_sub=cut_sub+Xvars[u1][u3];

                        if(Ady_Xvars[u2][u3]==true)
                            cut_sub=cut_sub+Xvars[u2][u3];

                        addCut(cut_sub<=arcos_subgrafo-gmin);
                        cuts_subg++;
                        desig_sub=true;
                   }



                ///CICLOS 4.2
                    if(desig_sub==false)
                    {
                        GRBLinExpr cut_ciclo=0;

                        sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u3]+Xreal_ij[u1][u3];
                        arcos_ciclo=Ady_Xvars[u1][u2]+Ady_Xvars[u2][u3]+Ady_Xvars[u1][u3];
                        if((sumawp>1+EPSILON)and(arcos_ciclo==3))
                        {
                            cut_ciclo=0;

                            if(Ady_Xvars[u1][u2]==true)
                                cut_ciclo=cut_ciclo+Xvars[u1][u2];


                            if(Ady_Xvars[u2][u3]==true)
                                cut_ciclo=cut_ciclo+Xvars[u2][u3];


                            if(Ady_Xvars[u1][u3]==true)
                                cut_ciclo=cut_ciclo+Xvars[u1][u3];

                            addCut(cut_ciclo<=1);
                            cuts_ciclos++;
                            desig_ciclo=true;
                        }
                     }
                  }
               }
                    ///************* TIPO KNAPSACK *************************
                    ///************** TEOREMA 4.9 **************************
                if(PLANOS_MOCHILA==true)
                {
                    if(desig_ciclo==false)
                    {

                            vector<double> r(WL_cb.size());
                            int cond_S=0;

                            for(unsigned int t=0;t<WL_cb.size();t++)
                            {
                                double wS=wp[t][u1]+wp[t][u2]+wp[t][u3];
                                double WU_min=+INF;
                                for(unsigned int c=0;c<WL_cb[0].size();c++)
                                {
                                    WU_min=min(WU_min,WU_cb[t][c]);
                                }
                                if(wS<WU_min)
                                {
                                    cond_S++;///calcula el número de veces que se verifica la condicion
                                    r[t]=WU_min-wS;
                                }
                            }

                            if(cond_S==WL_cb.size())///si es igual, entonces  S es valido
                            {

                                ///calculando el conjunto X
                                list<int> conj_X;
                                conj_X.clear();

                                for(unsigned int jx=0;jx<n_nodos;jx++)
                                {
                                    if((jx!=u1)and(jx!=u2)and(jx!=u3))
                                    {
                                        int n_tx=0;
                                        for(unsigned int tx=0;tx<WL_cb.size();tx++)
                                        {
                                            if(wp[tx][jx]>r[tx])
                                            {
                                                n_tx++;
                                            }
                                        }

                                        if(n_tx==WL_cb.size())
                                        {
                                            conj_X.push_back(jx);
                                        }
                                    }
                                }



                                if(conj_X.size()>0)
                                {

                                    for(unsigned int tx=0;tx<WL_cb.size();tx++)
                                    {
                                        double valor_knp_u1=0;
                                        valor_knp_u1=wp[tx][u2]*Xreal_ij[u1][u2]+wp[tx][u3]*Xreal_ij[u1][u3];

                                        for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                        {
                                            valor_knp_u1=valor_knp_u1+(wp[tx][*itx]-r[tx])*Xreal_ij[u1][*itx];
                                        }

                                        double wS_t=wp[tx][u2]+wp[tx][u3];


                                        if(valor_knp_u1>wS_t+EPS_CORTES)
                                        {

                                            GRBLinExpr cut_knp_u1=0;
                                            if(Ady_Xvars[u1][u2]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u2]*Xvars[u1][u2];
                                            }
                                            if(Ady_Xvars[u1][u3]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u3]*Xvars[u1][u3];
                                            }

                                            for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                            {
                                                if(Ady_Xvars[u1][*itx]==true)
                                                {
                                                    if(u1<*itx)
                                                        cut_knp_u1=cut_knp_u1+(wp[tx][*itx]-r[tx])*Xvars[u1][*itx];
                                                    else
                                                        cut_knp_u1=cut_knp_u1+(wp[tx][*itx]-r[tx])*Xvars[*itx][u1];
                                                }
                                            }

                                            if(cut_knp_u1.size()>0)
                                            {
                                                addCut(cut_knp_u1<=wS_t);
                                                cuts_mochila++;
                                            }
                                        }
                                    }
                                }
                            }
                       }
                }

                for(unsigned int u4=u3+1;u4<n_nodos;u4++)
                {

                    if(PLANOS_CICLOS==true)
                    {
                            int atributos=0;
                            for(unsigned int c=0;c<WL_cb[0].size();c++)
                            {
                                for(unsigned int t=0;t<WL_cb.size();t++)
                                {
                                    if(wp[t][u1]+wp[t][u2]+wp[t][u3]+wp[t][u4]>WU_cb[t][c])
                                        atributos++;
                                }
                            }

                            bool desig_sub=false;
                            desig_ciclo=false;

                            if(atributos>=WL_cb[0].size()*WL_cb.size())///ES UN CUBRIEMIENTO GLOBAL
                            {
                                ///SUBGRAFOS 4.5

                                int arcos_subgrafo,grado1,grado2,grado3,grado4,gmin;
                                bool desig_sub=false;

                                sumawp=Xreal_ij[u1][u2]+Xreal_ij[u1][u3]+Xreal_ij[u1][u4]+Xreal_ij[u2][u3]+Xreal_ij[u2][u4]+Xreal_ij[u3][u4];
                                arcos_subgrafo=Ady_Xvars[u1][u2]+Ady_Xvars[u1][u3]+Ady_Xvars[u1][u4]+Ady_Xvars[u2][u3]+Ady_Xvars[u2][u4]+Ady_Xvars[u3][u4];
                                grado1=Ady_Xvars[u1][u2]+Ady_Xvars[u1][u3]+Ady_Xvars[u1][u4];
                                grado2=Ady_Xvars[u2][u1]+Ady_Xvars[u2][u3]+Ady_Xvars[u2][u4];
                                grado3=Ady_Xvars[u3][u1]+Ady_Xvars[u3][u2]+Ady_Xvars[u3][u4];
                                grado4=Ady_Xvars[u4][u1]+Ady_Xvars[u4][u2]+Ady_Xvars[u4][u3];

                                gmin=min(grado1,grado2);
                                gmin=min(gmin,grado3);
                                gmin=min(gmin,grado4);

                                if(sumawp>arcos_subgrafo-gmin+EPS_CORTES)
                                {

                                    GRBLinExpr cut_sub=0;
                                    if(Ady_Xvars[u1][u2]==true)
                                        cut_sub=cut_sub+Xvars[u1][u2];

                                    if(Ady_Xvars[u1][u3]==true)
                                        cut_sub=cut_sub+Xvars[u1][u3];

                                    if(Ady_Xvars[u1][u4]==true)
                                        cut_sub=cut_sub+Xvars[u1][u4];

                                    if(Ady_Xvars[u2][u3]==true)
                                        cut_sub=cut_sub+Xvars[u2][u3];

                                    if(Ady_Xvars[u2][u4]==true)
                                        cut_sub=cut_sub+Xvars[u2][u4];

                                     if(Ady_Xvars[u3][u4]==true)
                                        cut_sub=cut_sub+Xvars[u3][u4];

                                    addCut(cut_sub<=arcos_subgrafo-gmin);
                                    cuts_subg++;
                                    desig_sub=true;
                                }

                               ///CICLOS 4.2
                                if(desig_sub==false)
                                {
                                    GRBLinExpr cut_ciclo=0;

                                    sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u3]+Xreal_ij[u3][u4]+Xreal_ij[u1][u4];
                                    if(sumawp>2+EPSILON)
                                    {
                                        cut_ciclo=0;

                                        if(Ady_Xvars[u1][u2]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u1][u2];

                                        if(Ady_Xvars[u2][u3]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u2][u3];

                                        if(Ady_Xvars[u3][u4]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u3][u4];

                                        if(Ady_Xvars[u1][u4]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u1][u4];


                                        addCut(cut_ciclo<=2);
                                        cuts_ciclos++;
                                        desig_ciclo=true;
                                    }

                                    sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u4]+Xreal_ij[u3][u4]+Xreal_ij[u1][u3];
                                    if(sumawp>2+EPSILON)
                                    {
                                        cut_ciclo=0;

                                        if(Ady_Xvars[u1][u2]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u1][u2];

                                        if(Ady_Xvars[u2][u4]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u2][u4];

                                        if(Ady_Xvars[u3][u4]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u3][u4];

                                        if(Ady_Xvars[u1][u3]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u1][u3];

                                        addCut(cut_ciclo<=2);
                                        cuts_ciclos++;
                                        desig_ciclo=true;
                                    }

                                    sumawp=Xreal_ij[u1][u3]+Xreal_ij[u2][u3]+Xreal_ij[u2][u4]+Xreal_ij[u1][u4];
                                    if(sumawp>2+EPSILON)
                                    {
                                        cut_ciclo=0;

                                        if(Ady_Xvars[u1][u3]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u1][u3];

                                        if(Ady_Xvars[u2][u3]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u2][u3];

                                        if(Ady_Xvars[u2][u4]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u2][u4];

                                        if(Ady_Xvars[u1][u4]==true)
                                            cut_ciclo=cut_ciclo+Xvars[u1][u4];

                                        addCut(cut_ciclo<=2);
                                        cuts_ciclos++;
                                        desig_ciclo=true;
                                    }

                               }
                         }
                    }
                    ///*************TIPO KNAPSACK *************************
                    ///************TEOREMA 4.9 **********************

                    if(PLANOS_MOCHILA==true)
                    {
                        if(desig_ciclo==false)
                        {

                                vector<double> r(WL_cb.size());
                                int cond_S=0;

                                for(unsigned int t=0;t<WL_cb.size();t++)
                                {
                                    double  wS=wp[t][u1]+wp[t][u2]+wp[t][u3]+wp[t][u4];
                                    double WU_min=+INF;
                                    for(unsigned int c=0;c<WL_cb[0].size();c++)
                                    {
                                        WU_min=min(WU_min,WU_cb[t][c]);
                                    }
                                    if(wS<WU_min)
                                    {
                                        cond_S++;///calcula el número de veces que se verifica la condicion
                                        r[t]=WU_min-wS;
                                    }
                                }

                                if(cond_S==WL_cb.size())///si es igual, entonces  S es valido
                                {
                                    ///calculando el conjunto X
                                    list<int> conj_X;
                                    conj_X.clear();

                                    for(unsigned int jx=0;jx<n_nodos;jx++)
                                    {
                                        if((jx!=u1)and(jx!=u2)and(jx!=u3)and(jx!=u4))
                                        {
                                            int n_tx=0;
                                            for(unsigned int tx=0;tx<WL_cb.size();tx++)
                                            {
                                                if(wp[tx][jx]>r[tx])
                                                {
                                                    n_tx++;
                                                }
                                            }

                                            if(n_tx==WL_cb.size())
                                            {
                                                conj_X.push_back(jx);
                                            }
                                        }
                                    }

                                    if(conj_X.size()>0)
                                    {
                                        for(unsigned int tx=0;tx<WL_cb.size();tx++)
                                        {
                                            double valor_knp_u1=0,valor_knp_u2=0;
                                            valor_knp_u1=wp[tx][u2]*Xreal_ij[u1][u2]+wp[tx][u3]*Xreal_ij[u1][u3]+wp[tx][u4]*Xreal_ij[u1][u4];
                                            valor_knp_u2=wp[tx][u1]*Xreal_ij[u1][u2]+wp[tx][u3]*Xreal_ij[u2][u3]+wp[tx][u4]*Xreal_ij[u2][u4];

                                            for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                            {
                                                valor_knp_u1=valor_knp_u1+(wp[tx][*itx]-r[tx])*Xreal_ij[u1][*itx];
                                                valor_knp_u2=valor_knp_u2+(wp[tx][*itx]-r[tx])*Xreal_ij[u2][*itx];
                                            }

                                            double  wS_t1,wS_t2;
                                            wS_t1=wp[tx][u2]+wp[tx][u3]+wp[tx][u4];


                                            if(valor_knp_u1>wS_t1+EPS_CORTES)
                                            {

                                                GRBLinExpr cut_knp_u1=0;
                                                if(Ady_Xvars[u1][u2]==true)
                                                {
                                                    cut_knp_u1=cut_knp_u1+wp[tx][u2]*Xvars[u1][u2];
                                                }
                                                if(Ady_Xvars[u1][u3]==true)
                                                {
                                                    cut_knp_u1=cut_knp_u1+wp[tx][u3]*Xvars[u1][u3];
                                                }
                                                if(Ady_Xvars[u1][u4]==true)
                                                {
                                                    cut_knp_u1=cut_knp_u1+wp[tx][u4]*Xvars[u1][u4];
                                                }

                                                for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                                {
                                                    if(Ady_Xvars[u1][*itx]==true)
                                                    {
                                                        if(u1<*itx)
                                                            cut_knp_u1=cut_knp_u1+(wp[tx][*itx]-r[tx])*Xvars[u1][*itx];
                                                        else
                                                            cut_knp_u1=cut_knp_u1+(wp[tx][*itx]-r[tx])*Xvars[*itx][u1];
                                                    }
                                                }

                                                if(cut_knp_u1.size()>0)
                                                {
                                                    addCut(cut_knp_u1<=wS_t1);
                                                    cuts_mochila++;
                                                }
                                            }


                                            wS_t2=wp[tx][u1]+wp[tx][u3]+wp[tx][u4];
                                            if(valor_knp_u2>wS_t2+EPS_CORTES)
                                            {

                                                GRBLinExpr cut_knp_u2=0;
                                                if(Ady_Xvars[u2][u1]==true)
                                                {
                                                    cut_knp_u2=cut_knp_u2+wp[tx][u1]*Xvars[u1][u2];
                                                }
                                                if(Ady_Xvars[u2][u3]==true)
                                                {
                                                    cut_knp_u2=cut_knp_u2+wp[tx][u3]*Xvars[u2][u3];
                                                }
                                                if(Ady_Xvars[u2][u4]==true)
                                                {
                                                    cut_knp_u2=cut_knp_u2+wp[tx][u4]*Xvars[u2][u4];
                                                }

                                                for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                                {
                                                    if(Ady_Xvars[u2][*itx]==true)
                                                    {
                                                        if(u2<*itx)
                                                            cut_knp_u2=cut_knp_u2+(wp[tx][*itx]-r[tx])*Xvars[u2][*itx];
                                                        else
                                                            cut_knp_u2=cut_knp_u2+(wp[tx][*itx]-r[tx])*Xvars[*itx][u2];
                                                    }
                                                }

                                                if(cut_knp_u2.size()>0)
                                                {
                                                    addCut(cut_knp_u2<=wS_t2);
                                                    cuts_mochila++;
                                                }
                                            }
                                       }
                                   }
                              }
                        }
                }

              }///cierra u4
           }///cierra u3
         }///cierra u2
       }///u1
     }
   }
  }///fin GRB_OPTIMAL
}



void planos_proyeccion::callback()
{
		//Terminación implementado para detener la optimización del modelo cuando se satisface la condición de que
		//nodecnt % 100 == 1
		if (where == GRB_CB_MIP)
		{
			//Callback general para branch-and-bound
			///callback codes
			// IN MIP (actualización del número de nodos) - Cuando se navega en el árbol
			nodecnt = (int)getDoubleInfo(GRB_CB_MIP_NODCNT); //obtener la información de la cuenta explorada actual del nodo.
			if (nodecnt % 100 == 1) // que nos dice esta restricción¿?{
			{
				//cout << "Lazy constraints added: " << lazys_added << endl;
				cout <<AZUL<<" |2P="<<cuts_2p<<" |CICLOS="<< cuts_ciclos<<" |SUB="<<cuts_subg<<" |KNP="<<cuts_mochila
				<<  " | NODOS="<<nodecnt<<endl<<RESET;
            }
        }

    else if (where == GRB_CB_MIPNODE)
    {

            if (getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL)
            {


                for (unsigned int  i= 0; i < Xreal_ij.size(); i++)
                {
					for (unsigned int  j= i+1; j < Xreal_ij[i].size(); j++)
					{
                        if(Ady_Xvars[i][j]==true)
                            Xreal_ij[i][j] =Xreal_ij[j][i]= getNodeRel(Xvars[i][j]);
                        else
                            Xreal_ij[i][j] =0;
                        ///cout<<"X i="<<i<<"  j="<<j<<"="<<Xreal_ij[i][j]<<endl;
                    }
                }

                bool desigualdad_t1=false;

                int Nnodos=wp[0].size();


///***************************************************************************************
///*******************CORTES TEOREMA 4.2 *************************************************
///***************************************************************************************
if(PLANOS_2P==true)
{
        if(desigualdad_t1==false)
        {
        double valor=0;


        for (int c = Nnodos; c < Xvars.size(); c++)///conjunto R
        {
		/// Creación del conjunto W
        list<int> conj_W;
            for (int w =0; w < Nnodos; w++)
			{
                if ((Xreal_ij[w][c] > EPSILON) and (Xreal_ij[w][c] < 1.0 - EPSILON))
                {
                    if(Xreal_ij[w][c]<0.5)
                        conj_W.push_back(w);
                    else
                        conj_W.push_front(w);
                }

            }
            ///cout << "Grupo W creado card(W)=" <<conj_W.size()<< endl;
            ///Creación del conjunto T
			list<int> conj_S;

			if (conj_W.size() >= 3)
            { /// generalmente | W | > = 3 es suficiente, pero es preferible más variables fraccionales (menos profundidad en el árbol)
                conj_S.clear();
                conj_S.push_back(conj_W.front());
				conj_W.pop_front();

				for(list<int>::iterator itw=conj_W.begin();itw!=conj_W.end();itw++)
                {
                    double sumax=0;
                    int i,j;
                    for(list<int>::iterator its=conj_S.begin();its!=conj_S.end();its++)
                    {

                        i=*itw;
                        j=*its;
                        sumax+=Xreal_ij[i][j];

                    }
                    if(sumax<=EPSILON)
                    {
                          conj_S.push_front(i);
                    }
                }

            /// cout<<"Conjunto S: "<<conj_S.size()<<endl;
            if(conj_S.size()>1)
            {
                double x_c_S=0;
                list<int>::iterator its,its1;
                for(its=conj_S.begin();its!=conj_S.end();its++)
                {
                     x_c_S=x_c_S+Xreal_ij[c][*its];
                }

                if(x_c_S>1+EPS_CORTES)
                {
                      GRBLinExpr cut_2p=0.0;
                      double Desig_2p=0;
                      ///Y[c:S]
                      for(its=conj_S.begin();its!=conj_S.end();its++)
                      {
                           Desig_2p=Desig_2p+Xreal_ij[*its][c];

                           if(Ady_Xvars[*its][c]==true)
                                cut_2p=cut_2p+Xvars[*its][c];
                      }
                        ///-x[E[S]]
                      for(its=conj_S.begin();its!=conj_S.end();its++)
                      {
                           for(its1=its;its1!=conj_S.end();its1++)
                                if(*its1!=*its)
                                {
                                    Desig_2p=Desig_2p-Xreal_ij[*its][*its1];

                                        if(*its<*its1)
                                        {
                                            if(Ady_Xvars[*its][*its1]==true)
                                                cut_2p=cut_2p-Xvars[*its][*its1];
                                        }

                                        if(*its>*its1)
                                        {
                                            if(Ady_Xvars[*its1][*its]==true)
                                                cut_2p=cut_2p-Xvars[*its1][*its];
                                        }
                                }
                        }

                          if(Desig_2p>1+EPS_CORTES)
                          {
                                addCut(cut_2p<=1);
                                cuts_2p++;
                                desigualdad_t1=true;
                          }

                    }
               }
            }
        }

    }
}

///*************************************************************************
///********CORTES DE CICLOS ************************************************
///*************************************************************************



    if(desigualdad_t1==false)
    {

    double sumawp;
    int n_nodos=wp[0].size();
    int arcos_ciclo=0;
    bool desig_ciclo,desig_sub;

    for(unsigned int u1=0;u1<n_nodos-3;u1++)
    {
        for(unsigned int u2=u1+1;u2<n_nodos-2;u2++)
        {
            for(unsigned int u3=u2+1;u3<n_nodos-1;u3++)
            {

            if(PLANOS_CICLOS==true)
            {
                int atributos=0;
                for(unsigned int c=0;c<WL_cb[0].size();c++)
                {
                    for(unsigned int t=0;t<WL_cb.size();t++)
                    {
                        if(wp[t][u1]+wp[t][u2]+wp[t][u3]>WU_cb[t][c])
                            atributos++;
                    }
                }

                desig_ciclo=false;
                if(atributos>=WL_cb[0].size()*WL_cb.size())///ES UN CUBRIEMIENTO GLOBAL
                {

                ///SUBGRAFOS 4.5

                   int arcos_subgrafo,grado1,grado2,grado3,gmin;
                   desig_sub=false;

                   sumawp=Xreal_ij[u1][u2]+Xreal_ij[u1][u3]+Xreal_ij[u2][u3];
                   arcos_subgrafo=Ady_Xvars[u1][u2]+Ady_Xvars[u1][u3]+Ady_Xvars[u2][u3];
                   grado1=Ady_Xvars[u1][u2]+Ady_Xvars[u1][u3];
                   grado2=Ady_Xvars[u2][u1]+Ady_Xvars[u2][u3];
                   grado3=Ady_Xvars[u3][u1]+Ady_Xvars[u3][u2];
                   gmin=min(grado1,grado2);
                   gmin=min(gmin,grado3);


                   if(sumawp>arcos_subgrafo-gmin+EPS_CORTES)
                   {

                        GRBLinExpr cut_sub=0;
                        if(Ady_Xvars[u1][u2]==true)
                            cut_sub=cut_sub+Xvars[u1][u2];

                        if(Ady_Xvars[u1][u3]==true)
                            cut_sub=cut_sub+Xvars[u1][u3];

                        if(Ady_Xvars[u2][u3]==true)
                            cut_sub=cut_sub+Xvars[u2][u3];

                        addCut(cut_sub<=arcos_subgrafo-gmin);
                        cuts_subg++;
                        desig_sub=true;
                   }



                ///CICLOS 4.2
                    if(desig_sub==false)
                    {
                        GRBLinExpr cut_ciclo=0;

                        sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u3]+Xreal_ij[u1][u3];
                        arcos_ciclo=Ady_Xvars[u1][u2]+Ady_Xvars[u2][u3]+Ady_Xvars[u1][u3];
                        if((sumawp>1+EPSILON)and(arcos_ciclo==3))
                        {
                            cut_ciclo=0;

                            if(Ady_Xvars[u1][u2]==true)
                                cut_ciclo=cut_ciclo+Xvars[u1][u2];


                            if(Ady_Xvars[u2][u3]==true)
                                cut_ciclo=cut_ciclo+Xvars[u2][u3];


                            if(Ady_Xvars[u1][u3]==true)
                                cut_ciclo=cut_ciclo+Xvars[u1][u3];

                            addCut(cut_ciclo<=1);
                            cuts_ciclos++;
                            desig_ciclo=true;
                        }
                      }
                   }
                }

                    ///*************TIPO KNAPSACK *************************
                    ///************TEOREMA 4.9 **********************
                if(PLANOS_MOCHILA==true)
                {
                    if(desig_ciclo==false)
                    {

                            vector<double> r(WL_cb.size());
                            int cond_S=0;

                            for(unsigned int t=0;t<WL_cb.size();t++)
                            {
                                double wS=wp[t][u1]+wp[t][u2]+wp[t][u3];
                                double WU_min=+INF;
                                for(unsigned int c=0;c<WL_cb[0].size();c++)
                                {
                                    WU_min=min(WU_min,WU_cb[t][c]);
                                }
                                if(wS<WU_min)
                                {
                                    cond_S++;///calcula el número de veces que se verifica la condicion
                                    r[t]=WU_min-wS;
                                }
                            }

                            if(cond_S==WL_cb.size())///si es igual, entonces  S es valido
                            {

                                ///calculando el conjunto X
                                list<int> conj_X;
                                conj_X.clear();

                                for(unsigned int jx=0;jx<n_nodos;jx++)
                                {
                                    if((jx!=u1)and(jx!=u2)and(jx!=u3))
                                    {
                                        int n_tx=0;
                                        for(unsigned int tx=0;tx<WL_cb.size();tx++)
                                        {
                                            if(wp[tx][jx]>r[tx])
                                            {
                                                n_tx++;
                                            }
                                        }

                                        if(n_tx==WL_cb.size())
                                        {
                                            conj_X.push_back(jx);
                                        }
                                    }
                                }



                                if(conj_X.size()>0)
                                {

                                    for(unsigned int tx=0;tx<WL_cb.size();tx++)
                                    {
                                        double valor_knp_u1=0;
                                        valor_knp_u1=wp[tx][u2]*Xreal_ij[u1][u2]+wp[tx][u3]*Xreal_ij[u1][u3];

                                        for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                        {
                                            valor_knp_u1=valor_knp_u1+(wp[tx][*itx]-r[tx])*Xreal_ij[u1][*itx];
                                        }

                                        double wS_t=wp[tx][u2]+wp[tx][u3];


                                        if(valor_knp_u1>wS_t+EPS_CORTES)
                                        {

                                            GRBLinExpr cut_knp_u1=0;
                                            if(Ady_Xvars[u1][u2]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u2]*Xvars[u1][u2];
                                            }
                                            if(Ady_Xvars[u1][u3]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u3]*Xvars[u1][u3];
                                            }

                                            for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                            {
                                                if(Ady_Xvars[u1][*itx]==true)
                                                {
                                                    if(u1<*itx)
                                                        cut_knp_u1=cut_knp_u1+(wp[tx][*itx]-r[tx])*Xvars[u1][*itx];
                                                    else
                                                        cut_knp_u1=cut_knp_u1+(wp[tx][*itx]-r[tx])*Xvars[*itx][u1];
                                                }
                                            }

                                            if(cut_knp_u1.size()>0)
                                            {
                                                addCut(cut_knp_u1<=wS_t);
                                                cuts_mochila++;
                                            }
                                        }
                                    }
                                }
                            }
                       }
                 }





            for(unsigned int u4=u3+1;u4<n_nodos;u4++)
            {
              if(PLANOS_CICLOS==true)
              {
                    int atributos=0;
                    for(unsigned int c=0;c<WL_cb[0].size();c++)
                    {
                        for(unsigned int t=0;t<WL_cb.size();t++)
                        {
                            if(wp[t][u1]+wp[t][u2]+wp[t][u3]+wp[t][u4]>WU_cb[t][c])
                                atributos++;
                        }
                    }


                    bool desig_sub, desig_ciclo;
                    if(atributos>=WL_cb[0].size()*WL_cb.size())///ES UN CUBRIEMIENTO GLOBAL
                    {

                            ///SUBGRAFOS 4.5

                            int arcos_subgrafo,grado1,grado2,grado3,grado4,gmin;
                            desig_sub=false;
                            desig_ciclo=false;

                            sumawp=Xreal_ij[u1][u2]+Xreal_ij[u1][u3]+Xreal_ij[u1][u4]+Xreal_ij[u2][u3]+Xreal_ij[u2][u4]+Xreal_ij[u3][u4];
                            arcos_subgrafo=Ady_Xvars[u1][u2]+Ady_Xvars[u1][u3]+Ady_Xvars[u1][u4]+Ady_Xvars[u2][u3]+Ady_Xvars[u2][u4]+Ady_Xvars[u3][u4];
                            grado1=Ady_Xvars[u1][u2]+Ady_Xvars[u1][u3]+Ady_Xvars[u1][u4];
                            grado2=Ady_Xvars[u2][u1]+Ady_Xvars[u2][u3]+Ady_Xvars[u2][u4];
                            grado3=Ady_Xvars[u3][u1]+Ady_Xvars[u3][u2]+Ady_Xvars[u3][u4];
                            grado4=Ady_Xvars[u4][u1]+Ady_Xvars[u4][u2]+Ady_Xvars[u4][u3];

                            gmin=min(grado1,grado2);
                            gmin=min(gmin,grado3);
                            gmin=min(gmin,grado4);


                            if(sumawp>arcos_subgrafo-gmin+EPS_CORTES)
                            {
                                GRBLinExpr cut_sub=0;

                                if(Ady_Xvars[u1][u2]==true)
                                    cut_sub=cut_sub+Xvars[u1][u2];

                                if(Ady_Xvars[u1][u3]==true)
                                    cut_sub=cut_sub+Xvars[u1][u3];

                                if(Ady_Xvars[u1][u4]==true)
                                    cut_sub=cut_sub+Xvars[u1][u4];

                                if(Ady_Xvars[u2][u3]==true)
                                    cut_sub=cut_sub+Xvars[u2][u3];

                                if(Ady_Xvars[u2][u4]==true)
                                    cut_sub=cut_sub+Xvars[u2][u4];

                                if(Ady_Xvars[u3][u4]==true)
                                    cut_sub=cut_sub+Xvars[u3][u4];

                                addCut(cut_sub<=arcos_subgrafo-gmin);
                                cuts_subg++;
                                desig_sub=true;
                            }

                        if(desig_sub==false)
                        {
                            GRBLinExpr cut_ciclo=0;

                            sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u3]+Xreal_ij[u3][u4]+Xreal_ij[u1][u4];
                            if(sumawp>2+EPSILON)
                            {
                                cut_ciclo=0;

                                if(Ady_Xvars[u1][u2]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u2];

                                if(Ady_Xvars[u2][u3]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u2][u3];

                                if(Ady_Xvars[u3][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u3][u4];

                                if(Ady_Xvars[u1][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u4];

                                addCut(cut_ciclo<=2);
                                cuts_ciclos++;
                                desig_ciclo=true;
                            }

                            sumawp=Xreal_ij[u1][u3]+Xreal_ij[u2][u3]+Xreal_ij[u2][u4]+Xreal_ij[u1][u4];
                            if(sumawp>2+EPSILON)
                            {

                                cut_ciclo=0;

                                if(Ady_Xvars[u1][u3]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u3];

                                if(Ady_Xvars[u2][u3]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u2][u3];

                                if(Ady_Xvars[u2][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u2][u4];

                                if(Ady_Xvars[u1][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u4];


                                addCut(cut_ciclo<=2);
                                cuts_ciclos++;
                                desig_ciclo=true;
                            }

                            sumawp=Xreal_ij[u1][u3]+Xreal_ij[u1][u2]+Xreal_ij[u2][u4]+Xreal_ij[u3][u4];
                            if(sumawp>2+EPSILON)
                            {
                                cut_ciclo=0;

                                if(Ady_Xvars[u1][u3]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u3];

                                if(Ady_Xvars[u1][u2]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u2];

                                if(Ady_Xvars[u2][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u2][u4];

                                if(Ady_Xvars[u3][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u3][u4];

                                addCut(cut_ciclo<=2);
                                cuts_ciclos++;
                                desig_ciclo=true;
                            }
                        }
                     }
                  }


                    ///************* TIPO KNAPSACK *************************
                    ///************** TEOREMA 4.9 **************************

                 if(PLANOS_MOCHILA==true)
                 {
                    if(desig_ciclo==false)
                    {

                            vector<double> r(WL_cb.size());
                            int cond_S=0;


                            for(unsigned int t=0;t<WL_cb.size();t++)
                            {
                                double wS=wp[t][u1]+wp[t][u2]+wp[t][u3]+wp[t][u4];
                                double WU_min=+INF;
                                for(unsigned int c=0;c<WL_cb[0].size();c++)
                                {
                                    WU_min=min(WU_min,WU_cb[t][c]);
                                }
                                if(wS<WU_min)
                                {
                                    cond_S++;///calcula el número de veces que se verifica la condicion
                                    r[t]=WU_min-wS;
                                }

                            }

                            if(cond_S==WL_cb.size())///si es igual, entonces  S es valido
                            {

                                ///calculando el conjunto X
                                list<int> conj_X;
                                conj_X.clear();

                                for(unsigned int jx=0;jx<n_nodos;jx++)
                                {
                                    if((jx!=u1)and(jx!=u2)and(jx!=u3)and(jx!=u4))
                                    {
                                        int n_tx=0;
                                        for(unsigned int tx=0;tx<WL_cb.size();tx++)
                                        {
                                            if(wp[tx][jx]>r[tx])
                                            {
                                                n_tx++;
                                            }
                                        }

                                        if(n_tx==WL_cb.size())
                                        {
                                            conj_X.push_back(jx);
                                        }
                                    }
                                }



                                if(conj_X.size()>0)
                                {

                                    for(unsigned int tx=0;tx<WL_cb.size();tx++)
                                    {
                                        double valor_knp_u1=0,valor_knp_u2=0;
                                        valor_knp_u1=wp[tx][u2]*Xreal_ij[u1][u2]+wp[tx][u3]*Xreal_ij[u1][u3]+wp[tx][u4]*Xreal_ij[u1][u4];
                                        valor_knp_u2=wp[tx][u1]*Xreal_ij[u1][u2]+wp[tx][u3]*Xreal_ij[u2][u3]+wp[tx][u4]*Xreal_ij[u2][u4];


                                        for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                        {
                                            valor_knp_u1=valor_knp_u1+(wp[tx][*itx]-r[tx])*Xreal_ij[u1][*itx];
                                            valor_knp_u2=valor_knp_u2+(wp[tx][*itx]-r[tx])*Xreal_ij[u2][*itx];
                                        }

                                        double wS_t1=wp[tx][u2]+wp[tx][u3]+wp[tx][u4];


                                        if(valor_knp_u1>wS_t1+EPS_CORTES)
                                        {

                                            GRBLinExpr cut_knp_u1=0;
                                            if(Ady_Xvars[u1][u2]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u2]*Xvars[u1][u2];
                                            }
                                            if(Ady_Xvars[u1][u3]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u3]*Xvars[u1][u3];
                                            }
                                            if(Ady_Xvars[u1][u4]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u2]*Xvars[u1][u4];
                                            }
                                            for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                            {
                                                if(Ady_Xvars[u1][*itx]==true)
                                                {
                                                    if(u1<*itx)
                                                        cut_knp_u1=cut_knp_u1+(wp[tx][*itx]-r[tx])*Xvars[u1][*itx];
                                                    else
                                                        cut_knp_u1=cut_knp_u1+(wp[tx][*itx]-r[tx])*Xvars[*itx][u1];
                                                }
                                            }

                                            if(cut_knp_u1.size()>0)
                                            {
                                                addCut(cut_knp_u1<=wS_t1);
                                                cuts_mochila++;
                                            }
                                        }

                                        double wS_t2=wp[tx][u1]+wp[tx][u3]+wp[tx][u4];
                                        if(valor_knp_u2>wS_t2+EPS_CORTES)
                                        {

                                            GRBLinExpr cut_knp_u2=0;
                                            if(Ady_Xvars[u2][u1]==true)
                                            {
                                                cut_knp_u2=cut_knp_u2+wp[tx][u1]*Xvars[u1][u2];
                                            }
                                            if(Ady_Xvars[u2][u3]==true)
                                            {
                                                cut_knp_u2=cut_knp_u2+wp[tx][u3]*Xvars[u2][u3];
                                            }
                                            if(Ady_Xvars[u2][u4]==true)
                                            {
                                                cut_knp_u2=cut_knp_u2+wp[tx][u4]*Xvars[u2][u4];
                                            }

                                            for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                            {
                                                if(Ady_Xvars[u2][*itx]==true)
                                                {
                                                    if(u2<*itx)
                                                        cut_knp_u2=cut_knp_u2+(wp[tx][*itx]-r[tx])*Xvars[u2][*itx];
                                                    else
                                                        cut_knp_u2=cut_knp_u2+(wp[tx][*itx]-r[tx])*Xvars[*itx][u2];
                                                }
                                            }

                                            if(cut_knp_u2.size()>0)
                                            {
                                                addCut(cut_knp_u2<=wS_t2);
                                                cuts_mochila++;
                                            }
                                        }

                                    }
                                }
                            }
                      }
                }

/*

                for(unsigned int u5=u4+1;u5<n_nodos;u5++)
                {
                    int atributos=0;
                    for(unsigned int c=0;c<WL_cb[0].size();c++)
                    {
                        for(unsigned int t=0;t<WL_cb.size();t++)
                        {
                            if(wp[t][u1]+wp[t][u2]+wp[t][u3]+wp[t][u4]+wp[t][u5]>WU_cb[t][c])
                                atributos++;
                        }
                    }


                    if(atributos>=WL_cb[0].size()*WL_cb.size())///ES UN CUBRIEMIENTO GLOBAL
                    {

                        ///SUBGRAFOS 4.5

                        int arcos_subgrafo,grado1,grado2,grado3,grado4,grado5,gmin;
                        bool desig_sub=false;

                        sumawp=Xreal_ij[u1][u2]+Xreal_ij[u1][u3]+Xreal_ij[u1][u4]+Xreal_ij[u1][u5]+Xreal_ij[u2][u3]+Xreal_ij[u2][u4]+Xreal_ij[u2][u5]+Xreal_ij[u3][u4]+Xreal_ij[u3][u5]+Xreal_ij[u4][u5];
                        arcos_subgrafo=Ady_Xvars[u1][u2]+Ady_Xvars[u1][u3]+Ady_Xvars[u1][u4]+Ady_Xvars[u1][u5]+Ady_Xvars[u2][u3]+Ady_Xvars[u2][u4]+Ady_Xvars[u2][u5]+Ady_Xvars[u3][u4]+Ady_Xvars[u3][u5]+Ady_Xvars[u4][u5];
                        grado1=Ady_Xvars[u1][u2]+Ady_Xvars[u1][u3]+Ady_Xvars[u1][u4]+Ady_Xvars[u1][u5];
                        grado2=Ady_Xvars[u2][u1]+Ady_Xvars[u2][u3]+Ady_Xvars[u2][u4]+Ady_Xvars[u2][u5];
                        grado3=Ady_Xvars[u3][u1]+Ady_Xvars[u3][u2]+Ady_Xvars[u3][u4]+Ady_Xvars[u3][u5];
                        grado4=Ady_Xvars[u4][u1]+Ady_Xvars[u4][u2]+Ady_Xvars[u4][u3]+Ady_Xvars[u4][u5];
                        grado5=Ady_Xvars[u5][u1]+Ady_Xvars[u5][u2]+Ady_Xvars[u5][u3]+Ady_Xvars[u5][u4];

                        gmin=min(grado1,grado2);
                        gmin=min(gmin,grado3);
                        gmin=min(gmin,grado4);
                        gmin=min(gmin,grado5);

                        if(sumawp>arcos_subgrafo-gmin+EPS_CORTES)
                        {
                            GRBLinExpr cut_sub=0;

                            if(Ady_Xvars[u1][u2]==true)
                                cut_sub=cut_sub+Xvars[u1][u2];

                            if(Ady_Xvars[u1][u3]==true)
                                cut_sub=cut_sub+Xvars[u1][u3];

                            if(Ady_Xvars[u1][u4]==true)
                                cut_sub=cut_sub+Xvars[u1][u4];

                            if(Ady_Xvars[u1][u5]==true)
                                cut_sub=cut_sub+Xvars[u1][u5];

                            if(Ady_Xvars[u2][u3]==true)
                                cut_sub=cut_sub+Xvars[u2][u3];

                            if(Ady_Xvars[u2][u4]==true)
                                cut_sub=cut_sub+Xvars[u2][u4];

                             if(Ady_Xvars[u2][u5]==true)
                                cut_sub=cut_sub+Xvars[u2][u5];

                             if(Ady_Xvars[u3][u4]==true)
                                cut_sub=cut_sub+Xvars[u3][u4];

                            if(Ady_Xvars[u3][u5]==true)
                                cut_sub=cut_sub+Xvars[u3][u5];

                            if(Ady_Xvars[u4][u5]==true)
                                cut_sub=cut_sub+Xvars[u4][u5];

                            addCut(cut_sub<=arcos_subgrafo-gmin);
                            cuts_subg++;
                            desig_sub=true;
                        }

                        if(desig_sub==false)
                        {
                            GRBLinExpr cut_ciclo=0;

                            sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u3]+Xreal_ij[u3][u4]+Xreal_ij[u4][u5]+Xreal_ij[u1][u5];
                            if(sumawp>3+EPSILON)
                            {
                                cut_ciclo=0;

                                if(Ady_Xvars[u1][u2]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u2];

                                if(Ady_Xvars[u2][u3]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u2][u3];

                                if(Ady_Xvars[u3][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u3][u4];

                                if(Ady_Xvars[u4][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u4][u5];

                                if(Ady_Xvars[u1][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u5];

                                addCut(cut_ciclo<=3);
                                cuts_ciclos++;
                                desig_ciclo=true;
                            }

                            sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u3]+Xreal_ij[u3][u5]+Xreal_ij[u4][u5]+Xreal_ij[u1][u4];
                            if(sumawp>3+EPSILON)
                            {
                                cut_ciclo=0;

                                if(Ady_Xvars[u1][u2]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u2];

                                if(Ady_Xvars[u2][u3]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u2][u3];

                                if(Ady_Xvars[u3][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u3][u5];

                                if(Ady_Xvars[u4][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u4][u5];

                                if(Ady_Xvars[u1][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u4];

                                addCut(cut_ciclo<=3);
                                cuts_ciclos++;
                                desig_ciclo=true;
                            }

                            sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u4]+Xreal_ij[u3][u4]+Xreal_ij[u3][u5]+Xreal_ij[u1][u5];
                            if(sumawp>3+EPSILON)
                            {
                                cut_ciclo=0;

                                if(Ady_Xvars[u1][u2]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u2];

                                if(Ady_Xvars[u2][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u2][u4];

                                if(Ady_Xvars[u3][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u3][u4];

                                if(Ady_Xvars[u3][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u3][u5];

                                if(Ady_Xvars[u1][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u5];

                                addCut(cut_ciclo<=3);
                                cuts_ciclos++;
                                desig_ciclo=true;
                            }

                            sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u4]+Xreal_ij[u4][u5]+Xreal_ij[u3][u5]+Xreal_ij[u1][u3];
                            if(sumawp>3+EPSILON)
                            {
                                cut_ciclo=0;

                                if(Ady_Xvars[u1][u2]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u2];

                                if(Ady_Xvars[u2][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u2][u4];

                                if(Ady_Xvars[u4][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u4][u5];

                                if(Ady_Xvars[u3][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u3][u5];

                                if(Ady_Xvars[u1][u3]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u3];

                                addCut(cut_ciclo<=3);
                                cuts_ciclos++;
                                desig_ciclo=true;
                            }

                            sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u5]+Xreal_ij[u3][u5]+Xreal_ij[u3][u4]+Xreal_ij[u1][u4];
                            if(sumawp>3+EPSILON)
                            {
                                cut_ciclo=0;

                                if(Ady_Xvars[u1][u2]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u2];

                                if(Ady_Xvars[u2][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u2][u5];

                                if(Ady_Xvars[u3][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u3][u5];

                                if(Ady_Xvars[u3][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u3][u4];

                                if(Ady_Xvars[u1][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u4];

                                addCut(cut_ciclo<=3);
                                cuts_ciclos++;
                                desig_ciclo=true;
                            }

                            sumawp=Xreal_ij[u1][u2]+Xreal_ij[u2][u5]+Xreal_ij[u4][u5]+Xreal_ij[u3][u4]+Xreal_ij[u1][u3];
                            if(sumawp>3+EPSILON)
                            {
                                cut_ciclo=0;

                                if(Ady_Xvars[u1][u2]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u2];

                                if(Ady_Xvars[u2][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u2][u5];

                                if(Ady_Xvars[u4][u5]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u4][u5];

                                if(Ady_Xvars[u3][u4]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u3][u4];

                                if(Ady_Xvars[u1][u3]==true)
                                    cut_ciclo=cut_ciclo+Xvars[u1][u3];

                                addCut(cut_ciclo<=3);
                                cuts_ciclos++;
                                desig_ciclo=true;
                            }
                          }
                        }

                    ///*************TIPO KNAPSACK *************************
                    ///************TEOREMA 4.9 **********************



                    if(desig_ciclo==false)
                    {

                            vector<double> r(WL_cb.size());
                            int cond_S=0;

                            for(unsigned int t=0;t<WL_cb.size();t++)
                            {
                                double  wS=wp[t][u1]+wp[t][u2]+wp[t][u3]+wp[t][u4]+wp[t][u5];
                                double WU_min=+INF;
                                for(unsigned int c=0;c<WL_cb[0].size();c++)
                                {
                                    WU_min=min(WU_min,WU_cb[t][c]);
                                }
                                if(wS<WU_min)
                                {
                                    cond_S++;///calcula el número de veces que se verifica la condicion
                                    r[t]=WU_min-wS;
                                }
                            }

                            if(cond_S==WL_cb.size())///si es igual, entonces  S es valido
                            {
                                ///calculando el conjunto X
                                list<int> conj_X;
                                conj_X.clear();

                                for(unsigned int jx=0;jx<n_nodos;jx++)
                                {
                                    if((jx!=u1)and(jx!=u2)and(jx!=u3)and(jx!=u4)and(jx!=u5))
                                    {
                                        int n_tx=0;
                                        for(unsigned int tx=0;tx<WL_cb.size();tx++)
                                        {
                                            if(wp[tx][jx]>r[tx])
                                            {
                                                n_tx++;
                                            }
                                        }

                                        if(n_tx==WL_cb.size())
                                        {
                                            conj_X.push_back(jx);
                                        }
                                    }
                                }

                                if(conj_X.size()>0)
                                {
                                    for(unsigned int tx=0;tx<WL_cb.size();tx++)
                                    {
                                        double valor_knp_u1=0;
                                        valor_knp_u1=wp[tx][u2]*Xreal_ij[u1][u2]+wp[tx][u3]*Xreal_ij[u1][u3]+wp[tx][u4]*Xreal_ij[u1][u4]+wp[tx][u5]*Xreal_ij[u1][u5];

                                        for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                        {
                                            valor_knp_u1=valor_knp_u1+(wp[tx][*itx]-r[tx])*Xreal_ij[u1][*itx];
                                        }

                                        double wS_t=wp[tx][u2]+wp[tx][u3]+wp[tx][u4]+wp[tx][u5];


                                        if(valor_knp_u1>wS_t+EPS_CORTES)
                                        {

                                            GRBLinExpr cut_knp_u1=0;
                                            if(Ady_Xvars[u1][u2]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u2]*Xvars[u1][u2];
                                            }
                                            if(Ady_Xvars[u1][u3]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u3]*Xvars[u1][u3];
                                            }
                                            if(Ady_Xvars[u1][u4]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u4]*Xvars[u1][u4];
                                            }

                                            if(Ady_Xvars[u1][u5]==true)
                                            {
                                                cut_knp_u1=cut_knp_u1+wp[tx][u5]*Xvars[u1][u5];
                                            }

                                            for(list<int>::const_iterator itx=conj_X.begin();itx!=conj_X.end();itx++)
                                            {
                                                if(Ady_Xvars[u1][*itx]==true)
                                                {
                                                    if(u1<*itx)
                                                        cut_knp_u1=cut_knp_u1+(wp[tx][*itx]-r[tx])*Xvars[u1][*itx];
                                                    else
                                                        cut_knp_u1=cut_knp_u1+(wp[tx][*itx]-r[tx])*Xvars[*itx][u1];
                                                }
                                            }

                                            if(cut_knp_u1.size()>0)
                                            {
                                                addCut(cut_knp_u1<=wS_t);
                                                cuts_mochila++;
                                            }
                                      }
                                    }
                                }
                            }
                    }




                 }///cierra u5
                 */
              }///cierra u4
           }///cierra u3
        }///cierra u2
      }///u1
    }
   }
  }///fin GRB_OPTIMAL
}
