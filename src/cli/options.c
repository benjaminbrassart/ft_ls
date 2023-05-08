/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctx.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 10:19:19 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/08 10:29:25 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"
#include "libft/ft.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void cli_init(LsContext* ctx)
{
    ctx->files = NULL;
    ctx->options = LSOPT_INIT;
}

void cli_free(LsContext* ctx)
{
    free(ctx->files);
}

int cli_parse(int argc, char const* argv[], LsContext* ctx)
{
    char** files = malloc(sizeof (*files) * argc);

    if (files == NULL)
    {
        int err = errno;

        print_error("%s (%d)", strerror(err), err);
        return EXIT_FAILURE;
    }

    int file_idx = 0;

    for (int arg_idx = 1; arg_idx < argc; ++arg_idx)
    {
        if (argv[arg_idx][0] == '-')
        {
            for (int char_idx = 1; argv[arg_idx][char_idx] != '\0'; ++char_idx)
            {
                switch (argv[arg_idx][char_idx])
                {
                    case 'l':
                        ctx->options |= LSOPT_LONG;
                        break;
                    case 'R':
                        ctx->options |= LSOPT_RECURSIVE;
                        break;
                    case 'a':
                        ctx->options |= LSOPT_ALL;
                        break;
                    case 'r':
                        ctx->options |= LSOPT_REVERSE;
                        break;
                    case 't':
                        ctx->options |= LSOPT_SORT_TIME;
                        break;
                    default:
                        print_error(
                            "invalid option -- '%c'\n"
                            "Try 'ft_ls --help' for more information.",
                            argv[arg_idx][char_idx]
                        );
                        goto _error;
                }
            }
        }
    }

    files[file_idx] = NULL;
    ctx->files = files;
    return EXIT_SUCCESS;

_error:
    free(files);
    return EXIT_FAILURE;
}
