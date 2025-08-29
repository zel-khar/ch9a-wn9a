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

static size_t   digits_len(long v)
{
    size_t  l;

    l = 1;
    if (v < 0)
        v = -v;
    while (v >= 10)
        l++, v /= 10;
    return (l);
}

char    *ft_itoa(int n)
{
    long    v;
    size_t  nd;
    size_t  total;
    int     neg;
    char    *s;

    v = n;
    neg = (v < 0);
    nd = digits_len(v);
    total = nd + (neg ? 1 : 0);
    s = ft_malloc(total + 1);
    if (!s)
        return (NULL);
    s[total] = '\0';
    if (neg)
        v = -v, s[0] = '-';
    while (nd-- > 0)
    {
        s[total - 1] = (char)('0' + (v % 10));
        v /= 10;
        total--;
    }
    return (s);
}
