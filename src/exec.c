/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 13:57:26 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/12 07:31:26 by bbrassar         ###   ########.fr       */
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

static int int_max(int a, int b)
{
    return a > b ? a : b;
}

static int int_length(int n)
{
    int len = 1;

    while (n >= 10)
    {
        n /= 10;
        ++len;
    }
    return len;
}

static void __update_file_data(FileInfo* file, FileInfoAlign* align, int options);
static void __print_file(FileInfo const* file, FileInfoAlign const* align, int first, int last, int options);
static int __print_dir(FileInfo* file, char const* parent, bool print_name, int options);
static void __remove_trailing_slashes(char* path);

static char __exec_char(mode_t mode, mode_t perm, mode_t alt, char c)
{
    if (mode & perm)
        return mode & alt ? c : 'x';
    else
        return mode & alt ? ft_toupper(c) : '-';
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
        ft_memcpy(&fi->st, &st, sizeof (fi->st));
        fi->name = ctx->files[i];
        fi->dot_dotdot = false;
    }

    free(ctx->files);

    ls_sort(files, file_count, ctx->options, false);

    FileInfoAlign algn;

    ft_memset(&algn, 0, sizeof (algn));

    for (size_t i = 0; i < file_count; ++i)
        __update_file_data(&files[i], &algn, ctx->options);

    for (size_t i = 0; i < file_count; ++i)
        __print_file(&files[i], &algn, i == 0, i == file_count - 1, ctx->options);

    free(files);

    if (dir_count > 0 && file_count > 0)
        write(STDOUT_FILENO, "\n\n", 2);

    ls_sort(directories, dir_count, ctx->options, false);

    bool print_name;

    for (size_t i = 0; i < dir_count; ++i)
    {
        if (i > 0)
            write(STDOUT_FILENO, "\n", 1);

        print_name = (dir_count > 1 || ctx->file_count > dir_count || CHECK_OPT(ctx->options, LSOPT_RECURSIVE));

        __print_dir(&directories[i], NULL, print_name, ctx->options);
        free(directories[i].name);
    }

    free(directories);
    return status;
}

static void __update_file_data(FileInfo* file, FileInfoAlign* align, int options)
{
    file->display_name_len = ft_strlcpy(file->display_name, file->name, sizeof (file->display_name));

    if (!CHECK_OPT(options, LSOPT_LONG))
    {
        free(file->name);
        return;
    }

    struct stat const* st = &file->st;

    if (S_ISLNK(st->st_mode))
    {
        file->display_name_len = ft_strlcat(file->display_name, " -> ", sizeof (file->display_name));

        ssize_t readlink_res = readlink(file->name, file->display_name + file->display_name_len, sizeof (file->display_name) - file->display_name_len);

        if (readlink_res == -1)
            file->display_name_len = ft_strlcat(file->display_name, "<readlink error>", sizeof (file->display_name));
        else
        {
            file->display_name_len += readlink_res;
            file->display_name[file->display_name_len] = '\0';
        }
    }

    free(file->name);

    struct passwd const* usr = getpwuid(st->st_uid);

    if (usr == NULL)
        file->user_len = ft_snprintf(file->user, sizeof (file->user), "%u", st->st_uid);
    else
        file->user_len = ft_strlcat(file->user, usr->pw_name, sizeof (file->user));

    struct group const* grp = getgrgid(st->st_gid);

    if (grp == NULL)
        file->group_len = ft_snprintf(file->group, sizeof (file->group), "%u", st->st_gid);
    else
        file->group_len = ft_strlcat(file->group, grp->gr_name, sizeof (file->group));

    align->user = int_max(align->user, file->user_len);
    align->group = int_max(align->group, file->group_len);
    align->links = int_max(align->links, int_length(st->st_nlink));
    align->size = int_max(align->size, int_length(st->st_size));
}

static void __print_file(FileInfo const* file, FileInfoAlign const* align, int first, int last, int options)
{
    if (CHECK_OPT(options, LSOPT_LONG))
    {
        struct stat const* st = &file->st;
        char const mode[] = {
            __type_char(st->st_mode),
            __perm_char(st->st_mode, S_IRUSR, 'r'),
            __perm_char(st->st_mode, S_IWUSR, 'w'),
            __exec_char(st->st_mode, S_IXUSR, S_ISUID, 's'),
            __perm_char(st->st_mode, S_IRGRP, 'r'),
            __perm_char(st->st_mode, S_IWGRP, 'w'),
            __exec_char(st->st_mode, S_IXGRP, S_ISGID, 's'),
            __perm_char(st->st_mode, S_IROTH, 'r'),
            __perm_char(st->st_mode, S_IWOTH, 'w'),
            __exec_char(st->st_mode, S_IXOTH, S_ISVTX, 't'),
        };

        char const* mtime = ctime(&st->st_mtim.tv_sec);

        ft_printf(
            "%.10s %*lu %*s %*s %*ld %.12s %s\n",
            mode,
            align->links, st->st_nlink,
            align->user, file->user,
            align->group, file->group,
            align->size, st->st_size,
            mtime + 4,
            file->display_name
        );
    }
    else
    {
        if (first)
            ft_printf("%s ", file->display_name);
        else if (last)
            ft_printf(" %s", file->display_name);
        else
            ft_printf(" %s ", file->display_name);
    }
}

static void __remove_trailing_slashes(char* path)
{
    size_t len = ft_strlen(path);

    while (len > 0 && path[--len] == '/')
        path[len] = '\0';
}

static int __print_dir(FileInfo* file, char const* parent, bool print_name, int options)
{
    size_t name_len = ft_strlen(file->name);

    if (parent != NULL)
    {
        size_t parent_len = ft_strlen(parent);
        size_t new_name_len = (name_len + parent_len + 2);
        char* new_name = malloc(sizeof (*new_name) * new_name_len);

        if (new_name == NULL)
            goto _malloc_error;

        ft_strlcpy(new_name, parent, new_name_len);
        ft_strlcat(new_name, "/", new_name_len);
        ft_strlcat(new_name, file->name, new_name_len);
        free(file->name);
        file->name = new_name;
    }

    DIR* dir = opendir(file->name);

    if (dir == NULL)
    {
        int err = errno;

        print_error("cannot open directory '%s': %s (%d)", file->name, strerror(err), err);
        return EXIT_MAJOR;
    }

    size_t files_capacity = 128;
    size_t file_count = 0;
    FileInfo* files = malloc(sizeof (*files) * files_capacity);

    if (files == NULL)
    {
        closedir(dir);
        goto _malloc_error;
    }

    if (print_name)
        ft_printf("%s:\n", file->name);

    struct dirent* entry;
    int result = EXIT_OK;

    // Reset errno.
    // Readdir will return NULL at the end without changing errno.
    // If an error occurs, readdir will also return NULL but will set errno to
    // The code of the error.
    errno = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.' && !CHECK_OPT(options, LSOPT_ALL))
            continue;

        if (file_count >= files_capacity)
        {
            FileInfo* new_files = malloc(sizeof (*files) * (files_capacity * 2));

            if (new_files == NULL)
            {
                closedir(dir);
                goto _malloc_error;
            }

            ft_memcpy(new_files, files, sizeof (*files) * file_count);
            free(files);
            files = new_files;
        }

        files[file_count].dot_dotdot = entry->d_name[0] == '.' && ((entry->d_name[1] == '.' && entry->d_name[2] == '\0') || entry->d_name[1] == '\0');

        size_t file_name_len = sizeof (entry->d_name) + ft_strlen(file->name) + 1;
        char* file_name = malloc(sizeof (*file_name) * (file_name_len + 1));

        if (file_name == NULL)
        {
            closedir(dir);
            goto _malloc_error;
        }

        *file_name = '\0';

        if (parent != NULL)
        {
            ft_strlcat(file_name, parent, file_name_len);
            ft_strlcat(file_name, "/", file_name_len);
        }
        ft_strlcat(file_name, entry->d_name, file_name_len);

        files[file_count].name = file_name;

        if (lstat(file_name, &files[file_count].st) == -1)
        {
            int err = errno;

            print_error("cannot stat HERE '%s': %s (%d)", file_name, strerror(err), err);
            result = EXIT_MAJOR;
        }

        ++file_count;
    }

    closedir(dir);

    ls_sort(files, file_count, options, false);

    FileInfoAlign algn;

    ft_memset(&algn, 0, sizeof (algn));

    for (size_t i = 0; i < file_count; ++i)
        __update_file_data(&files[i], &algn, options);

    for (size_t i = 0; i < file_count; ++i)
        __print_file(&files[i], &algn, i == 0, i == file_count - 1, options);

    if (CHECK_OPT(options, LSOPT_RECURSIVE))
    {
        int recurs_result;

        __remove_trailing_slashes(file->name);

        for (size_t i = 0; i < file_count; ++i)
        {
            if (!files[i].dot_dotdot && S_ISDIR(files[i].st.st_mode))
            {
                recurs_result = __print_dir(&files[i], parent == NULL ? "." : file->name, print_name, options);
                if (recurs_result > result)
                    result = recurs_result;
            }
        }
    }

    return result;

_malloc_error:
{
    int err = errno;

    print_error("%s (%d)", strerror(err), err);
}

    return EXIT_MAJOR;
}
