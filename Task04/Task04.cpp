#include <iostream>
#include <omp.h>
#include <utility>
#include <vector>
#include <ctime>
#include <stdlib.h>

//Вариант 25

using namespace std;

vector<bool> field;
pair<int, int> winner = { -1, -1 };

void startThread(int id, int l, int r)
{
    for (int i = l; i < r; i++) {
        if (field[i] == true) {
            winner = { id, i };
            #pragma omp critical 
            {
                cout << "Group " << id << " " << " found treasures!" << endl;
            }
            return;
        }
    }
    #pragma omp critical 
    {
        cout << "Group " << id << " " << " found nothing" << endl;
    }
    return;
}

void manage(int num, int area) {
#pragma omp parallel for
    for (int i = 0; i < num; i++) {
        startThread(i, i * area, i * area + area);
    }
    cout << "Found in " << winner.second << " by group " << winner.first << endl;
}

signed main() {
    int number_of_groups, area;
    cin >> number_of_groups >> area;

    field.resize(number_of_groups * area);
    srand(time(NULL));
    int treasure = rand() % field.size();
    cout << "Tsss... treasure in cell with number " << treasure << endl;
    field[treasure] = true;
    manage(number_of_groups, area);
}