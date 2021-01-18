#include "microshell.h"

//____________________________ LIB ___________________________________

int		ft_strlen(char *str)
{
	int	i = 0;
	while(str[i])
		i++;
	return(i);
}

void	ft_putstr(char *str)
{
	write(2, str, ft_strlen(str));
}

char 	*ft_strdup(char *str)
{
	char	*ret;
	int		i = 0;

	if ((ret = (char*)malloc((ft_strlen(str) + 1) * sizeof(char))) == 0)
		exit_error();
	while(str[i])
	{
		ret[i] = str[i];
		i++;
	}
	ret[i] = '\0';
	return(ret);
}

void	exit_error()
{
	ft_putstr("error: fatal\n");
	exit(EXIT_FAILURE);
}

void	clear(t_cmd	*cmd)
{
	t_cmd *tmp;

	while(cmd)
	{
		int	i = 0;
		while(cmd->args[i])
		{
			free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
		tmp = cmd;
		cmd = cmd->next;
		free(tmp);
	}
}

//____________________________ FT  ___________________________________

t_cmd	*get_cmd(t_cmd *cmd, char **av, int n, int type)
{
	t_cmd	*new_cmd;
	int		i = 0;

	if (!(new_cmd = malloc(sizeof(*new_cmd))))
		exit_error();
	if (!(new_cmd->args = (char**)malloc(sizeof(char*) * (n + 1))))
		exit_error();
	new_cmd->prev = cmd;
	new_cmd->next = NULL;
	new_cmd->type = type;
	if (cmd != NULL)
		cmd->next = new_cmd;
	while (i < n)
	{
		new_cmd->args[i] = strdup(av[i]);
		i++;
	}
	new_cmd->args[i] = NULL;
	return (new_cmd);	
}

int		exec(t_cmd *cmd, char **env)
{
	int		ret = 0;
	while(cmd)
	{
		if (!strcmp("cd", cmd->args[0]))
			ret = cd(cmd);
		else
			ret = binary(cmd, env);
		cmd = cmd->next;
	}
	return(ret);
}

int		cd(t_cmd *cmd)
{
	int ret;
	int	i = 0;

	while (cmd->args[i])
		i++;
	if (i != 2)
	{
		ft_putstr("error: cd: bad arguments\n");
		return (1);
	}
	if ((ret = chdir(cmd->args[1])) < 0)
	{
		ft_putstr("error: cd: cannot change directory to ");
		ft_putstr(cmd->args[1]);
		ft_putstr("\n");
	}
	return (ret);
}

int		binary(t_cmd *cmd, char **env)
{
	pid_t	pid;
	int		status;
	int		ret = 0;
	
	//------ pip ------
	if (cmd->type == 1)
	{
		if (pipe(cmd->pip) < 0)
			exit_error();
	}
	//------ fork ------
	pid = fork();
	if (pid < 0)
		exit_error();
	else if (pid == 0)
	{
		//------- open pipe ------
		if (cmd->type == 1)
		{
			if (dup2(cmd->pip[1], 1) < 0)
				exit_error();
		}
		if (cmd->prev && cmd->prev->type == 1)
		{
			if (dup2(cmd->prev->pip[0], 0) < 0)
				exit_error();
		}
		//------- execute --------
		if ((ret = execve(cmd->args[0], cmd->args, env)) < 0)
		{
			ft_putstr("error: cannot execute ");
			ft_putstr(cmd->args[0]);
			ft_putstr("\n");		
		}
		exit(ret);
	}
	else
	{
		//-------- waitpid -------
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			ret = WEXITSTATUS(status);
		//------- close pipe ------
		if (cmd->type == 1)
		{
			close(cmd->pip[1]);
			if (!cmd->next)
				close(cmd->pip[0]);
		}
		if (cmd->prev && cmd->prev->type == 1)
			close(cmd->prev->pip[0]);
	}
	return (ret);
}

//____________________________ MAIN  ___________________________________

int		main(int ac, char **av, char **env)
{
	t_cmd	*cmd;
	t_cmd	*tmp;
	int		ret;
	int		i = 1;
	int		j = 1; 
	int		type;

	while (i < ac)
	{
		if (strcmp("|", av[i]) == 0 || strcmp(";", av[i]) == 0 || i + 1 == ac)
		{
			//------ get type --------
			if (strcmp("|", av[i]) == 0)
				type = 1;
			else if ( strcmp(";", av[i]) == 0)
				type = 2;
			else
			{
				type = 0;
				i++;
			}
			//------ create cmd ------
			if (i - j != 0)
			{
				tmp = get_cmd(tmp, &av[j], i - j, type);
				if (cmd == NULL)
					cmd = tmp;
			}
			j = i + 1;
		}
		i++;
	}
	//------ exec --------
	ret = exec(cmd, env);
	clear(cmd);
	return (ret);	
}


