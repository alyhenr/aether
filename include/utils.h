void unix_err(char *err_message);

pid_t Fork(void);
void Execve(const char *pathname, char *const argv[], char *const envp[]);
