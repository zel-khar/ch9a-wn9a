#include "libft.h"

char	*ft_strjoin(char const *a, char const *b)
{
	size_t la;
	size_t lb;
	char *res;
	size_t i;
	la = ft_strlen(a);
	lb = ft_strlen(b);
	res = ft_malloc(la + lb + 1);
	if (!res)
		return (NULL);
	i = 0;
	while (i < la)
	{
		res[i] = a[i];
		i++;
	}
	size_t j = 0;
	while (j < lb)
	{
		res[i + j] = b[j];
		j++;
	}
	res[la + lb] = '\0';
	return (res);
}