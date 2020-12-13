#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <chrono>

//Вариант 3

using namespace std;
using namespace chrono;

int amount_of_writers = 0;
int amount_of_readers = 0;

int iter = 10;

bool sem_writer = false;

condition_variable cv;
mutex mtx;

string gen_string() {
    return "OpenMP";
}

class Note {
private:
    string text;
    int writer_id;
public:
    Note(string text_, int writer_id_) {
        text = text_;
        writer_id = writer_id_;
    }

    string GetText() {
        return text;
    }

    int GetWriterID() {
        return writer_id;
    }

    void SetText(string text_) {
        text = text_;
        return;
    }
};

class Reader {
private:
    int reader_id;
public:
    Reader() {
        reader_id = amount_of_readers++;
    }

    int GetID() {
        return reader_id;
    }

    void Read(vector<Note>& notes, int note_id) {
        //reading
        string note = notes[note_id].GetText();
        return;
    }
};

class Writer {
private:
    int writer_id;
public:
    Writer() {
        writer_id = amount_of_writers++;
    }

    void Write(vector<Note>& notes, int note_id) {
        //writing
        Note n = notes[note_id];
        n.SetText(gen_string());
        notes[note_id] = n;
        return;
    }

    int GetID() {
        return writer_id;
    }

    void Read(vector<Note>& notes, int note_id) {
        //reading
        string note = notes[note_id].GetText();
        return;
    }
};

void CreateWriters(int amount, vector<Writer>& writers) {
    for (int i = 0; i < amount; i++) {
        writers.push_back(Writer());
    }
    return;
}

void CreateReaders(int amount, vector<Reader>& readers) {
    for (int i = 0; i < amount; i++) {
        readers.push_back(Reader());
    }
    return;
}

void FillNotes(vector<Note>& notes, vector<Writer>& writers) {
    for (auto writer : writers) {
        int id = writer.GetID();
        for (int i = 0; i < 5; i++) {
            notes.push_back(Note("Empty", id));
        }
    }
    return;
}

string TimeTest()
{
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    return asctime(timeinfo);
}

void StartWriter(vector<Note> &notes, Writer writer) {
    while (true) {
        if (iter == 0) {
            cout << "Last iteration done. Exiting...";
            exit(0);
        }
        unique_lock<mutex> ulck(mtx);
        iter--;
        
        sem_writer = true;
        string time1 = TimeTest();
        cout << time1.substr(0, time1.size() - 1) << " **** Writer with id: " << writer.GetID() << " started changing his notes..." << endl;
        for (int i = 0; i < notes.size(); i++) {
            if (notes[i].GetWriterID() == writer.GetID() && notes[i].GetText() == "Empty") {
                writer.Write(notes, i);
                break;
            }
        }

        time1 = TimeTest();
        cout << time1.substr(0, time1.size() - 1) << " **** Writer with id: " << writer.GetID() << "; Now notes look like: " << endl;
        for (int i = 0; i < notes.size(); i++) {
            cout << "[    #" << i << ": " << "Author: " << notes[i].GetWriterID() << "; Text: " << notes[i].GetText() << "   ]" << endl;
        }

        time1 = TimeTest();
        cout << time1.substr(0, time1.size() - 1) << " **** Writer with id: " << writer.GetID() << " finished changing his notes... Now he is afk for 1 second." << endl;
        sem_writer = false;
        
        ulck.unlock();
        cv.notify_all();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
    }
}



void StartReader(vector<Note>& notes, Reader reader) {
    while (true) {
        unique_lock<mutex> lck(mtx);
        while (sem_writer == true) {
            cv.wait(lck);
        }
        string time1 = TimeTest();
        cout << time1.substr(0, time1.size() - 1) << " ---- Reader with id: " << reader.GetID() << " started searching the notes..." << endl;
        lck.unlock();
        for (int i = 0; i < notes.size(); i++) {
            reader.Read(notes, i);
        }
        lck.lock();
        time1 = TimeTest();
        cout << time1.substr(0, time1.size() - 1) <<  " ---- Reader with id: " << reader.GetID() << " finished searching the notes. Now he is afk for 1 sec. " << endl;
        lck.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

signed main() {

    int n, k;

    cout << "Enter amount of writers: ";
    cin >> n;
    while (true)
    {
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Wrong input! Try again: ";
            cin >> n;
        }
        if (!cin.fail())
            break;
    }

    cout << "Enter amount of readers: ";
    cin >> k;
    while (true)
    {
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Wrong input! Try again: ";
            cin >> k;
        }
        if (!cin.fail())
            break;
    }

    vector<Note> notes;
    vector<Writer> writers;
    vector<Reader> readers;

    CreateWriters(n, writers);
    CreateReaders(k, readers);
    FillNotes(notes, writers);

    for (auto x : notes) {
        cout << x.GetWriterID() << x.GetText() << endl;
    }
    thread *vec_reader = new thread[k];
    thread* vec_writer = new thread[n];
    for (int reader_index = 0; reader_index < readers.size(); reader_index++) {
        vec_reader[reader_index] = thread(StartReader, ref(notes), readers[reader_index]);
    }

    for (int writer_index = 0; writer_index < writers.size(); writer_index++) {
        vec_writer[writer_index] = thread(StartWriter, ref(notes), writers[writer_index]);
    }

    for (int i = 0; i < k; i++) {
        vec_reader[i].join();
    }

    for (int i = 0; i < n; i++) {
        vec_writer[i].join();
    }

}


















