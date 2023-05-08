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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void cli_init(LsContext* ctx)
{
    ctx->files = NULL;
    ctx->file_count = 0;
    ctx->options = LSOPT_INIT;
}

void cli_free(LsContext* ctx)
{
    for (size_t i = 0; i < ctx->file_count; ++i)
        free(ctx->files[i]);
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
    bool keep_opts = true;

    for (int arg_idx = 0; arg_idx < argc; ++arg_idx)
    {
        if (keep_opts && argv[arg_idx][0] == '-')
        {
            for (int char_idx = 1; argv[arg_idx][char_idx] != '\0'; ++char_idx)
            {
                switch (argv[arg_idx][char_idx])
                {
                    case '-':
                        if (char_idx != 1)
                        {
                            print_error(
                                "invalid option -- '-'\n"
                                "Try 'ft_ls --help' for more information."
                            );
                            goto _error;
                        }

                        if (argv[arg_idx][char_idx + 1] == '\0') // arg == "--"
                        {
                            keep_opts = false;
                            break;
                        }

                        ++char_idx;

                        if (ft_strcmp(argv[arg_idx] + 2, "help") == 0)
                        {
                            ctx->options |= LSOPT_HELP;
                            goto _loop_end;
                        }

                        print_error(
                            "invalid option -- '%s'\n"
                            "Try 'ft_ls --help' for more information.",
                            argv[arg_idx]
                        );
                        goto _error;
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
        else
            files[file_idx++] = ft_strdup(argv[arg_idx]);
_loop_end:
    }

    if (file_idx == 0)
        files[file_idx++] = ft_strdup(".");

    ctx->files = files;
    ctx->file_count = file_idx;
    return EXIT_SUCCESS;

_error:
    free(files);
    return EXIT_FAILURE;
}
