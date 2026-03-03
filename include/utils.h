void unix_err(char *err_message);

int Fork(void);
void Execve(const char *pathname, char *const argv[], char *const envp[]);
