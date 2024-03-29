/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/12 15:08:30 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 17:46:06 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "msh_strvec.h"

/*
** Allowed Functions:
**    malloc(), free()
**     - stdlib.h
**    access()
**     - unistd.h
**    open(), close(), read(), write()
**     - fcntl.h
**    opendir(), readdir(), closedir()
**     - dirent.h
**    getcwd(), chdir()
**     - unistd.h
**    stat(), lstat(), fstat()
**     - sys/stat.h
**    fork(), execve()
**     - unistd.h
**    wait(), waitpid(), wait3(), wait4()
**     - wait.h
**    signal(), kill()
**     - signal.h
**    exit()
**  For bonuses
**    tcgetattr()
**     - termios.h
**    getpid(), getpwuid()
**    ...
*/

# define EXPCHR(c) ((c) == '~' || (c) == '$')
# define ABSPATH(x) ((x)[0] == '/')
# define RELPATH(x) ((x)[0] != '/')
# define ROOTEDPATH(x) (ABSPATH(x))
# define DIRSEP '/'
# define ISDIRSEP(c) ((c) == '/')
# define PATHSEP(c) (ISDIRSEP(c) || (c) == 0)
# define FREE(s) if (s) free(s);

# define SHR8(x) (((x) >> 8) & 0xff)

typedef struct s_mainargs	t_margs;
typedef struct s_env	t_shenv;
typedef struct s_bc	t_bc;

struct	s_mainargs
{
	char	**v;
	char	**e;
	char	**a;
	int		c;
};

struct	s_env
{
	t_margs		*m;
	t_strlst	*envlst;
	char		*home;
	char		**path;
	char		**envp;
	char		**cmdv;
	char		*sl;
	int			wid;
	int			pwd_ex;
	int			path_ex;
	int			st;
	int			cmdc;
	int			ret;
	int			tty_output;
	int			tty_input;
	int			exp_dollar;
	int			expand_var;
	int			prompt_printed;
	int			signal_recv;
	int			exit_called;
};

struct	s_bc
{
	char	*cmd;
	void	(*f)(t_shenv*);
};

/*
** Mandatory Part:
**  - Create a mini UNIX command interpreter
**  - Must display a prompt (ex: "$> "), wait till a command has been typed and
**    submitted by pressing enter
**  - Redisplay the prompt after the command has been executed
**  - Pipes, redirections, and advanced functions are not required
**  - Must use the PATH variable to locate executables
**  - If an executable can't be found, display an appropriate error message and
**    redisplay the prompt
**  - Must implement errno myself
**  - Deal with PATH and the environment correctly (copy of system char **evp)
**  - Must implement 'echo','cd','setenv','unsetenv','env','exit' as builtins
**  - Must manage $ and ~ expansions
*/

/*
** Bonus Part:
**  - Management of signals, specifically Ctrl-C. Global variables are allowed
**    for this bonus
**  - Management of execution rights in PATH
**  - Auto completion (how tho)
**  - Separating commands with ";"
**  - Others that I think will be useful
*/

int		g_dbg;
t_shenv	*g_shenv;

int		msh_prompt(void);
void	msh_print_prompt(void);
char	*msh_readline(void);
void	msh_sigint(int s);
void	msh_panic(char *msg);

char	*strsub_rep(char *str, char *pat, char *rep);
char	*msh_dollar(char *ret, char *tmp);
char	*msh_tilde(char *ret, char *tmp);
char	*msh_expand(char *token);

int		check_token_sub(const char *tok);
char	**msh_tokenize(char *str);
void	msh_parse(char **inpt);

int		msh_repl(void);

int		msh_varlen(const char *s);
char	*get_string_value(const char *var);

int		msh_exec_path(t_shenv *e);
int		msh_exec_builtin(t_shenv *e);
int		msh_exec_pwd(t_shenv *e);
int		msh_exec(t_shenv *e);

char	**quote_strsplit(char *s, int c);
int		quote_wordcount(char *s, int c);

void	init_shenv(t_shenv *e, t_margs *m);

void	msh_debug_print(char *fmt, ...);

void	cd_builtin(t_shenv *e);
void	echo_builtin(t_shenv *e);
void	env_builtin(t_shenv *e);
void	exit_builtin(t_shenv *e);
void	setenv_builtin(t_shenv *e);
void	unsetenv_builtin(t_shenv *e);

#endif
