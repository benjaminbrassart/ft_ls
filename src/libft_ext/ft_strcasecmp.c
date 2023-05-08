/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcasecmp.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 21:03:26 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/08 21:05:02 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft_ext.h"
#include "libft/ft.h"

int ft_strcasecmp(char const* s1, char const* s2)
{
    while (*s1 != '\0' && ft_toupper(*s1) == ft_toupper(*s2))
    {
        ++s1;
        ++s2;
    }
    return ft_toupper(*s1) - ft_toupper(*s2);
}
