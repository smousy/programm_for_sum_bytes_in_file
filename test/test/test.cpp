#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include <boost/asio.hpp>
#include <windows.h>
#include <mutex>
#include <fstream>
#include "tinystr.h"
#include "tinyxml.h"
#include <boost/thread.hpp>
#include <queue>

using namespace std;
using namespace boost::asio;

struct result {
    string path;
    int summ;
};
io_service service;
mutex mu;
string my_path;
int num_of_files;
queue<string> paths;
queue<result> results;

bool check_directory(string path) {
    filesystem::directory_entry dir_path(path);
    if (dir_path.is_directory()) return true;
    else return false;
}
void read_directory(filesystem::path path) {
    for (auto& p : filesystem::directory_iterator(path)) {
        filesystem::directory_entry dir(p.path().string());
        if (dir.is_directory()) {
            read_directory(p.path());
        }
        else {
            mu.lock();
            paths.push(p.path().string());
            //cout << p.path().string();
            num_of_files++;
            mu.unlock();
        }
    }

}
void sum_bytes_of_file(string path) {
    
    ifstream file;
    file.open(path);
    if (file.is_open() == true) {
        int sum = 0;
        char ch;
        string file_string;
        while (file.get(ch)) {
            file_string += to_string(ch);
        }

        for (int i = 0; i < file_string.size(); i++) {
            sum += file_string[i];
        }
        mu.lock();
        //cout << path << " - " << sum << " - " << this_thread::get_id() << endl;
         cout << path << " - " << sum  << endl;
        mu.unlock();
        result res;
        res.path = path;
        res.summ = sum;
        mu.lock();
        results.push(res);
        mu.unlock();

    }
    else {
        mu.lock();
        cout << "ERROR" << path;
        mu.unlock();
    }
    file.close();
}
int get_num_of_cores() {
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}
void handle_thread() {
    service.run();
}
void create_XML_file() {
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
    doc.LinkEndChild(decl);

    TiXmlElement* element = new TiXmlElement("Files");
    doc.LinkEndChild(element);
    while (!results.empty()) {
        TiXmlElement* element2 = new TiXmlElement("File");
        result res = results.front();
        string p = res.path;
        cout << p << endl;
        const char* pathChar = p.c_str();
        element2->SetAttribute("path", pathChar);
        string s = to_string(res.summ);
        cout << s << endl;
        const char* sizeChar = s.c_str();
        results.pop();
        element2->SetAttribute("sum", sizeChar);
        element->LinkEndChild(element2);
    }

    string full_name = my_path + "\\info.xml";
    const char* full_char_name = full_name.c_str();
    doc.SaveFile(full_char_name);
}
void create_task_queue() {
    filesystem::directory_entry path(my_path);
    read_directory(path);

    for (int i = 0; i < num_of_files; i++) {
        mu.lock();
        string curr_path = paths.front();
        paths.pop();
        mu.unlock();
        service.post(boost::bind(sum_bytes_of_file, curr_path));
    }
}
void consumers() {
 boost::thread_group threads;
    for (int i = 0; i < get_num_of_cores(); i++) {
        threads.create_thread(handle_thread);
    }
    threads.join_all();
}
void producer() {
    thread curr_producer(create_task_queue);
    curr_producer.join();
}

int main()
{
    setlocale(LC_ALL, "ru");
    cout << "Введите путь к директории: ";
    while (1) {
        cin >> my_path;
        if (check_directory(my_path)) break;
        else cout << "Неверный путь" << endl << "Попробуйте еще раз:";
    }

    producer();
    consumers();
    create_XML_file();
}