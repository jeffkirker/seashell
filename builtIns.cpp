#include "processTable.h"
#include "builtIns.h"
#include "seashell.h"


/*
Removes any processes from the process table that are no longer running.
*/
void remove_finished_processes(PTable &table)
{
    // Get vector of all currently running processes 
    std::vector<int> running = table.get_running();
    
    // Get vector of all pids currently in the table
    std::vector<int> pidList = table.get_all_pids();

    // Any pids in pidList that are not running are removed from the table
    for(int i = 0; i < (int)pidList.size(); i++)
    {
        if(std::find(running.begin(), running.end(), pidList[i]) == running.end())
        {
            table.remove_process(pidList[i]);
        }
    }
}

/*
Kills any process that is suspended, as well as removes it from the process table.
*/
void kill_suspended_processes(PTable &table)
{    
    // Get vector of all pids currently in the table
    std::vector<int> pidList = table.get_all_pids();

    // Any pids in pidList that are not running are removed from the table
    for(int i = 0; i < (int)pidList.size(); i++)
    {
        if(strcmp(table.get_process_status(pidList[i]).c_str(), "S") == 0)
        {
            builtin_kill(pidList[i], table);
        }
    }
}

/*
Prints out a formatted display of the currently running processes, as well as 
a summary of utilized resources. 

BUG: Only prints user and system time for processes actually waited on. 
ie., and process ran in the background does not get picked up by getrusage, 
unless the process exits while the process is running in the background.
*/
void jobs(PTable &table)
{
    remove_finished_processes(table);

    int count = table.count_processes();
    // printf("process count: %d\n", count);

    std::cout<<"\nRunning Processes:"<<"\n";
    if(count >= 1){
        table.display_table();
    }
    std::cout<<"Processes: \t "<<count<<" active\n";
    std::cout<<"Completed processes:"<<"\n";
    std::cout<<"User Time = \t "<<table.get_usr_time()<<" seconds\n";
    std::cout<<"Sys  Time = \t "<<table.get_sys_time()<<" seconds\n\n";
}

/*
Sends a kill signal to the specified PID and removes the process from the table
*/
void builtin_kill(pid_t pid, PTable &table)
{
    if(kill(pid, SIGKILL) == 0){
        table.remove_process(pid);    
    }
}

/*
Sends a stop signal to specified PID, and updates it's status in the table
*/
void suspend(pid_t pid, PTable &table)
{
    std::string stat = "S";
    if(kill(pid, SIGSTOP) == 0){

        table.update_process_status(pid, stat);    
    }
}
/*
Sends a continue signal to specified PID, and updates it's status in the table
*/
void resume(pid_t pid, PTable &table)
{
    std::string stat = "R";
    if(kill(pid, SIGCONT) == 0){
        table.update_process_status(pid, stat);    
    }
}

/*
Sleep the shell for specified length of time
*/
void builtin_sleep(int seconds)
{
    sleep(seconds);
}


/*
Kills or waits for all processes, and exits the shell.
*/
void builtin_exit(PTable &table)
{
    //Remove any finished processes from table
    remove_finished_processes(table);

    //Kill suspended processes
    kill_suspended_processes(table); 

    // Wait until running processes are done.
    while(table.count_processes() != 0)
    {
        wait(NULL);
        update_usage(table);
        remove_finished_processes(table);
    }
    std::cout<<"Resources used"<<"\n";
    std::cout<<"User time = \t "<<table.get_usr_time()<<"\n";
    std::cout<<"User time = \t "<<table.get_sys_time()<<"\n";
    
    _exit(0);
}
