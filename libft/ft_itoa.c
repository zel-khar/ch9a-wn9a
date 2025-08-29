/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>      +#+  +:+       +#+      */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/08/24 00:00:00 by your_login       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t   int_len(int n)
{
    size_t  l;
    long    v;

    l = 1;
    v = n;
    if (v < 0)
        l++, v = -v;
    while (v >= 10)
        l++, v /= 10;
    return (l);
}

char    *ft_itoa(int n)
{
    long    v;
    size_t  len;
    char    *s;

    v = n;
    len = int_len(n);
    s = ft_malloc(len + 1);
    if (!s)
        return (NULL);
    s[len] = '\0';
    if (v < 0)
    {
        s[0] = '-';
        v = -v;
    }
    while (len && s[len - 1] != '-')
    {
        s[len - 1] = (char)('0' + (v % 10));
        v /= 10;
        len--;
    }
    return (s);
}
