#include "processTable.h"
#include "builtIns.h"
#include "seashell.h"
#define LINE_LENGTH 100   // Max # of characters in an input line
#define MAX_ARGS 7        // Max number of arguments to a command
#define MAX_LENGTH 20     // Max # of characters in an argument
#define MAX_PT_ENTRIES 32 // Max entries in the Process Table

/*
Update the total user and system times in the table.
*/
void update_usage(PTable &table)
{
    struct rusage buf;

    if (getrusage(RUSAGE_CHILDREN, &buf))
    {
        printf("failure");
    }
    table.set_usr_time(buf.ru_utime.tv_sec);
    table.set_sys_time(buf.ru_stime.tv_sec);
}

/*
Check each argument to see if it is an input file, returns the file name and 
it's index in the vector.
*/
int check_for_input(std::vector<std::string> args, std::string &input)
{
    std::string temp;
    for (int i = 1; i < (int)args.size(); i++)
    {
        temp = args[i];
        if (temp.at(0) == '<')
        {
            input = temp.substr(1);
            return i;
        }
    }
    return 0;
}

int check_for_output(std::vector<std::string> args, std::string &output)
{
    std::string temp;
    for (int i = 1; i < (int)args.size(); i++)
    {
        temp = args[i];
        if (temp.at(0) == '>')
        {
            output = temp.substr(1);
            return i;
        }
    }
    return 0;
}

/*
Function to run builtin commands.
*/
int check_built_ins(std::vector<std::string> args, PTable &table)
{
    if (strcmp(args.front().c_str(), "jobs") == 0)
    {
        update_usage(table);

        jobs(table);
        return 1;
    }
    else if (strcmp(args.front().c_str(), "kill") == 0)
    {
        pid_t pid;
        // Make sure there is actually a second argument and it is a valid input
        if (args.size() > 1)
        {
            try
            {
                pid = stoi(args[1]);
                builtin_kill(pid, table);
            }
            catch (const std::invalid_argument &ia)
            {
                std::cout << "Not a valid argument" << std::endl;
                return 1;
            }
        }
        return 1;
    }
    else if (strcmp(args.front().c_str(), "suspend") == 0)
    {
        pid_t pid;
        // Make sure there is actually a second argument and it is a valid input
        if (args.size() > 1)
        {
            try
            {
                pid = stoi(args[1]);
                suspend(pid, table);
            }
            catch (const std::invalid_argument &ia)
            {
                std::cout << "Not a valid argument" << std::endl;
                return 1;
            }
        }
        return 1;
    }
    else if (strcmp(args.front().c_str(), "sleep") == 0)
    {
        // Make sure there is actually a second argument and it is a valid input
        if (args.size() > 1)
        {
            try
            {
                builtin_sleep(stoi(args[1]));
            }
            catch (const std::invalid_argument &ia)
            {
                std::cout << "Not a valid argument" << std::endl;
                return 1;
            }
        }
        return 1;
    }
    else if (strcmp(args.front().c_str(), "wait") == 0)
    {
        // Make sure there is actually a second argument and it is a valid input
        if (args.size() > 1)
        {
            try
            {
                int status;
                waitpid(stoi(args[1]), &status, 0);
            }
            catch (const std::invalid_argument &ia)
            {
                std::cout << "Not a valid argument" << std::endl;
                return 1;
            }
        }
        return 1;
    }
    else if (strcmp(args.front().c_str(), "resume") == 0)
    {
        pid_t pid;
        // Make sure there is actually a second argument and it is a valid input
        if (args.size() > 1)
        {
            try
            {
                pid = stoi(args[1]);
                resume(pid, table);
            }
            catch (const std::invalid_argument &ia)
            {
                std::cout << "Not a valid argument" << std::endl;
                return 1;
            }
        }
        return 1;
    }
    else if (strcmp(args.front().c_str(), "exit") == 0)
    {
        update_usage(table);
        builtin_exit(table);
        return 1;
    }
    return 0;
}

/*
Parsing function. Takes input string and returns a vector of each token split
by white space.
*/
template <class Container>
void parse_input(const std::string &str, Container &cont)
{
    std::istringstream iss(str);
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter(cont));
}

/*
Function to execute non-builtin commands.
*/
int execute_command(std::vector<std::string> args, PTable &table)
{
    int status;
    int background = false;
    // size_t size = args.size();
    pid_t pid;

    // Input, output, pipe variables
    std::string input, output;
    int inputIndex = 0;
    int outputIndex = 0;

    int inputFlag = 0;
    int outputFlag = 0;
    int fd[2];

    // Get full command including args for the process table
    std::string cmd;
    for (std::vector<std::string>::const_iterator i = args.begin(); i != args.end(); ++i)
    {
        cmd = cmd + *i + " ";
    }

    // Check if the last argument is &, set background flag if so.
    if (strcmp(args.back().c_str(), "&") == 0)
    {
        background = true;
        args.pop_back();
    }

    // Check for input (<) and output (>) arguments.
    inputIndex = check_for_input(args, input);
    outputIndex = check_for_output(args, output);
    if (outputIndex != 0)
    {
        outputFlag = 1;
        args.pop_back();
    }
    if (inputIndex != 0)
    {
        inputFlag = 1;
        args.pop_back();
    }

    // Open a pipe before forking
    if (pipe(fd) < 0) /* create a pipe before forking a child */
    {
        perror("pipe error");
    }

    // Time to fork
    pid = fork();
    if (pid == 0)
    { // Child process
        // Child closes unused portion of pipe
        close(fd[0]);

        // Pipe output to provided filename
        if (outputFlag)
        {
            fd[1] = open(output.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
        }

        // If input file provided, append lines to arguments
        if (inputFlag)
        {
            std::string line;
            std::ifstream inputfile(input.c_str());
            if (inputfile.is_open())
            {
                while (getline(inputfile, line))
                {
                    args.push_back(line);
                }
                inputfile.close();
            }
        }
        // Reformat string vector into char* array for execvp
        char *argc[args.size() + 1];
        argc[args.size()] = NULL;
        for (std::size_t i = 0; i < args.size(); i++)
        {
            argc[i] = (char *)args[i].c_str();
        }

        if (execvp(args.front().c_str(), argc) == -1)
        {
            perror("Invalid command");
            _exit(0);
        }
    }
    else if (pid > 0)
    { // parent process

        // Add child pid and command to process table
        table.add_process(pid, cmd);

        // Parent process closes unused portion of pipe
        close(fd[1]);
        close(fd[0]);

        if (background != true)
        {
            waitpid(pid, &status, WUNTRACED);
        }
        else
        {
            waitpid(pid, &status, WNOHANG | WUNTRACED);
        }
    }
    else
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
        _exit(1);
    }
    return 1;
}

void ClearScreen()
{
    std::cout << std::string(100, '\n');
    std::printf(R"EOF(
                        _          _ _ 
                       | |        | | |
     ___  ___  __ _ ___| |__   ___| | |
    / __|/ _ \/ _` / __| '_ \ / _ \ | |
    \__ \  __/ (_| \__ \ | | |  __/ | |
    |___/\___|\__,_|___/_| |_|\___|_|_|
    )EOF");
    std::cout << std::string(2, '\n');
}

/*
Main shell function. Creates the process table, then runs an infinite loop 
that takes user inputs. Parses the inputs and either runs builtin commands, or
external commands.
*/
void cppshell()
{
    // Initializae process table instance
    PTable table;
    table.set_parent_pid(getpid());

    // Read command + argument input
    std::string token;
    std::string input;
    std::vector<std::string> args;
    ClearScreen();
    // Main shell loop
    while (true)
    {
        std::cout << "$ ";
        std::getline(std::cin, input, '\n');

        if (!input.empty())
        {
            // Parse input line into arguments
            parse_input(input, args);

            // Check if command is a built-in function
            if (!check_built_ins(args, table))
            {
                // Execute the commands if not a built-in
                execute_command(args, table);
            }
        }
        // Clear the vector for next input
        args.clear();
    }
}

int main()
{
    cppshell();
}
