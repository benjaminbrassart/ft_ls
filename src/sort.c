/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 14:02:50 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/08 21:05:37 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ls.h"
#include "libft_ext.h"
#include "libft/ft.h"

#include <sys/stat.h>

static int __compare(FileInfo const* f1, FileInfo const* f2, int options, bool dir_last);
static void __quicksort(FileInfo* files, ssize_t low, ssize_t high, int options, bool dir_last);

void ls_sort(FileInfo* files, size_t count, int options, bool dir_last)
{
    __quicksort(files, 0, count - 1, options, dir_last);
}

void __quicksort(FileInfo* files, ssize_t left, ssize_t right, int options, bool dir_last)
{
    if (left >= right)
        return;

    ssize_t i = left;
    ssize_t j = right;

    FileInfo pivot = files[(left + right) / 2];

    while (i <= j)
    {
        while (__compare(&files[i], &pivot, options, dir_last) < 0)
            ++i;
        while (__compare(&files[j], &pivot, options, dir_last) > 0)
            --j;
        if (i <= j)
        {
            FileInfo tmp = files[i];

            files[i] = files[j];
            files[j] = tmp;
            ++i;
            --j;
        }
    }

    __quicksort(files, left, j, options, dir_last);
    __quicksort(files, i, right, options, dir_last);
}

/**
 * Compare two timespec structures
 *
 * @return 1 if T1 > T2, -1 if T1 < T2, 0 otherwise
 */
static int __compare_time(struct timespec const* t1, struct timespec const* t2)
{
    if (t1->tv_sec > t2->tv_sec)
    {
        if (t1->tv_nsec > t2->tv_nsec)
            return 1;
        else if (t1->tv_nsec < t2->tv_nsec)
            return -1;
        return 0;
    }
    else if (t1->tv_sec < t2->tv_sec)
        return -1;
    return 0;
}

static int __compare(FileInfo const* f1, FileInfo const* f2, int options, bool dir_last)
{
    int result;

    if (dir_last)
    {
        if (S_ISDIR(f1->st.st_mode))
        {
            if (!S_ISDIR(f2->st.st_mode))
                return -1;
        }
        else if (S_ISDIR(f2->st.st_mode))
            return 1;
    }

    if (options & LSOPT_SORT_TIME)
        result = __compare_time(&f1->st.st_mtim, &f2->st.st_mtim);
    else
        result = ft_strcasecmp(f1->name, f2->name);

    if (options & LSOPT_REVERSE)
        return -result;

    return result;
}
