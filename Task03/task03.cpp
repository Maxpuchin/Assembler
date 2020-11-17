#include <iostream>
#include <utility>
#include <vector>
#include <mutex>

//Вариант 25
 
using namespace std;

vector<bool> field;
pair<int, int> winner = { -1, -1 };
mutex mtx;

void startThread(int id, int l, int r)
{
    for (int i = l; i < r; i++) {
        if (field[i] == true) {
            winner = { id, i };
            mtx.lock();
            cout << "Group " << id << " " << " found treasures!" << endl;
            mtx.unlock();
            return;
        }
    }
    mtx.lock();
    cout << "Group " << id << " " << " found nothing" << endl;
    mtx.unlock();
    return;
}

void manage(int num, int area) {
    thread *thread_array = new thread[num] ;
    for (int i = 0; i < num; i++) {
        thread_array[i] = thread(startThread, i, i * area, i * area + area);
    }
    for (int i = 0; i < num; i++) {
        if (thread_array[i].joinable()) {
            thread_array[i].join();
        }
    }
    cout << "Found in " << winner.second << " by group " << winner.first << endl;
}

signed main() {
    int number_of_groups, area;
    cin >> number_of_groups >> area;
    field.resize(number_of_groups * area);
    srand(4);
    int treasure = rand() % field.size();
    field[treasure] = true;
    thread silver(manage, number_of_groups, area);
    silver.join();
}
