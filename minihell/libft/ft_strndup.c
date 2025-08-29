#include "libft.h"


char	*ft_strndup(const char *s, size_t n)
{
	char *dup;
	size_t len;

	len = 0;
	while (len < n && s[len])
		len++;
	dup = ft_malloc(len + 1);
	if (!dup)
		return (NULL);
	memcpy(dup, s, len);
	dup[len] = '\0';
	return (dup);
}
