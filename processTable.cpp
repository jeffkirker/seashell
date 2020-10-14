/*
This file consists of all process table functions as declared in processTable.h
The process table implementation is essentially a singly linked list with 
some extra variables held by both the PTable objects as well as the process
struct objects. The processes represent the nodes of the linked list, with the
PTable class representing the list itself.
*/
#include "processTable.h"

PTable::PTable()
{
    head=NULL;
    tail=NULL;
    usrTime = 0;
    sysTime = 0;
}

/*
Add a process to the head of the process table
*/
void PTable::add_process(pid_t newPid, std::string cmd)
{   
    Process *temp=new Process;
    temp->pid=newPid;
    temp->command=cmd;
    temp->status = "R";
    temp->sec = 0;
    if(head != NULL)
    {
        temp->number=head->number + 1;
    } 
    else
    {
        temp->number = 0;
    }
    temp->next=head;
    head=temp;
}

// Removes the process with pid = id from the process table
void PTable::remove_process(pid_t id)
{
    int found = 0;
    Process *current=new Process;
    Process *previous=new Process;
    current=head;
    while(current != NULL)
    {
        if(current->pid == id){
            if(current == head){
                head=NULL;
                found = 1;
                break;
            }
            previous->next=current->next;
            found = 1;
            break;
        } else {
            previous = current;
            current=current->next;
        }
    }
}

/*
Returns a string containing the process status (either "S" or "R")
*/
std::string PTable::get_process_status(pid_t id)
{
    Process *current=new Process;
    Process *previous=new Process;
    current=head;
    while(current != NULL)
    {
        if(current->pid == id){
            return current->status;
        } else {
            previous = current;
            current=current->next;
        }
    }
}

/* 
Print out the process with the formatting required by the 'jobs' command in the
builtIns.cpp source file.
*/
void PTable::display_table()
{   
    std::cout<<"#    PID  S SEC  COMMAND"<<"\n";
    Process *temp=new Process;
    temp=head;
    while(temp!=NULL)
    {
        std::cout<<temp->number<<": "<<temp->pid<<"  "<<temp->status<<"  "<<this->get_pid_time(temp->pid)<<"   "<<temp->command<<"\n";
        temp=temp->next;
    }
}

/*
Count the number of processes in the table
*/
int PTable::count_processes()
{
    int count = 0;
    Process *temp = new Process;
    temp=head;
    while(temp != NULL)
    {
        count += 1;
        temp = temp->next;
    }
    return count;
}

/*
Basic setters and getters for private table class variables
*/
int PTable::get_usr_time(){
    return this->usrTime;
}

int PTable::get_sys_time(){
    return this->sysTime;
}

void PTable::set_parent_pid(pid_t pid){
    this->parentPid = pid;
}

pid_t PTable::get_parent_pid(){
    return this->parentPid;
}

void PTable::set_usr_time(int time)
{
    this->usrTime = time;
}
void PTable::set_sys_time(int time)
{
    this->sysTime = time;
}

/*
Open a pipe in order to run the pgrep command which returns all the child PIDs.
Format these pids as a vector and return them.
*/
std::vector<int> PTable::get_running(){
    FILE *stream;
    int max_size = 256;
    char buffer[max_size];
    std::ostringstream command;
    std::vector<int> data;
    
    // Use pgrep with -P tag to get a list of all pids running with the shell
    // parent pid.
    command << "pgrep -P "<<this->get_parent_pid()<<std::endl;
    std::string commandStr = command.str(); 

    // Turns all of the child pids into a string vector
    stream = popen(commandStr.c_str(), "r");
    if(stream)
    {
        while (!feof(stream)){
            if (fgets(buffer, max_size, stream) != NULL) 
            {
                data.push_back(std::stoi(buffer));
            }
        }
    }
    pclose(stream);
    return data;
}

/*
Create an integer vector of all of the pids in the table.
*/
std::vector<int> PTable::get_all_pids()
{
    std::vector<int> pidList;

    Process *temp = new Process;
    temp=head;
    while(temp != NULL)
    {
        pidList.push_back(temp->pid);
        temp = temp->next;
    }
    return pidList;
}

/*
Returns the current run time of the process specified
*/
time_t PTable::get_pid_time(pid_t pid)
{
    FILE *stream;
    int max_size = 256;
    char buffer[max_size];
    std::ostringstream command;
    time_t time =0;
    
    command << "ps -o etimes= -p "<<pid<<std::endl;
    std::string commandStr = command.str(); 

    // Opens a pipe to run the command to get the time
    stream = popen(commandStr.c_str(), "r");
    if(stream)
    {
        if (fgets(buffer, max_size, stream) != NULL) 
        {
            time=(std::stoi(buffer));
        }
    }
    pclose(stream);
    return time;
}

// void convert_to_seconds(const std::string& str, Container& cont)
// {
//     std::istringstream iss(str);
//     std::copy(std::istream_iterator<std::string>(iss),
//          std::istream_iterator<std::string>(),
//          std::back_inserter(cont));
// }

/*
Update the status of the specified process to the specified status.
*/
void PTable::update_process_status(pid_t id, std::string stat)
{
    Process *current=new Process;
    Process *previous=new Process;
    current=head;
    while(current != NULL)
    {
        if(current->pid == id){
            current->status = stat;
            break;
        } else {
            previous = current;
            current=current->next;
        }
    }
}