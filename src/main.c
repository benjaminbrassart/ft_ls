/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 09:59:26 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/08 13:10:59 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

#include <stdlib.h>
#include <unistd.h>

static void __usage(void);

int main(int argc, char const* argv[])
{
    LsContext ctx;

    cli_init(&ctx);

    int result = cli_parse(argc, argv, &ctx);

    if (result != EXIT_OK)
        return EXIT_MAJOR;

    if (CHECK_OPT(ctx.options, LSOPT_HELP))
    {
        cli_free(&ctx);
        __usage();
        return EXIT_OK;
    }

    cli_free(&ctx);
    return EXIT_OK;
}

static char const USAGE_MESSAGE[] =
    "Usage: ft_ls [OPTIONS]... [FILE]...\n"
    "List information about the FILEs (the current directory by default).\n"
    "Sort entries alphabetically if not specified.\n"
    "\n"
    "Options:\n"
    "  -a               do not ignore entries starting with .\n"
    "  -l               use a long listing format\n"
    "  -r               reverse order while sorting\n"
    "  -R               list subdirectories recursively\n"
    "  -t               sort by time, newest first\n"
    "      --help       display this help and exit\n"
    "\n"
    "Exit status:\n"
    "  0  if OK,\n"
    "  1  if minor problems (e.g. cannot access subdirectory),\n"
    "  2  if serious trouble (e.g., cannot access command-line argument).\n"
;

static void __usage(void)
{
    write(STDOUT_FILENO, USAGE_MESSAGE, sizeof (USAGE_MESSAGE) - 1);
}
