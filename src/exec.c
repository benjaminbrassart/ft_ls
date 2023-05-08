/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 13:57:26 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/08 14:09:49 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int ls_exec(LsContext* ctx)
{
    int status = EXIT_OK;
    struct stat st;

    // TODO sort file names here

    for (size_t i = 0; i < ctx->file_count; ++i)
    {
        if (stat(ctx->files[i], &st) == -1)
        {
            int err = errno;

            status = EXIT_MAJOR;
            print_error("cannot stat '%s': %s (%d)", ctx->files[i], strerror(err), err);
            continue;
        }
    }

    return status;
}
