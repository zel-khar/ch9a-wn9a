#include "minishell.h"

static t_malloc *save_malloc(t_malloc *m_list)
{
    static t_malloc *save;

    if (m_list)
        save = m_list;
    return (save);
}

void    ft_cleanup(t_malloc **m_list)
{
    t_malloc    *current;
    t_malloc    *tmp;

    if (m_list)
        current = *m_list;
    else
        current = save_malloc(NULL);
    while (current)
    {
        tmp = current->next;
        free(current->content);
        free(current);
        current = tmp;
    }
    if (m_list)
        *m_list = NULL;
}

static void ft_add_back_malloc(t_malloc **m_list, t_malloc *address)
{
    t_malloc    *current;

    if (*m_list == NULL)
    {
        *m_list = address;
        save_malloc(*m_list);
        return ;
    }
    current = *m_list;
    while (current)
    {
        if (current->next == NULL)
        {
            current->next = address;
            break ;
        }
        current = current->next;
    }
}

void    *ft_malloc(size_t size)
{
    void            *address;
    t_malloc        *new_malloc;
    static t_malloc *m_list;

    address = malloc(size);
    if (address == NULL)
    {
        perror("error :");
        ft_cleanup(&m_list);
        exit(1);
    }
    new_malloc = malloc(sizeof(t_malloc));
    if (new_malloc == NULL)
    {
        perror("error :");
        ft_cleanup(&m_list);
        exit(1);
    }
    new_malloc->content = address;
    new_malloc->next = NULL;
    ft_add_back_malloc(&m_list, new_malloc);
    return (address);
}
