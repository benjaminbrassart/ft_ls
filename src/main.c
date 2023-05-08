/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 09:59:26 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/08 11:01:35 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

#include <stdlib.h>

static void __usage(void);

int main(int argc, char const* argv[])
{
    LsContext ctx;

    cli_init(&ctx);

    int result = cli_parse(argc, argv, &ctx);

    if (result == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (CHECK_OPT(ctx.options, LSOPT_HELP))
    {
        cli_free(&ctx);
        __usage();
        return EXIT_SUCCESS;
    }

    cli_free(&ctx);
    return EXIT_SUCCESS;
}

static void __usage(void)
{
}
