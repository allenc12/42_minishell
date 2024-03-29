/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/01 01:20:10 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 17:45:35 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "ft_stdio.h"
#include <signal.h>

static t_bc	g_cmds[7] = {
	{ "cd", &cd_builtin },
	{ "echo", &echo_builtin },
	{ "env", &env_builtin },
	{ "exit", &exit_builtin },
	{ "setenv", &setenv_builtin },
	{ "unsetenv", &unsetenv_builtin },
};

int			msh_exec_builtin(t_shenv *e)
{
	register int	i;
	int				status;

	i = -1;
	status = 127;
	while (g_cmds[++i].cmd != 0)
	{
		if (ft_strequ(g_cmds[i].cmd, *e->cmdv))
		{
			g_cmds[i].f(e);
			status = e->ret;
			break ;
		}
	}
	msh_debug_print("exec_builtin: end status(%d, %d) ret(%d, %d)",
			SHR8(status), status & 0xff,
			SHR8(e->ret), e->ret & 0xff);
	return (status);
}

int			msh_exec_pwd(t_shenv *e)
{
	int		st;
	int		ex;
	pid_t	pid;
	pid_t	wid;

	msh_debug_print("exec_pwd: start ret(%d, %d)",
			SHR8(e->ret), e->ret & 0xff);
	pid = fork();
	signal(SIGINT, msh_sigint);
	if (!pid)
	{
		if ((ex = execve(*e->cmdv, e->cmdv, e->envlst->list)) == -1)
			exit(127);
		exit((e->pwd_ex = ex));
	}
	else if (pid < 0)
		msh_panic(*e->cmdv);
	else
	{
		wid = wait(&st);
		e->wid = wid;
		return ((e->st = st));
	}
	ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
	return (255);
}

#define CHKEP(x) (SHR8(x) != 127 && SHR8(x) != 255 && (x) != 11)
#define ISPWD(s) ((s)[0] == '.' && (s)[1] == '/' && (s)[2])
#define ISRELPU(s) ((s)[0] == '.' && (s)[1] == '.' && (s)[2] == '/' && (s)[3])
#define ISRELPD(s) ((s)[0] != '/' && (ft_strrchr((s), '/')) != 0)
#define ISRELP(s) (ISRELPU(s) || ISRELPD(s))
#define ISABSP(s) ((s)[0] == '/' && (s)[1])

int			msh_exec_pwd_check(t_shenv *e)
{
	int		ret;

	ret = 0;
	if (!access(*e->cmdv, F_OK) && access(*e->cmdv, X_OK) < 0)
	{
		ft_dprintf(2, "minishell: %s: Permission denied\n", *e->cmdv);
		return (126);
	}
	ret = msh_exec_pwd(e);
	if (WIFEXITED(ret) && SHR8(ret) == 127)
		ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
	return (ret);
}

int			msh_exec(t_shenv *e)
{
	int	st_b;
	int	st_d;
	int	st_p;

	msh_debug_print("exec: start cmdv[0](\"%s\")", e->cmdv[0]);
	st_d = 0;
	st_b = msh_exec_builtin(e);
	if (st_b == 1 || WIFEXITED(st_b))
		return (st_b == 127 ? e->ret : st_b);
	if (ISABSP(*e->cmdv) || ISPWD(*e->cmdv) || ISRELP(*e->cmdv))
	{
		st_d = msh_exec_pwd_check(e);
		if (WIFEXITED(st_d))
			return (st_d);
	}
	st_p = msh_exec_path(e);
	if (CHKEP(st_p) && WIFEXITED(st_p))
		return (st_p);
	else if (st_b == 127 && !st_d && SHR8(st_p) == 255)
		ft_dprintf(2, "minishell: %s: Unknown command\n", *e->cmdv);
	else
		e->signal_recv = 0;
	msh_debug_print("exec: Ret err b(%d) d(%d) p(%d) e->ret(%d)(%d, %d)",
			st_b, st_d, st_p, e->ret, SHR8(e->ret), e->ret & 0xff);
	return (e->signal_recv ? e->ret + 128 : e->ret);
}

#undef CHKEP
#undef ISPWD
#undef ISRELP
#undef ISABSP

/*
** msh_debug_print("exec: builtin st_b(%d, %d) e->ret(%d, %d)",
**  SHR8(st_b), st_b & 0xff,
**  SHR8(e->ret), e->ret & 0xff);
** msh_debug_print("exec: Ret builtin st_b(%d, %d) e->ret(%d, %d)",
**  SHR8(st_b), st_b & 0xff,
**  SHR8(e->ret), e->ret & 0xff);
** msh_debug_print("exec: pwd st_d(%d, %d) e->ret(%d, %d)",
**  SHR8(st_d), st_d & 0xff,
**  SHR8(e->ret), e->ret & 0xff);
** msh_debug_print("exec: Ret pwd st_d(%d, %d) e->ret(%d, %d)",
**  SHR8(st_d), st_d & 0xff,
**  SHR8(e->ret), e->ret & 0xff);
** msh_debug_print("exec: path st_p(%d, %d) e->ret(%d, %d)",
**  SHR8(st_p), st_p & 0xff,
**  SHR8(e->ret), e->ret & 0xff);
** msh_debug_print("exec: Ret path st_p(%d, %d) e->ret(%d, %d)",
**  SHR8(st_p), st_p & 0xff,
**  SHR8(e->ret), e->ret & 0xff);
** msh_debug_print("exec: start cmdv[0](%s)", e->cmdv[0]);
** msh_debug_print("exec: builtin st_b(%d)", st_b);
** msh_debug_print("exec: found /");
** msh_debug_print("exec: pwd st_d(%d)", st_d);
** msh_debug_print("exec: pwd ret st_d(%d)", st_d);
** msh_debug_print("exec: path st_b(%d) st_d(%d) st_p(%d)", st_b, st_d, st_p);
** msh_debug_print("exec: path ret st_p(%d)", st_p);
*/
