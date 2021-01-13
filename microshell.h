#ifndef MICROSHELL_H
# define MICROSHELL_H

//________________ INCLUDE _____________________

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//________________ STRUCT _____________________

typedef	struct 	s_cmd
{
	struct s_cmd	*next;
	struct s_cmd	*prev;
	int		type;
	char	**args;
	int		pip[2];
}				t_cmd;

//__________________ FT _______________________

int     ft_strlen(char *str);
void    ft_putstr(char *str);
char    *ft_strdup(char *str);
void    exit_error();
void    clear(t_cmd *cmd);

t_cmd   *get_cmd(t_cmd *cmd, char **av, int n, int type);
int     exec(t_cmd *cmd, char **env);

int     cd(t_cmd *cmd);
int     binary(t_cmd *cmd, char **env);

#endif
