/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>      +#+  +:+       +#+      */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/08/24 00:00:00 by your_login       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(char const *s, char c)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		if (s[i])
			count++;
		while (s[i] && s[i] != c)
			i++;
	}
	return (count);
}

static char	*get_word(char const *s, char c, int *i)
{
	int		len;
	int		start;
	char	*word;

	len = 0;
	start = *i;
	while (s[*i] && s[*i] != c)
	{
		len++;
		(*i)++;
	}
	word = (char *)ft_malloc(len + 1);
	if (!word)
		return (NULL);
	len = 0;
	while (start < *i)
		word[len++] = s[start++];
	word[len] = '\0';
	return (word);
}

char	**ft_split(char const *s, char c)
{
	char	**split;
	int		words;
	int		i;
	int		j;

	if (!s)
		return (NULL);
	words = count_words(s, c);
	split = (char **)ft_malloc(sizeof(char *) * (words + 1));
	if (!split)
		return (NULL);
	i = 0;
	j = 0;
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		if (s[i])
			split[j++] = get_word(s, c, &i);
	}
	split[j] = NULL;
	return (split);
}
