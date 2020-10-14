#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <time.h>
#include <sys/resource.h> 
#include <signal.h>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

struct Process
{
    pid_t pid;
    std::string command;
    int number;
    std::string status;
    int sec;
    Process* next;
};

class PTable
{
    private:
        Process *head, *tail;
        int usrTime, sysTime;
        pid_t parentPid;
    public:
        PTable();

        void add_process(pid_t newPid, std::string command);
        void remove_process(pid_t id);
        std::string get_process_status(pid_t pid);
        void display_table();
        int count_processes();

        int get_usr_time();
        int get_sys_time();
        void set_usr_time(int time);
        void set_sys_time(int time);
        void set_parent_pid(pid_t pid);
        pid_t get_parent_pid();

        int check_pid(pid_t pid);
        std::vector<int> get_running();
        std::vector<int> get_all_pids();
        time_t get_pid_time(pid_t pid);
        void update_process_status(pid_t pid, std::string stat);
};	
