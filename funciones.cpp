#include "funciones.h"


///GENERADOR DE INSTANCIAS ALEATORIAS
void generar_instancia_aleatoria(int n, int k, int numero)
{
vector<double> esperas(n,0.0);
vector<double> pesos(n,0.0);
vector<pair<double,double> > puntos(n);
vector<vector<double> > dist(n);

for(unsigned int ki=1;ki<=numero;ki++)
{

    for(unsigned int i=0;i<pesos.size();i++)
    {
        pesos[i]=aleatorio_entre(1.0,10.0);
        esperas[i]=aleatorio_entre(0.1,0.9);
    }

    for(unsigned int i=0;i<puntos.size();i++)
    {
        pair<double,double> p;
        p=make_pair(aleatorio_entre(-100,100),aleatorio_entre(-100,100));
        puntos[i]=p;
    }

    for(unsigned int i=0;i<dist.size();i++)
        dist[i].resize(n,0.0);

    for(unsigned int i=0;i<dist.size();i++)
        for(unsigned int j=0;j<dist[i].size();j++)
            if(i!=j)
            {
                dist[i][j]=sqrt(pow(puntos[i].first-puntos[j].first,2.0)+pow(puntos[i].second-puntos[j].second,2.0));
            }
    ostringstream Nombre;
    Nombre<<"entradas/random"<<ki<<"_"<<n<<"_"<<k<<".txt";
    ofstream f(Nombre.str().c_str());
    f<<n<<endl<<k<<endl;
    for(unsigned int i=0;i<dist.size();i++)
    {
        f<<pesos[i]<<"  "<<esperas[i]<<"  ";
        for(unsigned int j=0;j<dist[i].size();j++)
        {
            f<<dist[i][j]<<" ";
        }
        f<<endl;
    }
f.close();
}
}


///GENERADOR DE INSTANCIAS ALEATORIAS EN GRAFOS GENERALES
void generar_instancia_aleatoria_generales(double prob_arco, int n, int k, int numero)
{
if((prob_arco<1)and(prob_arco>0))
{
vector<double> esperas(n,0.0);
vector<double> pesos(n,0.0);
vector<pair<double,double> > puntos(n);
vector<vector<double> > dist(n);

for(unsigned int ki=1;ki<=numero;ki++)
{

    for(unsigned int i=0;i<pesos.size();i++)
    {
        pesos[i]=aleatorio_entre(1.0,10.0);
        esperas[i]=aleatorio_entre(0.1,0.9);
    }

    for(unsigned int i=0;i<puntos.size();i++)
    {
        pair<double,double> p;
        p=make_pair(aleatorio_entre(-100,100),aleatorio_entre(-100,100));
        puntos[i]=p;
    }

    for(unsigned int i=0;i<dist.size();i++)
        dist[i].resize(n,0.0);

    for(unsigned int i=0;i<dist.size();i++)
        for(unsigned int j=0;j<dist[i].size();j++)
        {
            double al=aleatorio_01();
            if((i!=j)and(prob_arco>=al))
            {
                dist[i][j]=sqrt(pow(puntos[i].first-puntos[j].first,2.0)+pow(puntos[i].second-puntos[j].second,2.0));
            }
            else
            {
                dist[i][j]=0;
            }
        }
    ostringstream Nombre;
    Nombre<<"entradas/random_gen"<<ki<<"_"<<n<<"_"<<k<<".txt";
    ofstream f(Nombre.str().c_str());
    f<<n<<endl<<k<<endl;
    for(unsigned int i=0;i<dist.size();i++)
    {
        f<<pesos[i]<<"  "<<esperas[i]<<"  ";
        for(unsigned int j=0;j<dist[i].size();j++)
        {
            f<<dist[i][j]<<" ";
        }
        f<<endl;
    }
f.close();
}
}
else
    cout<<"ERROR: PROBABILIDAD FUERA DE RANGO"<<endl;
}


///GENERADOR DE INSTANCIAS PEQUEÑAS A PARTIR DE LA MATRIZ DEL INEC
void generar_instancia_reducidas(int n, int k,int numero)
{
vector<double> esperas_orig(822,0.0);
vector<double> pesos_orig(822,0.0);
vector<vector<double> > dist_orig(822);

for(unsigned int i=0;i<dist_orig.size();i++)
    dist_orig[i].resize(822,0.0);

esperas_orig=leer_vector_archivo("entradas/Tiempo_Espera.txt");
pesos_orig=leer_vector_archivo("entradas/Tiempo_Servicio.txt");
dist_orig=leer_matriz_archivo("entradas/Tiempo_Encuestador.txt");


for(unsigned int ki=1;ki<=numero;ki++)
{
    vector<int> muestra;
    vector<bool> marca(822,false);

    int cont=0;
    while(cont<n)
    {
        int k=aleatorio_entre(1,821);
        if(marca[k]==false)
        {
            marca[k]=true;
            cont++;
        }
    }

    for(unsigned int i=0;i<marca.size();i++)
    {
        if(marca[i]==true)
            muestra.push_back(i);
    }



    ostringstream Nombre;
    Nombre<<"entradas/muestra"<<ki<<"_"<<n<<"_"<<k<<".txt";
    ofstream f(Nombre.str().c_str());
    f<<n<<endl<<k<<endl;
    for(unsigned int i=0;i<muestra.size();i++)
    {
        f<<pesos_orig[ muestra[i] ]<<"  "<<esperas_orig[ muestra[i] ]<<"  ";
        for(unsigned int j=0;j<muestra.size();j++)
        {
            f<<dist_orig[ muestra[i] ][ muestra[j] ]<<" ";
        }
    f<<endl;
    }
    f.close();
}
}



//leer instancia
void leer_instancia(string Nombre_Instancia,vector<double>&pesos, vector<double>& esperas, vector<vector<double> >&Distancias, int &n, int &k)
{
	vector<double>aux_pesos;
	vector<vector<double> >aux_distancia;
	//int k;
	ifstream fa;
	fa.open(Nombre_Instancia.c_str());
	fa >> n;
	cout << "Numero de nodos:"<<""<< n << endl;
	fa >> k;
	cout << "Numero de cliques a formarse:"<<""<< k << endl;
	//vector<vector<double>> A;
	//vector<double> pesos(n, 0.0);
	Distancias.resize(n);
	pesos.resize(n);
	esperas.resize(n);

	for (int i = 0; i<n; i++)
	{
		Distancias[i].resize(n);
	}
	//cout << "creando matriz dimensiones";
	for (int i = 0; i<n; i++)
	{
		fa >> pesos[i]>>esperas[i];
		for (int j = 0; j < n; j++)
		{
			fa >> Distancias[i][j];
		}
	}
	fa.close();
	cout << "Lectura de datos" << endl;
}


vector<double> leer_vector_archivo(string NOMBRE)
{
    int n;
    ifstream fa(NOMBRE.c_str());
    fa>>n;
    vector<double> x(n);
    for(unsigned int i=0;i<n;i++)
    {
        fa>>x[i];
    }
    fa.close();
    return x;
}

vector<vector<double> > leer_matriz_archivo(string NOMBRE)
{
    int n;
    ifstream fa(NOMBRE.c_str());
    fa>>n;
    vector < vector <double> > d(n);
    for(unsigned int i=0;i<d.size();i++)
        d[i].resize(n,0.0);

    for(unsigned int i=0;i<n;i++)
        for(unsigned int j=0;j<n;j++)
        {
            fa>>d[i][j];
        }
    fa.close();
    return d;
}

//imprimir matriz
void escribir_matriz(vector<vector<double> > A)
{
	int n;
	n = A.size();
	for (int i = 0; i<n; i++)
	{
		for (int j = 0; j<n; j++)
		{
			cout << A[i][j] << " ";
		}
		cout << endl;
	}
}

double media(vector<double>x)
{
	int n;
	n = x.size();
	double mu = 0;
	for (int i = 0; i < n; i++) {
		mu+=x[i];
	}
	return (mu+0.0)/n;
}
//desviación estandar
double desviacion(vector<double>x)
{
	int n;
	n = x.size();
	double mu = media(x);
	double des = 0;
	for (int i = 0; i < n; i++)
	{
		des +=(x[i] - mu)*(x[i] - mu);
	}

	return sqrt((des+0.0)/n);
}

double aleatorio_01()
{
    return (rand()%10001)/10000.0;
}

double aleatorio_entre(double a, double b)
{
    double x= a+(b-a)*aleatorio_01();
    return x;
}
