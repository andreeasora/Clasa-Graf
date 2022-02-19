#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <stack>
#include <bits/stdc++.h>
#include <climits>
using namespace std;

class Graf
{
   protected:
       int nrN, nrM; ///Numar noduri, numar muchii
       vector <vector <int>> listaAd; ///lista de adiacenta graf
   public:
       Graf(int const, int const, vector <vector <int>> &);
       Graf(const Graf &graf);
       ~Graf();
       int getNoduri() const;
       int getMuchii() const;
       vector <vector <int>> getLista() const;

       vector <int> bfs(int); ///returneaza vector de distante minime, plecand din nodul de start

       int nrComponenteConexe();

       void dfs(int, vector <int> &); ///dfs ce primeste argument nod de start si vectorul vizitat.
                                      ///folosit in problema numar componente conexe

       vector <int> sortareTopologica(); ///returneaza sortarea topologica

       bool havelHakimi(vector <int> &); ///false - nu exista graf / true - exista graf. Primeste argument un vector de grade

       vector <vector <int>> ctc(); ///returneaza componentele tare conexe

       vector <pair <int, int>> muchiiCritice(); ///returneaza vector cu muchiile critice dintr-un graf

       ///---Paduri de multimi disjuncte---
       int cauta(int, vector <int>); ///gaseste tatal unui nod
       void uneste(int, int, vector <int> &, vector <int> &); ///uneste doua noduri

       vector <vector <int>> royFloyd(); ///returneaza matricea drumurilor minime

       int diametruArbore();

       int fluxMaxim();

    private:
       void dfsMuchiiCritice(int, int, vector <int> &, vector <int> &, vector <int> &, vector <pair <int, int>> &);

       void dfs1CtcKosaraju(int, vector <int> &, stack <int>&); ///primul DFS pe graf
       void dfs2CtcKosaraju(int, vector <int> &, vector <vector<int>>, vector <int> &); ///al doilea DFS pe graful transpus

       bool bfsFlux(vector <int> &, vector <vector <int>> &); ///bfs pentru problema flux maxim - aflarea drumurilor de ameliorare
};

Graf :: Graf(int const nrNoduri, int const nrMuchii, vector <vector<int>> &lista)
{
     nrN = nrNoduri;
     nrM = nrMuchii;
     listaAd = lista;
}

Graf :: Graf(const Graf & graf)
{
     nrN = graf.nrN;
     nrM = graf.nrM;
     listaAd = graf.listaAd;
}

Graf :: ~Graf()
{
    nrN = 0;
    nrM = 0;
    listaAd.clear();
}

int Graf :: getNoduri() const
{
    return nrN;
}

int Graf :: getMuchii() const
{
    return nrM;
}

vector <vector<int>> Graf :: getLista() const
{
    return listaAd;
}

///BFS - O(N + M)
vector <int> Graf :: bfs(int start)
{
    vector <int> cost(nrN + 1, -1);
    int S;
    queue <int> coada;
    coada.push(start); ///adaugam in coada nodul de start
    cost[start] = 0; ///costul pentru nodul de start este 0
    while (!coada.empty()) ///cat timp avem elemente in coada
    {
        S = coada.front();
        for (int i = 0; i < listaAd[S].size(); i++) ///parcurgem vecinii nodului curent
        {
            int nodCurent = listaAd[S][i];
            if (cost[nodCurent] == -1) ///daca e nevizitat
            {
                cost[nodCurent] = cost[S] + 1; ///actualizam costul
                coada.push(nodCurent); ///adaugam in coada vecinul
            }
        }
        coada.pop(); ///eliminam nodul curent din coada
    }
    return cost;
}

///DFS - O(N + M)
void Graf :: dfs(int nod, vector <int> &viz)
{
    viz[nod] = 1; ///vizitam nodul curent
    for (int i = 0; i < listaAd[nod].size(); i++) ///parcurgem vecinii nodului curent
    {
        int nodCurent = listaAd[nod][i];
        if (viz[nodCurent] == 0) ///daca vecinul este nevizitat
            dfs(nodCurent, viz);
    }
}

int Graf :: nrComponenteConexe()
{
    vector <int> viz(nrN + 1, 0); ///initializam vectorul viz
    int nrCC = 0; ///numar componente conexe
    for (int i = 1; i <= nrN; i++)
    {
        if (viz[i] == 0) ///daca nodul este nevizitat
        {
            dfs(i, viz);
            nrCC ++;
        }
    }
    return nrCC;
}

///SortareTopologica - O(N + M)
vector <int> Graf :: sortareTopologica()
{
    vector <int> grade(nrN + 1);
    for (int i = 0; i < listaAd.size(); i++)
        for (int j = 0; j < listaAd[i].size(); j++)
            grade[listaAd[i][j]]++; ///calculam vectorul de grade interne
    queue <int> coada;
    vector <int> rez;
    for (int i = 1; i <= nrN; i++)
        if (grade[i] == 0)
            coada.push(i); ///adaugam in coada nodurile cu gradul intern 0
    while (!coada.empty())
    {
        int nodCurent = coada.front();
        rez.push_back(nodCurent);
        coada.pop();
        for (int i = 0; i < listaAd[nodCurent].size(); i++) ///parcurgem vecinii
        {
            int vecin = listaAd[nodCurent][i];
            grade[vecin]--; ///scadem 1 tuturor gradelor nodurilor adiacente
            if (grade[vecin] == 0) ///daca gradul intern a devenit 0, il adaugam in coada
                coada.push(vecin);
        }
    }
    return rez;
}

///HavelHakimi - O(N^2logN)
bool Graf :: havelHakimi(vector <int> &v)
{
    bool ok = true;
    while (ok == true)
    {
        sort(v.begin(), v.end(), greater<int>()); ///sortam descrescator gradele nodurilor
        if (v[0] == 0) ///daca toate gradele sunt egale cu 0 - primul element 0 in ordine descrescatoare
            break; ///exista graf
        int nr = v[0]; ///extragem primul grad
        v.erase(v.begin());
        if (nr > v.size()) ///daca gradul este mai mare decat numarul de elemente ramase
         {
            ok = false; ///nu exista graf
            break;
         }
        for (int i = 0; i < nr; i++) ///parcurgem urmatoarele nr elemente
        {
            v[i] = v[i] - 1; ///scadem 1 din grad
            if (v[i] < 0) ///daca gradul a devenit -1
                {
                    ok = false; ///nu exista graf
                    break;
                }
        }
    }
    return ok;
}

///Ctc - O(N + M)
void Graf :: dfs1CtcKosaraju(int nod, vector <int> &vizitat, stack <int> &stiva)
{
    vizitat[nod] = 1;
    for (int i = 0; i < listaAd[nod].size(); i++)
    {
        int nodCurent = listaAd[nod][i];
        if (vizitat[nodCurent] == 0)
            dfs1CtcKosaraju(nodCurent, vizitat, stiva);
    }
   stiva.push(nod); ///adaugam in stiva nodurile
}

void Graf :: dfs2CtcKosaraju(int nod, vector <int> &vizitatTr, vector <vector<int>> listaTr, vector <int> &vectorAux)
{
    vectorAux.push_back(nod); ///adaugam nodul la componenta curenta
    vizitatTr[nod] = 1;
    for (int i = 0; i < listaTr[nod].size(); i++)
    {
        int nodCurent = listaTr[nod][i];
        if (vizitatTr[nodCurent] == 0)
            dfs2CtcKosaraju(nodCurent, vizitatTr, listaTr, vectorAux);
    }
}

vector <vector <int>> Graf :: ctc()
{
    vector <vector<int>> listaTr(nrN + 1);
    for (int i = 0; i < listaAd.size(); i++)
        for (int j = 0; j < listaAd[i].size(); j++)
           listaTr[listaAd[i][j]].push_back(i); ///calculam graful transpus
    vector <int> vizitat(nrN + 1, 0);
    vector <int> vizitatTr(nrN + 1, 0);
    stack <int> stiva;
    vector <vector<int>> rez(nrN + 1);
    int nr = 0;
    for (int i = 1; i <= nrN; i++)
        if (vizitat[i] == 0)
            dfs1CtcKosaraju(i, vizitat, stiva); ///primul dfs
    while (!stiva.empty()) ///parcurgem stiva
    {
       int v = stiva.top();
       stiva.pop();
       if (vizitatTr[v] == 0) ///daca nodul curent din stiva e nevizitat
       {
          vector <int> vectorAux;
          dfs2CtcKosaraju(v, vizitatTr, listaTr, vectorAux); ///al doilea dfs
          rez.push_back(vectorAux); ///adaugam la solutie componenta curenta
          nr++; ///incrementam numarul de componente
       }
    }
    return rez;
}

///MuchiiCritice - O(N + M)
vector <pair <int, int>> Graf :: muchiiCritice()
{
    vector <int> vizitat(nrN + 1, 0);
    vector <int> nivel(nrN + 1, 0);
    vector <int> nivelInt(nrN + 1, 0);
    vector <pair <int, int>> rez;
    dfsMuchiiCritice(0, 0, vizitat, nivel, nivelInt, rez);
    return rez;
}

void Graf :: dfsMuchiiCritice(int nod, int tata, vector <int> &vizitat, vector <int> &nivel, vector <int> &nivelInt,
                              vector <pair <int, int>> &rez )
{
    vizitat[nod] = 1;
    nivel[nod] = nivel[tata] + 1; ///actualizam nivelul
    nivelInt[nod] = nivel[nod]; ///actualizam nivelul de intoarcere
    for (int i = 0; i < listaAd[nod].size(); i++)
    {
        int copil = listaAd[nod][i];
        if (copil != tata)
        {
            if (vizitat[copil] == 1)
                {
                    if (nivelInt[nod] > nivel[copil]) ///actualizam nivelul de intoarcere acolo unde gasim o muchie de intoarcere
                       nivelInt[nod] = nivel[copil];
                }
            else
            {
                dfsMuchiiCritice(copil, nod, vizitat, nivel, nivelInt, rez);
                if (nivelInt[nod] > nivelInt[copil]) ///actualizam nivelul de intoarcere in cazul in care un copil are acest nivel mai mic
                    nivelInt[nod] = nivelInt[copil];
                if (nivel[nod] < nivelInt[copil]) ///conditie muchii critice
                    rez.push_back(make_pair(nod, copil));
            }
        }
    }

}

///PaduriDeMultimiDisjuncte
///O(logN)
int Graf :: cauta(int nod, vector <int> tata)
{
    while (nod != tata[nod])
        nod = tata[nod];
    return nod;
}

///O(1)
void Graf :: uneste(int nod1, int nod2, vector <int> &tata, vector <int> &rang)
{
    int TataNod1 = cauta(nod1, tata);
    int TataNod2 = cauta(nod2, tata);
    if (TataNod1 == TataNod2)
        return;
    if (rang[TataNod1] < rang[TataNod2])
           tata[TataNod1] = TataNod2;
    else if (rang[TataNod1] > rang[TataNod2])
             tata[TataNod2] = TataNod1;
         else
         {
            tata[TataNod2] = TataNod1;
            rang[TataNod1]++;
         }
}

///RoyFloyd - O(N^3)
vector <vector <int>> Graf :: royFloyd()
{
    ///se incearca pentru orice pereche de noduri {i, j} sa se obtina un drum de cost mai mic prin noduri intermediare t
    for (int t = 0; t < nrN; t++)
      for (int i = 0; i < nrN; i++)
        for (int j = 0; j < nrN; j++)
            if (listaAd[i][t] + listaAd[t][j] < listaAd[i][j])
                    listaAd[i][j] = listaAd[i][t] + listaAd[t][j];
    return listaAd;
}

///DiametrulUnuiArbore - O(N) - n noduri, n - 1 muchii
int Graf :: diametruArbore()
{
    int maxim = 0, ultimulElement;
    vector <int> rez = bfs(1); ///primul BFS din nodul 1
    for (int i = 1; i < rez.size(); i++)
        if (rez[i] > maxim)
        {
            maxim = rez[i];
            ultimulElement = i;
        }
    rez = bfs(ultimulElement); ///al doilea BFS din nodul in care am ajuns in prima parcurgere
    maxim = 0;
    for (int i = 1; i < rez.size(); i++)
        if (rez[i] > maxim)
            maxim = rez[i];
    return maxim + 1; /// + 1 deoarece functia BFS folosita returneaza costurile, iar problema ne cere numarul de noduri din drum
}

///FluxMaxim - O(NM^2)
bool Graf :: bfsFlux(vector <int> &tata, vector <vector <int>> &grafRezidual)
{
    ///BFS pentru a gasi un drum de ameliorare
    vector <bool> vizitat(nrN + 1, false);
    queue <int> coada;
    coada.push(1);
    vizitat[1] = true;
    tata[1] = -1;
    while(!coada.empty())
    {
        int nodCurent = coada.front();
        for (int i = 1; i <= nrN; i++)
           if (vizitat[i] == false && grafRezidual[nodCurent][i] > 0)
           {
               if (i == nrN) ///daca am ajuns in destinatie
               {
                   tata[i] = nodCurent;
                   return true;
               }
               coada.push(i);
               tata[i] = nodCurent;
               vizitat[i] = true;
            }
        coada.pop();
    }
    return false;
}

int Graf :: fluxMaxim()
{
    int fluxulMaxim = 0, fluxCurent, tataCurent;
    vector <vector <int>> grafRezidual(nrN + 1, vector <int>(nrN + 1, 0)); ///Graf rezidual - graf in care retinem si muchia inversa
    vector <int> tata(nrN + 1, 0);
    for (int i = 1; i <= nrN; i++)
         for (int j = 1; j <= nrN; j++)
             grafRezidual[i][j] = listaAd[i][j];
    while (bfsFlux(tata, grafRezidual) == true) ///cat timp gasim un drum de ameliorare
    {
        fluxCurent = INT_MAX;
        for (int v = nrN; v != 1; v = tata[v]) ///gasim valoarea cu care putem mari fluxul (minimul dintre capacitatile ramase)
        {
            tataCurent = tata[v];
            fluxCurent = min(fluxCurent, grafRezidual[tataCurent][v]);
        }
        for (int v = nrN; v != 1; v = tata[v]) ///actualizam graful rezidual
        {
            tataCurent = tata[v];
            grafRezidual[v][tataCurent] += fluxCurent; ///pe muchia x -> y marim fluxul cu acea valoare
            grafRezidual[tataCurent][v] -= fluxCurent; ///pe muchia y -> x micsoram fluxul cu acea valoare
        }
        fluxulMaxim += fluxCurent;
    }
    return fluxulMaxim;
}

///---------------------Graf Ponderat - Muchii cu costuri-------------------------------
class GrafPonderat : public Graf
{
    private:
        vector <vector <pair <int, int>>> muchiiCuCost;

        ///Functii APM Kruskal
        int findTata(int, vector <int>); ///gaseste tatal
        void setTata(int, int, vector <int> &); ///seteaza tatal

    public:
        GrafPonderat(int, int, vector <vector <int>> &, vector <vector<pair <int, int>>> &);

        vector <pair<int, int>> apmKruskal(ostream &, vector <vector <int>>); ///returneaza muchiile din APM si afiseaza costul total si numarul muchiilor din APM

        vector <int> dijkstra(int); ///returneaza vectorul de distante minime, plecand din nodul de start

        vector <int> bellmanford(int); ///returneaza vectorul de distante minime, plecand din nodul de start

        vector <int> cicluEuler(int); ///vom folosi aceasta functie in clasa graf ponderat deoarece data membru muchiiCuCost
                                      ///o vom trata ca fiind un vector de vectori de perechi de forma {vecin, index muchie}
};

GrafPonderat :: GrafPonderat(int nrNoduri, int nrMuchii, vector <vector <int>> &lista,
                             vector <vector<pair<int, int>>> &muchiiCost) : Graf(nrNoduri, nrMuchii, lista) ///apelam constructorul din baza
{
    muchiiCuCost = muchiiCost;
}

///Apm - Kruskal - O(MlogN)
int GrafPonderat :: findTata(int nod, vector <int> tata)
{
    while (nod != tata[nod])
        nod = tata[nod];
    return nod;
}

void GrafPonderat :: setTata(int nod1, int nod2, vector <int> &tata)
{
    tata[nod1] = tata[nod2];
}

vector <pair<int, int>> GrafPonderat :: apmKruskal(ostream &out, vector <vector <int>> muchiiCost)
{
    vector <pair<int, int>> rez;
    vector <int> tata(nrN + 1);
    for (int i = 1; i <= nrN; i++)
        tata[i] = i;
    ///sortam muchiile crescator dupa cost
    sort (muchiiCost.begin(), muchiiCost.end(), [] (vector <int> v1, vector <int> v2){return v1[2] < v2[2];});
    int costTotal = 0, nod1, nod2, tata1, tata2, nrMuchiiAPM = 0, indexMuchie = 0;
    while (nrMuchiiAPM < nrN - 1) ///parcurgem muchiile pana cand avem suficiente ca graful sa fie conex = (numar noduri - 1) muchii
    {
        nod1 = muchiiCost[indexMuchie][0]; ///nod1 din muchia curenta
        nod2 = muchiiCost[indexMuchie][1]; ///nod2 din muchia curenta
        ///cautam radacina arborilor partiali din care fac parte nodurile
        tata1 = findTata(nod1, tata);
        tata2 = findTata(nod2, tata);
        if (tata1 != tata2) ///am gasit muchie de adaugat in APM
        {
            if (tata1 < tata2)
                setTata(tata1, tata2, tata);
            else
                setTata(tata2, tata1, tata);
            rez.push_back(make_pair(nod1, nod2));
            costTotal = costTotal + muchiiCost[indexMuchie][2];
            nrMuchiiAPM++;
        }
        indexMuchie++;
    }
    out << costTotal << "\n";
    out << nrMuchiiAPM << "\n";
    return rez;
}

///Dijkstra - O(MlogN)
vector <int> GrafPonderat :: dijkstra(int nodStart)
{
    vector <int> distante(nrN + 1, INT_MAX); ///initial distantele sunt egale cu infinit
    vector <int> inCoada(nrN + 1, 0);
    priority_queue <pair <int, int>, vector <pair <int, int>>, greater<pair <int, int>>> coada; ///perechi {cost, nod}
    distante[nodStart] = 0;
    coada.push({0, nodStart});
    inCoada[nodStart] = 1;
    while (!coada.empty())
    {
        int nodCurent = coada.top().second;
        coada.pop();
        inCoada[nodCurent] = 0;
        for (int i = 0; i < muchiiCuCost[nodCurent].size(); i++)
        {
            int vecin, costVecin;
            vecin = muchiiCuCost[nodCurent][i].first;
            costVecin = muchiiCuCost[nodCurent][i].second;
            if (distante[nodCurent] + costVecin < distante[vecin])
    ///daca distanta pana la nodul curent + costul pana la nodul adiacent < distanta nodului adiacent, actualizam aceasta distanta
            {
                distante[vecin] = distante[nodCurent] + costVecin;
                if (inCoada[vecin] == 0)
                {
                    coada.push({distante[vecin], vecin});
                    inCoada[vecin] = 1;
                }
            }
        }
    }
    return distante;
}

///Bellmanford - O(MNlogN)
vector <int> GrafPonderat :: bellmanford(int nodStart) ///relaxam de nr_noduri - 1 ori
{
    vector <int> distante(nrN + 1, INT_MAX);
    vector <int> inCoada(nrN + 1, 0);
    vector <int> vizitat(nrN + 1, 0);
    priority_queue <pair <int, int>, vector <pair <int, int>>, greater<pair <int, int>>> coada;
    int ok = 1;
    distante[nodStart] = 0;
    coada.push({0, nodStart});
    inCoada[nodStart] = 1;
    while (!coada.empty())
    {
        int nodCurent = coada.top().second;
        coada.pop();
        inCoada[nodCurent] = 0;
        vizitat[nodCurent]++;
        if (vizitat[nodCurent] > nrN - 1) ///daca am vizitat un nod de mai mult de (nr_noduri - 1) ori
        {
             ok = 0; ///am ciclu negativ
             break;
        }
        for (int i = 0; i < muchiiCuCost[nodCurent].size(); i++)
        {
            int vecin, costVecin;
            vecin = muchiiCuCost[nodCurent][i].first;
            costVecin = muchiiCuCost[nodCurent][i].second;
            if (distante[nodCurent] + costVecin < distante[vecin])
            {
                distante[vecin] = distante[nodCurent] + costVecin;
                if (inCoada[vecin] == 0)
                {
                    coada.push({distante[vecin], vecin});
                    inCoada[vecin] = 1;
                }
            }
        }
    }
    if (ok == 0)
         distante.clear();
    return distante;
}

///CicluEulerian - O(N + M)
vector <int> GrafPonderat :: cicluEuler(int nodStart)
{
    vector <int> vizitat(nrM + 1, 0);
    vector <int> rez;
    stack <int> st;
    st.push(nodStart);
    while (!st.empty())
    {
        int nod = st.top();
        if (muchiiCuCost[nod].size() != 0)
        {
            int nodCurent = muchiiCuCost[nod].back().first;
            int muchieCurenta = muchiiCuCost[nod].back().second;
            muchiiCuCost[nod].pop_back();
            if (vizitat[muchieCurenta] == 0)
            {
                vizitat[muchieCurenta] = 1;
                st.push(nodCurent);
            }
        }
        else
        {
            st.pop();
            rez.push_back(nod);
        }
    }
    return rez;
}

///-------------------------BFS------------------------------
void problemaBfs()
{
    int n, s, st, dr, m;
    /*cout << "Numerotare noduri de la index 1! Graf orientat!" << "\n";
    cout << "Introduceti numarul de noduri: ";
    cin >> n;
    cout << "Introduceti numarul de muchii: ";
    cin >> m;
    cout << "Introduceti nodul start: ";
    cin >> s;
    cout << "Introduceti muchiile: ";*/
    ifstream fin("bfs.in");
    ofstream fout("bfs.out");
    fin >> n >> m >> s;
    vector <vector <int>> listaAd(n + 1);
    for (int i = 1; i <= m; i++)
    {
       // cin >> st >> dr;
        fin >> st >> dr;
        listaAd[st].push_back(dr);
    }
    Graf g(n, m, listaAd);
    vector <int> cost = g.bfs(s);
    for (int i = 1; i <= n; i++)
        fout << cost[i] << " ";
    fin.close();
    fout.close();
}

///-------------------------DFS--------------------------------
void problemaDfs()
{
    int n, m, st, dr;
    /*cout << "Numerotare noduri de la index 1! Graf neorientat!" << "\n";
    cout << "Introduceti numarul de noduri: ";
    cin >> n;
    cout << "Introduceti numarul de muchii: ";
    cin >> m;
    cout << "Introduceti muchiile: ";*/
    ifstream fin("dfs.in");
    ofstream fout("dfs.out");
    fin >> n >> m;
    vector <vector <int>> listaAd(n + 1);
    for (int i = 1; i <= m; i++)
    {
       // cin >> st >> dr;
        fin >> st >> dr;
        listaAd[st].push_back(dr);
        listaAd[dr].push_back(st);
    }
    Graf g(n, m, listaAd);
    int rez = g.nrComponenteConexe();
    fout << rez;
    fin.close();
    fout.close();
}

///------------------Havel Hakimi-------------------------
void problemaHavelHakimi()
{
    vector <int> v;
    int n, el;
    cout << "Introduceti numarul de elemente: ";
    cin >> n;
    cout << "Introduceti gradele: ";
    for (int i = 0; i < n; i++)
    {
        cin >> el;
        v.push_back(el);
    }
    vector <vector <int>> vgol = {};
    Graf g(0, 0, vgol);
    bool rez = g.havelHakimi(v);
    if (rez == true)
        cout << "Da, exista graf!";
    else cout << "Nu, nu exista graf!";
}

///------------------Sortare Topologica------------------
void problemaSortareTopologica()
{
    int n, m, st, dr;
    /*cout << "Numerotare noduri de la index 1! Graf orientat!" << "\n";
    cout << "Introduceti numarul de noduri: ";
    cin >> n;
    cout << "Introduceti numarul de muchii: ";
    cin >> m;
    cout << "Introduceti muchiile: ";*/
    ifstream fin("sortaret.in");
    ofstream fout("sortaret.out");
    fin >> n >> m;
    vector <vector <int>> listaAd (n + 1);
    for (int i = 1; i <= m; i++)
    {
       // cin >> st >> dr;
        fin >> st >> dr;
        listaAd[st].push_back(dr);
    }
    Graf g(n, m, listaAd);
    vector <int> rez = g.sortareTopologica();
    for (int i = 0; i < rez.size(); i++)
        fout << rez[i] << " ";
    fin.close();
    fout.close();
}

///---------------Componente Tare Conexe-------------------
void problemaComponenteTareConexe()
{
    int n, m, st, dr;
   /* cout << "Numerotare noduri de la index 1! Graf orientat!" << "\n";
    cout << "Introduceti numarul de noduri: ";
    cin >> n;
    cout << "Introduceti numarul de muchii: ";
    cin >> m;
    cout << "Introduceti muchiile: ";*/
    ifstream fin("ctc.in");
    ofstream fout("ctc.out");
    fin >> n >> m;
    vector <vector <int>> listaAd(n + 1);
    for (int i = 1; i <= m; i++)
    {
        //cin >> st >> dr;
        fin >> st >> dr;
        listaAd[st].push_back(dr);
    }
    Graf g(n, m, listaAd);
    vector <vector <int>> rez = g.ctc();
    int nrCtc = 0;
    for (int i = 0; i < rez.size(); i++)
       if (rez[i].size() != 0)
           nrCtc++;
    fout << nrCtc << "\n";
    for (int i = 0; i < rez.size(); i++)
    {
       if (rez[i].size() != 0)
        {
           for (int j = 0; j < rez[i].size(); j++)
              fout << rez[i][j] << " ";
           fout << "\n";
        }
    }
    fin.close();
    fout.close();
}

///------------------Muchii Critice---------------------
void problemaCriticalConnectionsLeetCode()
{
    int n, m, st, dr;
    cout << "Numerotare noduri de la index 0! Graf neorientat!" << "\n";
    cout << "Introduceti numarul de noduri: ";
    cin >> n;
    cout << "Introduceti numarul de muchii: ";
    cin >> m;
    cout << "Introduceti muchiile: ";
    vector <vector <int>> listaAd(n + 1);

    for (int i = 1; i <= m; i++)
    {
        cin >> st >> dr;
        listaAd[st].push_back(dr);
        listaAd[dr].push_back(st);
    }
    Graf g(n, m, listaAd);
    cout << "Muchii critice: ";
    vector <pair <int, int>> rez = g.muchiiCritice();
    for (int i = 0; i < rez.size(); i++)
        cout << rez[i].first << " " << rez[i].second << "\n";
}

///---------------Paduri De Multimi Disjuncte------------------
void problemaPaduriDeMultimiDisjuncte()
{
    ifstream fin("disjoint.in");
    ofstream fout("disjoint.out");
    int n, m, st, dr, operatie;
    fin >> n >> m;
    /*cout << "Introduceti numarul de multimi: ";
    cin >> n;
    cout << "Introduceti numarul de operatii efectuate: ";
    cin >> m;
    cout << "Operatie 1: Reuneste multimile unde se afla elementul x, respectiv y" << "\n";
    cout << "Operatie 2: Afiseaza 'DA' daca cele doua elemente se afla in aceeasi multime, 'NU' in caz contrar" << "\n";
    cout << "Introduceti valorile de forma: operatie - elementul x - elementul y: ";*/
    vector <vector <int>> vgol = {};
    vector <int> tata(n + 1);
    vector <int> rang(n + 1, 0);
    Graf g(n, m, vgol);
    for (int i = 1; i <= n; i++)
           tata[i] = i;
    for (int i = 1; i <= m; i++)
    {
        fin >> operatie >> st >> dr;
        if (operatie == 1)
        {
            g.uneste(st, dr, tata, rang);
        }
        else if (g.cauta(st, tata) == g.cauta(dr, tata))
                 fout << "DA" << "\n";
             else fout << "NU" << "\n";
    }
    fin.close();
    fout.close();
}

///---------------------APM Kruskal-------------------------
void problemaApmKruskal()
{
    ifstream fin("apm.in");
    ofstream fout("apm.out");
    int n, m, st, dr, cost;
    //cout << "Introduceti numarul de noduri: ";
    //cin >> n;
    //cout << "Introduceti numarul de muchii: ";
    //cin >> m;
    //cout << "Introduceti muchiile si costul acestora: " << "\n";
    fin >> n >> m;
    vector <vector <int>> lista = {};
    vector <vector <pair <int, int>>> muchiiCuCost = {{}};
    vector <vector <int>> muchiiCost;
    for (int i = 1; i <= m; i++)
    {
       // cin >> st >> dr >> cost;
        fin >> st >> dr >> cost;
        muchiiCost.push_back({st, dr, cost});
    }
    GrafPonderat g(n, m, lista, muchiiCuCost);
    vector <pair<int, int>> rezultat = g.apmKruskal(fout, muchiiCost);
    for (int i = 0; i < rezultat.size(); i++)
       fout << rezultat[i].first << " " << rezultat[i].second << "\n";
    fin.close();
    fout.close();
}

///----------------------------Dijkstra----------------------------------
void problemaDijkstra()
{
    ifstream fin("dijkstra.in");
    ofstream fout("dijkstra.out");
    int n, m, st, dr, cost;
    fin >> n >> m;
    vector <vector <int>> lista = {};
    vector <vector <pair <int, int>>> muchiiCuCost(n + 1);
    for (int i = 1; i <= m; i++)
    {
        fin >> st >> dr >> cost;
        muchiiCuCost[st].push_back(make_pair(dr, cost));
    }
    GrafPonderat g(n, m, lista, muchiiCuCost);
    vector <int> rez = g.dijkstra(1);
    for (int i = 2; i < rez.size(); i++)
        if (rez[i] == INT_MAX)
            fout << 0 << " ";
        else
            fout << rez[i] << " ";
    fin.close();
    fout.close();
}

///-------------------------Bellmanford---------------------------------
void problemaBellmanford()
{
    ifstream fin("bellmanford.in");
    ofstream fout("bellmanford.out");
    int n, m, st, dr, cost;
    fin >> n >> m;
    vector <vector <int>> lista = {};
    vector <vector <pair <int, int>>> muchiiCuCost(n + 1);
    for (int i = 1; i <= m; i++)
    {
        fin >> st >> dr >> cost;
        muchiiCuCost[st].push_back(make_pair(dr, cost));
    }
    GrafPonderat g(n, m, lista, muchiiCuCost);
    vector <int> rez = g.bellmanford(1);
    if (rez.size() == 0)
        fout << "Ciclu negativ!";
    else
    {
        for (int i = 2; i < rez.size(); i++)
            fout << rez[i] << " ";
    }
    fin.close();
    fout.close();
}

///------------------------Roy Floyd--------------------------------------
void problemaRoyFloyd()
{
    ifstream fin("royfloyd.in");
    ofstream fout("royfloyd.out");
    int n,el;
    fin >> n;
    vector <vector <int>> matrice(n + 1);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
           fin >> el;
           if (el == 0 && i != j)
               matrice[i].push_back(INT_MAX);
           else
               matrice[i].push_back(el);
        }
    Graf g(n, 0, matrice);
    vector <vector <int>> rez = g.royFloyd();
    for (int i = 0; i < n; i++)
    {
         for (int j = 0; j < n; j++)
            if (rez[i][j] != INT_MAX)
                fout << rez[i][j] << " ";
            else
                fout << 0 << " ";
         fout << "\n";
    }
    fin.close();
    fout.close();
}

///-----------------------Diametrul Unui Arbore-----------------------------
void problemaDiametrulUnuiArbore()
{
    int n, m, st, dr;
    ifstream fin("darb.in");
    ofstream fout("darb.out");
    fin >> n;
    vector <vector <int>> listaAd(n + 1);
    for (int i = 1; i <= n - 1; i++)
    {
        fin >> st >> dr;
        listaAd[st].push_back(dr);
        listaAd[dr].push_back(st);
    }
    Graf g(n, n - 1, listaAd);
    int rez = g.diametruArbore();
    fout << rez;
    fin.close();
    fout.close();
}

///----------------------Flux maxim--------------------------
void problemaFluxMaxim()
{
    ifstream fin("maxflow.in");
    ofstream fout("maxflow.out");
    int n, m, st, dr, cost;
    fin >> n >> m;
    vector <vector <int>> listaAd(n + 1, vector <int>(n + 1, 0));
    for (int i = 1; i <= m; i++)
    {
        fin >> st >> dr >> cost;
        listaAd[st][dr] = cost;
    }
    Graf g(n, m, listaAd);
    int flux_maxim = g.fluxMaxim();
    fout << flux_maxim;
    fin.close();
    fout.close();
}

///-------------------Ciclu Eulerian------------------------
void problemaCicluEulerian()
{
    ifstream fin("ciclueuler.in");
    ofstream fout("ciclueuler.out");
    int n, m, st, dr;
    fin >> n >> m;
    vector <vector <int>> lista = {};
    vector <vector <pair <int, int>>> listaVecinSiIndice(n + 1);
    for (int i = 1; i <= m; i++)
    {
        fin >> st >> dr;
        listaVecinSiIndice[st].push_back(make_pair(dr, i));
        listaVecinSiIndice[dr].push_back(make_pair(st, i));
    }
    int ok = 1;
    for (int i = 1; i <= n; i++)
    {
        if (listaVecinSiIndice[i].size() % 2 == 1) ///daca un nod are grad impar, nu exista ciclu eulerian
        {
            fout << -1;
            ok = 0;
        }
    }
    if (ok == 1)
    {
        GrafPonderat g(n, m, lista, listaVecinSiIndice);
        vector <int> rez = g.cicluEuler(1);
        for (int i = 0; i < rez.size() - 1; i++)
            fout << rez[i] << " ";
    }
    fin.close();
    fout.close();
}

///------------------Cuplajul Maxim-------------------
void problemaCuplajMaximInGrafBipartit()
{
    ifstream fin("cuplaj.in");
    ofstream fout("cuplaj.out");
    int l, r, m, st, dr;
    fin >> l >> r >> m;
    vector <vector <int>> listaAd(l + r + 2 + 1, vector <int>(l + r + 2 + 1, 0));
    for (int i = 1; i <= m; i++)
    {
        fin >> st >> dr;
        listaAd[st + 1][dr + l + 1] = 1;
        listaAd[1][st + 1] = 1;
        listaAd[dr + l + 1][l + r + 2] = 1;
    }
    Graf g(l + r + 2, m + l + r, listaAd);
    ///cuplajul maxim se reduce la a calcula fluxul maxim in reteaua de transport asociata
    ///adaugand un nod de start cu legaturi catre toate nodurile din stanga
    ///si un nod destinatie cu legaturi catre toate nodurile din dreapta
    ///costul pe fiecare muchie este 1
    int rez = g.fluxMaxim();
    fout << rez;
    fin.close();
    fout.close();
}

int main()
{
    cout << "---Sora Andreea-Ioana/grupa 234/Proiect - Algoritmi Fundamentali---" << "\n";
    cout << "Probleme: " << "\n";
    cout << "1. BFS - drumuri minime de la un nod start la toate celelalte" << "\n";
    cout << "2. DFS - Numarul de componente conexe" << "\n";
    cout << "3. Sortare Topologica" << "\n";
    cout << "4. Havel Hakimi" << "\n";
    cout << "5. Componente Tare Conexe" << "\n";
    cout << "6. Muchii Critice - LeetCode" << "\n";
    cout << "7. Paduri De Multimi Disjuncte" << "\n";
    cout << "8. Arbore Partial de Cost Minim - Kruskal" << "\n";
    cout << "9. Dijkstra - distante minime din nodul de start 1" << "\n";
    cout << "10. Bellmanford - distante minime din nodul de start 1" << "\n";
    cout << "11. RoyFloyd" << "\n";
    cout << "12. Diametrul unui arbore" << "\n";
    cout << "13. Flux maxim" << "\n";
    cout << "14. Ciclu eulerian" << "\n";
    cout << "15. Cuplajul maxim in graf bipartit" << "\n";
    cout << "\nIntroduceti numarul problemei pe care doriti sa o rulati: ";
    int numar;
    cin >> numar;
    cout << "\n";
    switch (numar)
    {
        case 1:
            problemaBfs();
            break;
        case 2:
            problemaDfs();
            break;
        case 3:
            problemaSortareTopologica();
            break;
        case 4:
            problemaHavelHakimi();
            break;
        case 5:
            problemaComponenteTareConexe();
            break;
        case 6:
            problemaCriticalConnectionsLeetCode();
            break;
        case 7:
            problemaPaduriDeMultimiDisjuncte();
            break;
        case 8:
            problemaApmKruskal();
            break;
        case 9:
            problemaDijkstra();
            break;
        case 10:
            problemaBellmanford();
            break;
        case 11:
            problemaRoyFloyd();
            break;
        case 12:
            problemaDiametrulUnuiArbore();
            break;
        case 13:
            problemaFluxMaxim();
            break;
        case 14:
            problemaCicluEulerian();
            break;
        case 15:
            problemaCuplajMaximInGrafBipartit();
            break;
        default:
            cout << "Numar invalid!";
            break;
    }
    return 0;
}
