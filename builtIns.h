void jobs(PTable &table);

void builtin_kill(pid_t id, PTable &table);

void suspend(pid_t id, PTable &table);

void resume(pid_t id, PTable &table);

void builtin_sleep(int seconds);

void builtin_exit(PTable &table);

void remove_finished_processes(PTable &table);
