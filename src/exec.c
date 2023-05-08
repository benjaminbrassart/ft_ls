/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 13:57:26 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/08 21:36:35 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"

#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/dir.h>
#include <sys/stat.h>

static void __print_file(FileInfo const* file, int first, int last, int options);

/*

1. access all files from CTX, print and skip if error
2. sort files according to CTX options, non-directory first
3. print one by one ->
    non-dir:
        print file
    dir:
    3.1 sort files according to CTX options
    3.2 print one by one ->
            print file
            dir:
                add to queue
        for dirs in queue:
            do 3.1

*/

int ls_exec(LsContext* ctx)
{
    int status = EXIT_OK;
    struct stat st;
    FileInfo* directories = malloc(ctx->file_count * sizeof (*directories));
    FileInfo* files = malloc(ctx->file_count * sizeof (*files));
    size_t dir_count = 0;
    size_t file_count = 0;

    if (directories == NULL || files == NULL)
    {
        int err = errno;

        free(directories);
        free(files);

        print_error("%s (%d)", strerror(err), err);
        cli_free(ctx);
        return EXIT_MAJOR;
    }

    for (size_t i = 0; i < ctx->file_count; ++i)
    {
        if (stat(ctx->files[i], &st) == -1)
        {
            int err = errno;

            status = EXIT_MAJOR;
            print_error("cannot stat '%s': %s (%d)", ctx->files[i], strerror(err), err);
            free(ctx->files[i]);
            continue;
        }

        FileInfo *fi = S_ISDIR(st.st_mode)
            ? &directories[dir_count++]
            : &files[file_count++];
        fi->st = st;
        fi->name = ctx->files[i];
    }

    free(ctx->files);

    ls_sort(files, file_count, ctx->options, false);

    for (size_t i = 0; i < file_count; ++i)
    {
        __print_file(&files[i], i == 0, i == file_count - 1, ctx->options);
        free(files[i].name);
    }

    free(files);

    if (dir_count > 0 && file_count > 0)
        write(STDOUT_FILENO, "\n\n", 2);

    ls_sort(directories, dir_count, ctx->options, false);

    for (size_t i = 0; i < dir_count; ++i)
    {
        // if ((dir_count > 1 && i > 0) || ((CHECK_OPT(ctx->options, LSOPT_RECURSIVE) && file_count > 0)))
        if (dir_count > 1 || ctx->file_count > dir_count || CHECK_OPT(ctx->options, LSOPT_RECURSIVE))
        {
            if (i > 0)
                write(STDOUT_FILENO, "\n", 1);
            ft_printf("%s:\n", directories[i].name);
        }

        // TODO print what is inside directories

        free(directories[i].name);
    }

    free(directories);
    return status;
}

#define EXEC_ALT    (1 << 0)
#define EXEC_PERM   (1 << 1)
#define EXEC_DIR    (1 << 2)

/**
 * Get the character that will be used for the execution permission char
 *
 * @param mode the mode of the file, usually from stat(2)
 * @param perm the permission to be checked (S_IXUSR, S_IXGRP or S_IXOTH)
 * @param alt the alternative mode (sticky bit, setuid bit or setgid bit)
 * to be checked (S_ISUID, S_ISGRP or S_ISVTX)
 *
 * @return the right character for the file mode
 *
 * @details
 * Truth table:
 *
 * DEC = decimal reprentation of DIR + PERM + ALT
 * DIR = is the file a directory?
 * PERM = is perm set for the directory mode?
 * ALT = is alt set for the directoy mode?
 * R = the resulting character
 *
 * +-----+-----+------+-----+---+
 * | DEC | DIR | PERM | ALT | R |
 * +-----+-----+------+-----+---+
 * |   0 |   0 |    0 |   0 | - |
 * |   1 |   0 |    0 |   1 | S |
 * |   2 |   0 |    1 |   0 | x |
 * |   3 |   0 |    1 |   1 | s |
 * |   4 |   1 |    0 |   0 | - |
 * |   5 |   1 |    0 |   1 | T |
 * |   6 |   1 |    1 |   0 | x |
 * |   7 |   1 |    1 |   1 | t |
 * +-----+-----+------+-----+---+
 */
static char __exec_char(mode_t mode, int perm, int alt)
{
    unsigned int bits = 0
        | (S_ISDIR(mode) ? EXEC_DIR : 0)
        | ((mode & perm) ? EXEC_PERM : 0)
        | ((mode & alt) ? EXEC_ALT : 0);

    switch (bits)
    {
        case 0:
        case EXEC_DIR:
            return '-';
        case EXEC_PERM:
        case EXEC_DIR | EXEC_PERM:
            return 'x';
        case EXEC_ALT:
            return 'S';
        case EXEC_ALT | EXEC_PERM:
            return 's';
        case EXEC_ALT | EXEC_DIR:
            return 'T';
        case EXEC_ALT | EXEC_PERM | EXEC_DIR:
            return 't';
        default: // should never happen, terminates the program
            ft_dprintf(STDERR_FILENO,
                "Internal error for %s:%d\n"
                "Value of `bits`: %u\n"
                "THIS IS A BUG, please report it!\n",
                __func__, __LINE__, bits
            );
            exit(128 + SIGILL);
    }
}

static char __type_char(mode_t mode)
{
    switch (mode & __S_IFMT)
    {
        case S_IFREG:  return '-';
        case S_IFDIR:  return 'd';
        case S_IFCHR:  return 'c';
        case S_IFBLK:  return 'b';
        case S_IFIFO:  return 'p';
        case S_IFLNK:  return 'l';
        case S_IFSOCK: return 's';
        default:       return '?';
    }
}

#define __perm_char(Mode, Perm, Char) ((Mode & Perm) ? Char : '-')

static void __print_file(FileInfo const* file, int first, int last, int options)
{
    if (CHECK_OPT(options, LSOPT_LONG))
    {
        struct stat const* st = &file->st;

        char mode[] = {
            __type_char(st->st_mode),
            __perm_char(st->st_mode, S_IRUSR, 'r'),
            __perm_char(st->st_mode, S_IWUSR, 'w'),
            __exec_char(st->st_mode, S_IXUSR, S_ISUID),
            __perm_char(st->st_mode, S_IRGRP, 'r'),
            __perm_char(st->st_mode, S_IWGRP, 'w'),
            __exec_char(st->st_mode, S_IXGRP, S_ISGID),
            __perm_char(st->st_mode, S_IROTH, 'r'),
            __perm_char(st->st_mode, S_IWOTH, 'w'),
            __exec_char(st->st_mode, S_IXOTH, S_ISVTX),
            '\0',
        };
        struct passwd* usr = getpwuid(st->st_uid);
        struct group* grp = getgrgid(st->st_gid);

        char user_buff[LOGIN_NAME_MAX + 1];
        char group_buff[LOGIN_NAME_MAX + 1];

        if (usr == NULL)
            ft_snprintf(user_buff, sizeof (user_buff), "%u", st->st_uid);
        else
            ft_strlcpy(user_buff, usr->pw_name, sizeof (user_buff));

        if (grp == NULL)
            ft_snprintf(group_buff, sizeof (group_buff), "%u", st->st_uid);
        else
            ft_strlcpy(group_buff, grp->gr_name, sizeof (group_buff));

        char* mtime = ctime(&st->st_mtim.tv_sec) + 4;
        mtime[12] = '\0';

        // TODO align every column
        // TODO add "-> <target>" if file is a link
        ft_printf("%s %u %s %s %u %s %s\n", mode, st->st_nlink, user_buff, group_buff, st->st_size, mtime, file->name);
    }
    else
        ft_printf(first ? "%s " : last ? " %s" : " %s ", file->name);
}
