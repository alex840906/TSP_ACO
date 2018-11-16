#include <vector>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <sstream>
#include <stdlib.h>

using namespace std;

vector<int> shortest_path(51);
vector<vector<int>> eil, antdata;
vector<vector<double>> distance_table, pheromone_table;
const int dim = 2, data_size = 51, ants_num = 51, alpha = 1, beta = 2, antPheromone = 100;
double init_pheromone = 0.000168, shortest_distance = 5000, p = 0.1f;

void readfile();
void init_ant(vector<vector<int>> &antdata);
void fitness(double ant_distance[]);
void create_distance_table(vector<vector<double>> &distance_table);
void create_pheromone_table(vector<vector<double>> &pheromone_table);
void transition_probibility(vector<vector<double>> &pheromone_table, double ant_distance[]);
void update_pheromone(vector<vector<double>> &pheromone_table, double ant_distance[]);
void debug(vector<int> &shortest_path, vector<vector<int>> &antdata);

int main()
{
    readfile();
    int i, j, iteration = 0, a = 0, run = 0;
    srand(time(NULL));
    double avg_distance = 0;
    while (run < 30)
    {
        init_ant(antdata);
        create_distance_table(distance_table);
        create_pheromone_table(pheromone_table);
        shortest_distance = 5000;
        iteration = 0;

        while (iteration <= 1000)
        {
            double ant_distance[ants_num] = {0};
            transition_probibility(pheromone_table, ant_distance);

            fitness(ant_distance);
            //cout<<pheromone_table[0][1]<<endl;
            //cout << iteration << endl;
            update_pheromone(pheromone_table, ant_distance);
            //cout<<pheromone_table[0][1]<<endl;
            //debug(shortest_path, antdata);

            iteration++;
        }
        for (i = 0; i < ants_num; i++)
            cout << shortest_path[i] << " ";
        cout << endl;
        avg_distance += shortest_distance;
        cout << "run " << run << "  "
             << "shortest distance = " << shortest_distance << endl;
        run++;
    }
    avg_distance /= 30;
    cout << endl
         << avg_distance;

    system("pause");
    return 0;
}

void readfile() /////////讀檔//////////////
{
    string line, tmp;
    int tmp_2;
    int i = 0;

    eil.resize(data_size);
    fstream file;
    file.open("TSP_ACO/eil51.txt", ios::in);

    while (getline(file, line))
    {
        stringstream t_line(line), ss;
        while (getline(t_line, tmp, ' '))
        {
            ss.clear();
            ss << tmp;
            ss >> tmp_2;
            eil[i].push_back(tmp_2);
        }
        i++;
    }
}

void init_ant(vector<vector<int>> &antdata) //////初始化螞蟻//////////
{
    antdata.resize(51);
    for (int i = 0; i < ants_num; i++)
        antdata[i].resize(ants_num);

    // for (int i = 0; i < ants_num; i++)
    //     for (int j = 0; j < ants_num; j++)
    //         antdata[i][j] = j;

    // int r, tmp;
    // for (int i = 0; i < ants_num; i++)
    //     for (int j = 0; j < ants_num; j++)
    //     {
    //         r = rand() % 51;
    //         antdata[i][0] = r;

    //         tmp = antdata[i][j];
    //         antdata[i][j] = antdata[i][r];
    //         antdata[i][r] = tmp;
    //     }
}

void fitness(double ant_distance[]) //////////距離計算/////////////
{
    int i, j, k;
    double current_distance = 0;
    for (i = 0; i < ants_num; i++)
        ant_distance[i] = 0;

    for (i = 0; i < ants_num; i++)
    {
        for (j = 0; j < ants_num - 1; j++)
        {
            current_distance = 0;
            for (k = 0; k < dim; k++)
                current_distance += (pow(eil[antdata[i][j]][k] - eil[antdata[i][j + 1]][k], 2));

            current_distance = sqrt(current_distance);
            ant_distance[i] += current_distance;
        }
        current_distance = 0.0;
        for (k = 0; k < dim; k++)
            current_distance += (pow(eil[antdata[i][j]][k] - eil[antdata[i][0]][k], 2));

        current_distance = sqrt(current_distance);
        ant_distance[i] += current_distance;
    }

    /////////計算最短路徑///////////
    for (i = 0; i < ants_num; i++)
        if (ant_distance[i] <= shortest_distance)
        {
            shortest_distance = ant_distance[i];
            for (j = 0; j < ants_num; j++)
                shortest_path[j] = antdata[i][j];
        }

    /////////計算最短路徑///////////
}

void create_distance_table(vector<vector<double>> &distance_table) /////////建距離表///////////
{
    distance_table.resize(ants_num, vector<double>(ants_num));

    int i, j, k;
    double current_distance;
    for (i = 0; i < ants_num; i++)
    {
        for (j = 0; j < ants_num; j++)
        {
            current_distance = 0;
            for (k = 0; k < dim; k++)
                current_distance += (pow(eil[i][k] - eil[j][k], 2));

            current_distance = sqrt(current_distance);
            distance_table[i][j] = current_distance;
        }
    }
}

void create_pheromone_table(vector<vector<double>> &pheromone_table) /////////建費洛蒙表/////////
{
    pheromone_table.resize(ants_num, vector<double>(ants_num));
    for (int i = 0; i < ants_num; i++)
        for (int j = 0; j < ants_num; j++)
            pheromone_table[i][j] = init_pheromone;
}

void transition_probibility(vector<vector<double>> &pheromone_table, double ant_distance[]) //////////決定路徑//////////
{
    int i, j, r, k;

    for (i = 0; i < ants_num; i++)
    {
        int visted[ants_num][ants_num] = {0};
        r = rand() % ants_num;
        visted[i][r] = 1;
        antdata[i][0] = r;
        int curren_city = r;

        for (k = 1; k < ants_num; k++)
        {
            double den = 0;
            for (j = 0; j < ants_num; j++)
                if (visted[i][j] == 0 && j != curren_city)
                    den = den + pheromone_table[curren_city][j] * pow(pow(distance_table[curren_city][j], -1), 2);

            double dart, interval = 0;
            dart = (double)rand() / RAND_MAX;
            bool test = false;

            if (den == 0)
            {
                cout << "k = " << k << endl;
                for (j = 0; j < ants_num; j++)
                    cout << visted[i][j];
                cout << endl;
            }

            //////圓盤///////////
            for (j = 0; j < ants_num; j++)
                if (visted[i][j] == 0 && j != curren_city)
                {
                    interval += pheromone_table[curren_city][j] * pow(pow(distance_table[curren_city][j], -1), 2) / den;

                    if (dart <= interval || (dart >= 1)||den==0)
                    {
                        curren_city = j;
                        visted[i][j] = 1;
                        antdata[i][k] = j;
                        test = true;
                        break;
                    }
                    
                }
            if (test == false)
            {
                cout << "k = " << k << endl;
                cout << "dart = " << dart << endl
                     << "interval = " << interval << endl
                     <<"den = "<<den<<endl;
            }

            //////圓盤/////////////
        }
    }
}

void update_pheromone(vector<vector<double>> &pheromone_table, double ant_distance[]) //////////費洛蒙更新///////////
{
    int i, j, k;
    for (i = 0; i < ants_num; i++)
        for (j = i; j < ants_num; j++)
        {
            pheromone_table[i][j] *= p;
            pheromone_table[j][i] = pheromone_table[i][j];
        }

    for (i = 0; i < ants_num; i++)
    {
        for (j = i; j < ants_num - 1; j++)
        {
            pheromone_table[antdata[i][j]][antdata[i][j + 1]] += antPheromone / ant_distance[i];
            pheromone_table[antdata[i][j + 1]][antdata[i][j]] = pheromone_table[antdata[i][j]][antdata[i][j + 1]];
        }

        pheromone_table[antdata[i][ants_num - 1]][antdata[i][0]] += antPheromone / ant_distance[i];
        pheromone_table[antdata[i][0]][antdata[i][ants_num - 1]] = pheromone_table[antdata[i][ants_num - 1]][antdata[i][0]];
    }
}

void debug(vector<int> &shortest_path, vector<vector<int>> &antdata)
{
    int i, j;
    int check[51][51];

    for (i = 0; i < ants_num; i++)
        for (j = 0; j < ants_num; j++)
            check[i][j] = antdata[i][j];

    for (i = 0; i < ants_num; i++)
        for (j = i; j < ants_num; j++)
        {
            if (check[i][j] >= check[i][j + 1])
            {
                int tmp;
                tmp = check[i][j];
                check[i][j] = check[i][j + 1];
                check[i][j + 1] = tmp;
            }
        }

    for (i = 0; i < ants_num - 1; i++)
        for (j = 0; j < ants_num; j++)
        {
            if (check[i][j] == check[i][j + 1])
            {
                for (int k = 0; k < ants_num; k++)
                {

                    cout << "error" << endl;
                    cout << check[i][k] << endl;
                }
                cout << check[i][j] << endl
                     << check[i][j + 1];
                system("pause");
            }
        }
    // if (shortest_distance <= 426)
    // {
    //     cout << "error" << endl;
    //     cout << shortest_distance << endl;
    //     for (i = 0; i < ants_num; i++)
    //         cout << shortest_path[i] << endl;
    // }
}
