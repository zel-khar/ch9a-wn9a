
#include "libft.h"

char    *ft_strdup(const char *s)
{
    char    *dup;
    size_t  len;

    len = strlen(s) + 1;
    dup = ft_malloc(len);
    if (!dup)
        return (NULL);
    memcpy(dup, s, len);
    return (dup);
}