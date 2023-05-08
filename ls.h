/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ls.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 10:20:24 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/08 13:59:23 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LS_H
# define LS_H

# ifdef __cplusplus
extern "C" {
# endif

# include "libft/ft.h"
# include <unistd.h>

// returned when something was successful
# define EXIT_OK 0
// returned when something minor went wrong
# define EXIT_MINOR 1
// returned when something major went wrong (file access, argument parsing)
# define EXIT_MAJOR 2

/**
 * Print a formatted error
 */
# define print_error(Fmt, ...) (ft_dprintf(STDERR_FILENO, "ft_ls: " Fmt "\n", ##__VA_ARGS__))

# define LSOPT_INIT         0
// -l option
# define LSOPT_LONG         (1 << 0)
// -R option
# define LSOPT_RECURSIVE    (1 << 1)
// -a option
# define LSOPT_ALL          (1 << 2)
// -r option
# define LSOPT_REVERSE      (1 << 3)
// -t option
# define LSOPT_SORT_TIME    (1 << 4)
// --help option
# define LSOPT_HELP         (1 << 5)

/**
 * Check if an option is present in a bitfield
 */
# define CHECK_OPT(Bits, Option) ((Bits & Option) == Option)

typedef unsigned char BitField;

/**
 * The execution context of the ls command
 *
 * @param files the files requested by the command
 * @param file_count the number of files in the files array
 * @param options the option bits
 */
typedef struct ls_context
{
    char** files;
    size_t file_count;
    BitField options;
} LsContext;

/**
 * Initialize a context with default data
 *
 * @param ctx the context structure to initialize
 */
void cli_init(LsContext* ctx);

/**
 * Free a context's allocated resources
 *
 * @param ctx the context to free
 */
void cli_free(LsContext* ctx);

/**
 * Parse command line arguments into a context structure
 *
 * @param argc the number of arguments, usually from main(argc, argv)
 * @param argv the actual arguments, usually from main(argc, argv)
 * @param ctx the destination context structure
 *
 * @return EXIT_OK or EXIT_MAJOR
 */
int cli_parse(int argc, char const* argv[], LsContext* ctx);

/**
 * Execute the ls command core
 *
 * @param ctx the execution context
 *
 * @return EXIT_OK, EXIT_MINOR or EXIT_MAJOR
 */
int ls_exec(LsContext* ctx);

# ifdef __cplusplus
}
# endif

#endif // LS_H
