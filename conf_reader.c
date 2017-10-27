# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <libgen.h>

# include "conf_reader.h"

static char* trim_left(char* str)
{
    char*       p = str;
    int         index = 0;
    int         length = 0;

    if (!str)
    {
        return NULL;
    }

    length = strlen(str);

    for (index = 0; index < length; index++)
        if (!(' ' == *(p + index) || '\n' == *(p + index) || '\t' == *(p + index)
                || '\r' == *(p + index)))
        {
            break;
        }

    if (index > 0)
    {
        memmove(str, p + index, length - index + 1);    //include '\0'
    }

    return str;
}

static char* trim_right(char* str)
{
    int         index = 0;
    int         length;

    if (!str)
    {
        return NULL;
    }

    length = strlen(str);

    for (index = length - 1; index >= 0; index--)
        if (!(' ' == *(str + index) || '\n' == *(str + index) || '\t' == *(str + index)
                || '\r' == *(str + index)))
        {
            break;
        }

    *(str + index + 1) = '\0';
    return str;
}

static char* trim(char* str)
{
    trim_left(str);
    trim_right(str);
    return str;
}

static PSSECTION __get_section(const PSCONFIG config, const char* section_name)
{
    PLIST_NODE          p = NULL;
    PSSECTION           psection;

    if (!config)
    {
        return NULL;
    }

    if (NULL == section_name || 0 == strlen(section_name))
    {
        return &config->m_global_section;
    }

    p = config->m_sections.first;

    while (p)
    {
        psection = container_of(p, SSECTION, m_list);

        if (0 == strcasecmp(section_name, psection->m_name))
        {
            break;
        }

        p = p->next;
    }

    return (p ? container_of(p, SSECTION, m_list) : NULL);
}

static PSOPTION __get_option(const PSSECTION section, const char* option_name)
{
    PLIST_NODE          node;
    PSOPTION            option;

    if (!section || !option_name)
    {
        return NULL;
    }

    node = section->m_options.first;

    while (node)
    {
        option = container_of(node, SOPTION, m_list);

        if (0 == strcasecmp(option_name, option->m_name))
        {
            break;
        }

        node = node->next;
    }

    return (node ? container_of(node, SOPTION, m_list) : NULL);
}

static PSSECTION get_and_insert_section(PSCONFIG config,
                                        const char* section_name)
{
    PSSECTION           psection = NULL;

    if (!config)
    {
        return NULL;
    }

    if (NULL == (psection = __get_section(config, section_name)))
    {
        if (NULL == (psection = (PSSECTION)malloc (sizeof(*psection))))
        {
            return NULL;
        }

        memset(psection, 0, sizeof(*psection));
        memcpy(psection->m_name, section_name, strlen(section_name));
        INIT_LIST_HEAD(&psection->m_options);
        INIT_LIST_NODE(&psection->m_list);
        list_add_head(&config->m_sections, &psection->m_list);
    }

    return psection;
}

static int add_option_to_section(PSSECTION section, const char* key,
                                 const char* value)
{
    PLIST_NODE          p;
    PSOPTION            option;

    if (!section || !key || !value)
    {
        return -1;
    }

    if (NULL == (option = (PSOPTION) malloc (sizeof(*option))))
    {
        return -1;
    }

    memset(option, '\0', sizeof(*option));
    memcpy(option->m_name, key, strlen(key));
    memcpy(option->m_value, value, strlen(value));
    p = &option->m_list;
    INIT_LIST_NODE(p);
    list_add_head(&section->m_options, p);

    return 0;
}

static const char* get_sub_file_name(const char* parent_file,
                                     const char* inc_file, char* sub_file)
{
    char    temp[SECTION_OPTION_NAME_LENGTH];
    char*   p = NULL;

    if (!parent_file || !inc_file || !sub_file)
    {
        return NULL;
    }

    if ('/' == *inc_file)
    {
        memcpy(sub_file, inc_file, strlen(inc_file));
    }
    else
    {
        memset(temp, 0, sizeof(temp));
        memcpy(temp, parent_file, strlen(parent_file));
        p = dirname(temp);
        memcpy(sub_file, p, strlen(p));

        if ('/' != *(sub_file + strlen(sub_file) - 1))
        {
            strcat(sub_file, "/");
        }

        strcat(sub_file, inc_file);
    }

    return sub_file;
}

int load_config_file(const char* file_name, PSCONFIG config)
{
    char            buffer[SECTION_OPTION_NAME_LENGTH + OPTION_VALUE_LENGTH + 10];
    char*           p;
    PSSECTION       psection = NULL;
    FILE*           file = NULL;

    if (!file_name || !config || 0 != access(file_name, F_OK))
    {
        return -1;
    }

    if (NULL == (file = fopen(file_name, "rb")))
    {
        return -1;
    }

    psection = &config->m_global_section;

    while (1)
    {
        if (NULL == fgets(buffer, sizeof(buffer), file))
        {
            if (feof(file))
            {
                break;
            }
            else
            {
                goto err;
            }
        }

        trim(buffer);

        if ('#' == *buffer)
        {
            if (0 == strncasecmp("include", buffer + 1, strlen("include")) && \
                    (' ' == *(buffer + strlen("include") + 1)
                     || '\t' == *(buffer + strlen("include") + 1)))
            {
                char    sub_file_name[SECTION_OPTION_NAME_LENGTH];

                memset(sub_file_name, 0, sizeof(sub_file_name));
                get_sub_file_name(file_name, trim(buffer + strlen("include") + 2),
                                  sub_file_name);

                if (0 > load_config_file(sub_file_name, config))
                {
                    goto err;
                }
            }
            else
            {
                continue;
            }
        }

        if (NULL != (p = strrchr(buffer, '#')))
        {
            *p = '\0';
            trim(buffer);
        }

        if (0 == strlen(buffer))
        {
            continue;
        }

        if ('[' == *buffer && ']' == *(buffer + strlen(buffer) - 1))
        {
            *(buffer + strlen(buffer) - 1) = '\0';

            if (NULL == (psection = get_and_insert_section(config, trim(buffer + 1))))
            {
                goto err;
            }

            continue;
        }

        if (NULL == (p = strchr(buffer, '=')))
        {
            continue;
        }

        *p = '\0';
        add_option_to_section(psection, trim(buffer), trim(p + 1));
    }

    fclose(file);
    return 0;

err:
    fclose(file);
    free_config_file(config);
    return -1;
}

static void free_options_of_section(PSSECTION section)
{
    PSOPTION    option;
    PLIST_NODE  node;

    if (!section)
    {
        return;
    }

    while ((node = section->m_options.first))
    {
        list_del(node);
        option = container_of(node, SOPTION, m_list);
        free(option);
    }
}

int free_config_file(PSCONFIG config)
{
    PSSECTION   section;
    PLIST_NODE  node;

    if (!config)
    {
        return -1;
    }


    section = &config->m_global_section;
    free_options_of_section(section);

    while ((node = config->m_sections.first))
    {
        section = container_of(node, SSECTION, m_list);
        list_del(node);
        free_options_of_section(section);
        free(section);
    }

    return 0;
}

char* get_str_value(const PSCONFIG config, const char* section_name,
                    const char* option_name, char* def_value)
{
    PSSECTION       psection = NULL;
    PSOPTION        poption = NULL;

    if (!((psection = __get_section(config, section_name)) && \
            (poption = __get_option(psection, option_name))))
    {
        return def_value;
    }
    else
    {
        return poption->m_value;
    }
}

int get_int_value(const PSCONFIG config, const char* section_name,
                  const char* option_name, int def_value)
{
    char*   pvalue = NULL;

    if ((pvalue = get_str_value(config, section_name, option_name, NULL)))
    {
        return atoi(pvalue);
    }
    else
    {
        return def_value;
    }
}

long get_long_value(const PSCONFIG config, const char* section_name,
                    const char* option_name, long def_value)
{
    char*   pvalue = NULL;

    if ((pvalue = get_str_value(config, section_name, option_name, NULL)))
    {
        return strtoll(pvalue, NULL, 10);
    }
    else
    {
        return def_value;
    }
}

double get_double_value(const PSCONFIG config, const char* section_name,
                        const char* option_name, double def_value)
{
    char*   pvalue = NULL;

    if ((pvalue = get_str_value(config, section_name, option_name, NULL)))
    {
        return strtod(pvalue, NULL);
    }
    else
    {
        return def_value;
    }
}

bool get_bool_value(const PSCONFIG config, const char* section_name,
                    const char* option_name, bool def_value)
{
    char*   pvalue = NULL;

    if ((pvalue = get_str_value(config, section_name, option_name, NULL)))
    {
        return  strcasecmp(pvalue, "true") == 0 ||
                strcasecmp(pvalue, "yes") == 0 ||
                strcasecmp(pvalue, "on") == 0 ||
                strcmp(pvalue, "1") == 0;
    }
    else
    {
        return def_value;
    }
}


static void print_options_of_section(const PSSECTION pSection)
{
    PSOPTION        pOption;
    PLIST_NODE      pNode;

    if (!pSection)
    {
        return;
    }

    printf("[ %s ]\n", pSection->m_name);
    pNode = pSection->m_options.first;

    while (pNode)
    {
        pOption = container_of(pNode, SOPTION, m_list);
        printf("\t%s = %s\n", pOption->m_name, pOption->m_value);
        pNode = pNode->next;
    }

    printf("\n");
}

void print_config_values(const PSCONFIG config)
{
    PSSECTION       psection;
    PLIST_NODE      pnode;

    if (!config)
    {
        return;
    }

    psection = &config->m_global_section;
    print_options_of_section(psection);

    pnode = config->m_sections.first;

    while (pnode)
    {
        psection = container_of(pnode, SSECTION, m_list);
        pnode = pnode->next;
        print_options_of_section(psection);
    }
}

