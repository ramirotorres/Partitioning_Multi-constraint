#include "ProblemaMIP.h"


int main()
{
/*
generar_instancia_aleatoria_generales(0.8,20,3,5);
cin.get();

generar_instancia_reducidas(60,8,10);

generar_instancia_aleatoria_generales(0.8,30,6,10);
generar_instancia_aleatoria_generales(0.8,40,7,10);
generar_instancia_aleatoria_generales(0.8,50,8,10);
generar_instancia_aleatoria_generales(0.8,60,8,10);
*/


	cout << "   MODELO DE PARTICIONAMIENTO PII-001-2018" << endl << endl;


	ofstream f;
	f.open(REPORTE.c_str());
	f << "Instancia;Modelo;Objetivo;CotaInf;Gap;Tiempo;Vars;Restr;2P;CICLOS;SUBGRAFOS;KP;NODOS;" <<endl;
	f.close();

	vector<string> instancias;

	//instancias.push_back("entradas/random1_20_3.txt");

/*
	instancias.push_back("entradas/random_gen1_20_3.txt");
	instancias.push_back("entradas/random_gen2_20_3.txt");
	instancias.push_back("entradas/random_gen3_20_3.txt");
	instancias.push_back("entradas/random_gen4_20_3.txt");
	instancias.push_back("entradas/random_gen5_20_3.txt");

	instancias.push_back("entradas/random_gen1_30_6.txt");
	instancias.push_back("entradas/random_gen2_30_6.txt");
	instancias.push_back("entradas/random_gen3_30_6.txt");
	instancias.push_back("entradas/random_gen4_30_6.txt");
	instancias.push_back("entradas/random_gen5_30_6.txt");
	instancias.push_back("entradas/random_gen6_30_6.txt");
	instancias.push_back("entradas/random_gen7_30_6.txt");
	instancias.push_back("entradas/random_gen8_30_6.txt");
	instancias.push_back("entradas/random_gen9_30_6.txt");
	instancias.push_back("entradas/random_gen10_30_6.txt");

	instancias.push_back("entradas/random_gen1_40_7.txt");
	instancias.push_back("entradas/random_gen2_40_7.txt");
	instancias.push_back("entradas/random_gen3_40_7.txt");
	instancias.push_back("entradas/random_gen4_40_7.txt");
	instancias.push_back("entradas/random_gen5_40_7.txt");
	instancias.push_back("entradas/random_gen6_40_7.txt");
	instancias.push_back("entradas/random_gen7_40_7.txt");
	instancias.push_back("entradas/random_gen8_40_7.txt");
	instancias.push_back("entradas/random_gen9_40_7.txt");
	instancias.push_back("entradas/random_gen10_40_7.txt");

    instancias.push_back("entradas/random_gen1_50_8.txt");
	instancias.push_back("entradas/random_gen2_50_8.txt");
	instancias.push_back("entradas/random_gen3_50_8.txt");
	instancias.push_back("entradas/random_gen4_50_8.txt");
	instancias.push_back("entradas/random_gen5_50_8.txt");
	instancias.push_back("entradas/random_gen6_50_8.txt");
	instancias.push_back("entradas/random_gen7_50_8.txt");
	instancias.push_back("entradas/random_gen8_50_8.txt");
	instancias.push_back("entradas/random_gen9_50_8.txt");
	instancias.push_back("entradas/random_gen10_50_8.txt");

    instancias.push_back("entradas/random_gen1_60_8.txt");
	instancias.push_back("entradas/random_gen2_60_8.txt");
	instancias.push_back("entradas/random_gen3_60_8.txt");
	instancias.push_back("entradas/random_gen4_60_8.txt");
	instancias.push_back("entradas/random_gen5_60_8.txt");
	instancias.push_back("entradas/random_gen6_60_8.txt");
	instancias.push_back("entradas/random_gen7_60_8.txt");
	instancias.push_back("entradas/random_gen8_60_8.txt");
	instancias.push_back("entradas/random_gen9_60_8.txt");
	instancias.push_back("entradas/random_gen10_60_8.txt");
*/

	instancias.push_back("entradas/arandom1_20_3.txt");
	instancias.push_back("entradas/arandom2_20_3.txt");
	instancias.push_back("entradas/arandom3_20_3.txt");
	instancias.push_back("entradas/arandom4_20_3.txt");
	instancias.push_back("entradas/arandom5_20_3.txt");



	instancias.push_back("entradas/muestra1_30_6.txt");
	///instancias.push_back("entradas/muestra2_30_6.txt");
	instancias.push_back("entradas/muestra3_30_6.txt");
	instancias.push_back("entradas/muestra4_30_6.txt");
	///instancias.push_back("entradas/muestra5_30_6.txt");
	///instancias.push_back("entradas/muestra6_30_6.txt");
	instancias.push_back("entradas/muestra7_30_6.txt");
	///instancias.push_back("entradas/muestra8_30_6.txt");
	///instancias.push_back("entradas/muestra9_30_6.txt");
	instancias.push_back("entradas/muestra10_30_6.txt");
	instancias.push_back("entradas/random1_30_6.txt");
	instancias.push_back("entradas/random2_30_6.txt");
	instancias.push_back("entradas/random3_30_6.txt");
	instancias.push_back("entradas/random4_30_6.txt");
	instancias.push_back("entradas/random5_30_6.txt");
	instancias.push_back("entradas/random6_30_6.txt");
	///instancias.push_back("entradas/random7_30_6.txt");
	///instancias.push_back("entradas/random8_30_6.txt");
	///instancias.push_back("entradas/random9_30_6.txt");
	///instancias.push_back("entradas/random10_30_6.txt");

    ///instancias.push_back("entradas/muestra1_40_7.txt");
	///instancias.push_back("entradas/muestra2_40_7.txt");
	instancias.push_back("entradas/muestra3_40_7.txt");
	///instancias.push_back("entradas/muestra4_40_7.txt");
	///instancias.push_back("entradas/muestra5_40_7.txt");
	instancias.push_back("entradas/muestra6_40_7.txt");
	instancias.push_back("entradas/muestra7_40_7.txt");
	instancias.push_back("entradas/muestra8_40_7.txt");
	instancias.push_back("entradas/muestra9_40_7.txt");
	instancias.push_back("entradas/muestra10_40_7.txt");
	instancias.push_back("entradas/random1_40_7.txt");
	instancias.push_back("entradas/random2_40_7.txt");
	instancias.push_back("entradas/random3_40_7.txt");
	instancias.push_back("entradas/random4_40_7.txt");
	instancias.push_back("entradas/random5_40_7.txt");
	instancias.push_back("entradas/random6_40_7.txt");
	instancias.push_back("entradas/random7_40_7.txt");
	///instancias.push_back("entradas/random8_40_7.txt");
	///instancias.push_back("entradas/random9_40_7.txt");
	///instancias.push_back("entradas/random10_40_7.txt");

    instancias.push_back("entradas/muestra1_50_8.txt");
	instancias.push_back("entradas/muestra2_50_8.txt");
	instancias.push_back("entradas/muestra3_50_8.txt");
	instancias.push_back("entradas/muestra4_50_8.txt");
	instancias.push_back("entradas/muestra5_50_8.txt");
    instancias.push_back("entradas/muestra6_50_8.txt");
	instancias.push_back("entradas/muestra7_50_8.txt");
	instancias.push_back("entradas/muestra8_50_8.txt");
	instancias.push_back("entradas/muestra9_50_8.txt");
	instancias.push_back("entradas/muestra10_50_8.txt");

	instancias.push_back("entradas/random1_50_8.txt");
	instancias.push_back("entradas/random2_50_8.txt");
	instancias.push_back("entradas/random3_50_8.txt");
	instancias.push_back("entradas/random4_50_8.txt");
	instancias.push_back("entradas/random5_50_8.txt");
    instancias.push_back("entradas/random6_50_8.txt");
	instancias.push_back("entradas/random7_50_8.txt");
	instancias.push_back("entradas/random8_50_8.txt");
	instancias.push_back("entradas/random9_50_8.txt");
	instancias.push_back("entradas/random10_50_8.txt");

    instancias.push_back("entradas/muestra1_60_8.txt");
	instancias.push_back("entradas/muestra2_60_8.txt");
	instancias.push_back("entradas/muestra3_60_8.txt");
	instancias.push_back("entradas/muestra4_60_8.txt");
	instancias.push_back("entradas/muestra5_60_8.txt");
    instancias.push_back("entradas/muestra6_60_8.txt");
	instancias.push_back("entradas/muestra7_60_8.txt");
	instancias.push_back("entradas/muestra8_60_8.txt");
	instancias.push_back("entradas/muestra9_60_8.txt");
	instancias.push_back("entradas/muestra10_60_8.txt");


	instancias.push_back("entradas/random1_60_8.txt");
	instancias.push_back("entradas/random2_60_8.txt");
	instancias.push_back("entradas/random3_60_8.txt");
	instancias.push_back("entradas/random4_60_8.txt");
	instancias.push_back("entradas/random5_60_8.txt");
    instancias.push_back("entradas/random6_60_8.txt");
	instancias.push_back("entradas/random7_60_8.txt");
	instancias.push_back("entradas/random8_60_8.txt");
	instancias.push_back("entradas/random9_60_8.txt");
	instancias.push_back("entradas/random10_60_8.txt");


ofstream f_sol("salidas/solucion.txt");

	for(unsigned int i=0;i<instancias.size();i++)
	{

        int k, n; //número de cliques a formarse:k;nodos originales:n
        vector<double> pesos,esperas;
        vector<vector<double> > Distancias,WU,WL,w_pesos;

        leer_instancia(instancias[i], pesos,esperas, Distancias, n, k);
        ///matriz de pesos de nodos
        w_pesos.push_back(pesos);
        w_pesos.push_back(esperas);

        ///cargar las cotas superiores e inferiores
        WU.resize(2);
        WL.resize(2);
        for(unsigned int ii=0;ii<WU.size();ii++)
        {
            WU[ii].resize(k,0);
            WL[ii].resize(k,0);
        }

    double WLp, WUp,WLe,WUe;
	///pesos
	WLp = media(pesos)*((n+ 0.0) / k) - FACTOR_VAR*desviacion(pesos);
	WUp = media(pesos)*((n + 0.0) / k) + FACTOR_VAR*desviacion(pesos);
	///esperas
	WLe = media(esperas)*((n + 0.0) / k) - FACTOR_VAR*desviacion(esperas);
	WUe = media(esperas)*((n + 0.0) / k) + FACTOR_VAR*desviacion(esperas);

        for(unsigned int ii=0;ii<WU[0].size();ii++)
        {
            WL[0][ii]=WLp;
            WU[0][ii]=WUp;
            WL[1][ii]=WLe;
            WU[1][ii]=WUe;
        }

        ofstream f1;
        f1.open(REPORTE.c_str(), ios::app);

        Modelo1_xy MIP1;
        MIP1.incluir_parametros(false);
        cout<<"...CREANDO VARIABLES"<<endl;
        MIP1.crear_vars(Distancias,pesos,esperas,n,k);
        cout<<"...CREANDO RESTRICCIONES"<<endl;
        MIP1.crear_restricciones(pesos,esperas,k,n,WL,WU);

        MIP1.resolver();
        if(MIP1.Obt_Brecha()<100)
        {
            f1 << instancias[i] << ";xy_ip;"<< MIP1.Obt_Obj() <<";"<<MIP1.Lower()<<";" << MIP1.Obt_Brecha() << ";"
               << MIP1.Obt_tiempo()<<";"<< MIP1.ObtN_Vars() <<";"<<MIP1.ObtN_Restr()<<";0;0;0;0;"<<MIP1.Obt_N_Nodos()<<endl;
        }
        else
        {
            f1 << instancias[i] <<";xy_ip;Infactible;INF;--;" << MIP1.ObtN_Vars() << ";" << MIP1.ObtN_Restr()<< endl;
        }

        MIP1.incluir_parametros(false);
        MIP1.Pre_procesamiento(pesos,n,k);
        int m1_cuts_2p=0, m1_cuts_ciclos=0, m1_cuts_sg=0,m1_cuts_mochila,m1_nodecnt=0;
        MIP1.resolver_con_planos(m1_cuts_2p, m1_cuts_ciclos, m1_cuts_sg,m1_cuts_mochila, m1_nodecnt,w_pesos,WL,WU);

        ///MIP1.resolver();
        MIP1.presentar_sol(f_sol,instancias[i],n,k);

        if(MIP1.Obt_Brecha()<100)
        {
		f1 << instancias[i] << ";xy;"<< MIP1.Obt_Obj() << ";"<<MIP1.Lower()<<";" << MIP1.Obt_Brecha() << ";"
           <<MIP1.Obt_tiempo()<< ";" << MIP1.ObtN_Vars() << ";" << MIP1.ObtN_Restr()<<";"
           <<m1_cuts_2p<<";"<< m1_cuts_ciclos<<";" <<m1_cuts_sg<<";"<<m1_cuts_mochila<<";"<<m1_nodecnt<<endl;
        }
        else
        {
            f1 << instancias[i] <<";xy;Infactible;INF;--;" << MIP1.ObtN_Vars() << ";" << MIP1.ObtN_Restr()<< endl;
        }


        Modelo2_x MIP2;
        MIP2.incluir_parametros(false);
        cout<<"...CREANDO VARIABLES"<<endl;
        MIP2.crear_vars(Distancias,pesos,esperas,n,k);
        cout<<"...CREANDO RESTRICCIONES"<<endl;
        MIP2.crear_restricciones(pesos,esperas,n,k,WL,WU);

        MIP2.resolver();
        if(MIP2.Obt_Brecha()<100)
        {
		f1 << instancias[i] << ";x_proy_ip;"<< MIP2.Obt_Obj()<<";" <<MIP2.Lower() << ";" << MIP2.Obt_Brecha() << ";"
           << MIP2.Obt_tiempo() << ";" << MIP2.ObtN_Vars() << ";" << MIP2.ObtN_Restr()<<";0;0;0;0;"<<MIP2.Obt_N_Nodos()<<endl;
        }
        else
        {
            f1 << instancias[i] <<";xy_ip;Infactible;INF;--;" << MIP2.ObtN_Vars() << ";" << MIP2.ObtN_Restr()<< endl;
        }

        MIP2.incluir_parametros(false);
        MIP2.Pre_procesamiento(pesos,n,k);
        int m2_cuts_2p=0, m2_cuts_ciclos=0, m2_cuts_sg=0,m2_cuts_mochila,m2_nodecnt=0;
        MIP2.resolver_con_planos(m2_cuts_2p, m2_cuts_ciclos, m2_cuts_sg,m2_cuts_mochila, m2_nodecnt,w_pesos,WL,WU);
        MIP2.presentar_sol(f_sol,instancias[i],n,k);

        if(MIP2.Obt_Brecha()<100)
        {
		f1 << instancias[i] << ";Proy_x;"<< MIP2.Obt_Obj() << ";" <<MIP2.Lower()<<";"<< MIP2.Obt_Brecha() << ";"
           << MIP2.Obt_tiempo()<< ";" << MIP2.ObtN_Vars() << ";" << MIP2.ObtN_Restr()<<";"
           << m2_cuts_2p<<";"<< m2_cuts_ciclos<<";" <<m2_cuts_sg<<";"<<m2_cuts_mochila<<";"<<m2_nodecnt<<endl;

        }
        else
        {
            f1 << instancias[i] <<";Proy_x;Infactible;INF;--;" << MIP2.ObtN_Vars() << ";" << MIP2.ObtN_Restr()<< endl;
        }

        f1.close();
				//system("pause");

		}
f_sol.close();

	return 0;
}

