#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int strln(char *s)
{
	int i = 0;

	while (s[i])
		i++;
	return (i);
}

void ft_exit_fatal(void)
{
	write(2, "error: fatal\n", 14);
	exit(1);
}
void ft_exit_execve(char *s)
{
	write(2, "error: cannot execute ", 22);
	write(2, s, strln(s));
	write(2, "\n", 1);
	exit(1);
}

int	ft_cd(char **argv)
{
	if (strcmp(argv[0], "cd"))
		return (0);
	if (!argv[1] || argv[2])
		write(2, "error: cd: bad arguments\n", 25);
	else if (chdir(argv[1]) == -1)
	{
		write(2, "error: cd: cannot change directory to ", 38);
		write(2, argv[1], strln(argv[1]));
		write(2, "\n", 1);
	}
	return (1);
}

int main(int i, char **argv, char **envp)
{
	char	**tab;
	int		fd[2];
	pid_t	pid;

	tab = &argv[1];
	fd[0]= 0;
	fd[1]= 1;
	while (tab[0])
	{
		argv = tab;
		i = 0;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (argv[i])
			tab = &argv[i + 1];
		else
			tab = &argv[i];
		if (i == 0)
			i = i + 1 - 1;
		else if (!argv[i] || !strcmp(argv[i], ";"))
		{
			argv[i] = NULL;
			if (ft_cd(argv) == 0)
			{
				pid = fork();
				if (pid == -1)
					ft_exit_fatal();
				if (pid == 0)
				{
					execve(argv[0], argv, envp);
					ft_exit_execve(argv[0]);
				}
				waitpid(pid, NULL, 0);
			}
		}
		else
		{
			argv[i] = NULL;
			if (pipe(fd) == -1)
				ft_exit_fatal();
			pid = fork();
			if (pid == -1)
				ft_exit_fatal();
			if (pid == 0)
			{
				dup2(fd[1], 1);
				execve(argv[0], argv, envp);
				ft_exit_execve(argv[0]);
			}
			waitpid(pid, NULL, 0);
			dup2(fd[0], 0);
			close(fd[1]);
			close(fd[0]);
		}
	}
	return (0);
}
