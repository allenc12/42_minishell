/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: callen <callen@student.42.us.org>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/01 12:06:31 by callen            #+#    #+#             */
/*   Updated: 2019/06/03 17:32:56 by callen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			check_token_sub(const char *tok)
{
	const char	*p = tok;

	msh_debug_print("check_token_sub: start tok(\"%s\")", tok);
	if (!p || *p == 0)
		return (0);
	while (*p)
	{
		if (*p == '~')
		{
			if (*(p + 1) && *(p + 1) == '~')
				break ;
			return (1);
		}
		if (*p == '$' && *(p + 1))
		{
			return (1);
		}
		p++;
	}
	msh_debug_print("check_token_sub: end ret0 tok(\"%s\")", tok);
	return (0);
}

char		**msh_tokenize(char *str)
{
	char	**tokens;
	int		i;

	if (!(tokens = quote_strsplit(str, ' ')))
		msh_panic("Memory allocation error in msh_tokenize");
	i = -1;
	while (tokens[++i])
	{
		g_shenv->expand_var = 1;
		while (g_shenv->expand_var && check_token_sub(tokens[i]))
			tokens[i] = msh_expand(tokens[i]);
	}
	return (tokens);
}

void		msh_parse(char **inpt)
{
	char			**tkns;
	register int	i;

	msh_debug_print("parse: start e->ret(%d)(%d, %d)",
			g_shenv->ret, SHR8(g_shenv->ret), g_shenv->ret & 0xff);
	i = -1;
	while (inpt && inpt[++i])
	{
		tkns = msh_tokenize(inpt[i]);
		if (tkns && *tkns)
		{
			g_shenv->cmdv = tkns;
			g_shenv->cmdc = strvec_len(tkns);
			msh_debug_print("parse: exec start e->ret(%d)(%d, %d)",
					g_shenv->ret, SHR8(g_shenv->ret), g_shenv->ret & 0xff);
			g_shenv->ret = msh_exec(g_shenv);
			msh_debug_print("parse: exec end e->ret(%d)(%d, %d)",
					g_shenv->ret, SHR8(g_shenv->ret), g_shenv->ret & 0xff);
			strvec_dispose(tkns);
		}
		else
			free(tkns);
	}
	strvec_dispose(inpt);
}

/*
** XXX: quote_strsplit instead to avoid ``echo "s ; echo st"'' undesired output
*/

int			msh_repl(void)
{
	char	*ln;
	char	*dink;
	char	**boy;

	while (1)
	{
		if (g_shenv->exit_called)
			break ;
		ln = msh_readline();
		if (ln && !(*ln))
		{
			free(ln);
			continue ;
		}
		dink = ft_strtrim(ln);
		boy = ft_strsplit(dink, ';');
		msh_parse(boy);
		free(ln);
		free(dink);
		g_shenv->prompt_printed = 0;
	}
	return (g_shenv->ret);
}
