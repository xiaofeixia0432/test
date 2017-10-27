# ifndef __CONF_READER_H__
# define __CONF_READER_H__

# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include "list.h"

# define SECTION_OPTION_NAME_LENGTH     128
# define OPTION_VALUE_LENGTH            256

typedef int     bool;

#ifndef true
    #define true 1
#endif

#ifndef false
    #define false 0
#endif

typedef struct _Soption
{
    char        m_name[SECTION_OPTION_NAME_LENGTH + 1];
    char        m_value[OPTION_VALUE_LENGTH + 1];
    LIST_NODE   m_list;
} SOPTION, *PSOPTION;

typedef struct _Ssection
{
    char        m_name[SECTION_OPTION_NAME_LENGTH + 1];
    LIST_NODE   m_list;
    LIST_HEAD   m_options;
} SSECTION, *PSSECTION;

typedef struct _Sconfig
{
    SSECTION    m_global_section;
    LIST_HEAD   m_sections;
} SCONFIG, *PSCONFIG;

# define SCONFIG_INIT {.m_global_section = {.m_name  = "\0", .m_list = {.next = NULL, .pprev = NULL}, .m_options = LIST_HEAD_INIT}, .m_sections = LIST_HEAD_INIT}
# define SCONIFG(config) SCONFIG config = SCONFIG_INIT

static inline void INIT_SCONFIG(PSCONFIG config)
{
    if (!config)
    {
        return;
    }

    memset(config, '\0', sizeof(*config));
    config->m_sections.first = NULL;
    config->m_global_section.m_options.first = NULL;
    INIT_LIST_NODE(&config->m_global_section.m_list);
}

/*
para:
file_name: the configure file name which will be loaded into memery.
config: out parameter

fun: loading the configure file into memery.

return: return true for success, else return false
*/
extern int load_config_file(const char* file_name, PSCONFIG config);

/*
fun: free the SCONFIG struct, and free the configures in memery.
*/
extern int free_config_file(PSCONFIG config);

/*
para:
config:
secton_name: the section name to find, NULL or "" is ok, that's mean to find the option from the global section
option_name: the option name under the section which you want to get
def_value: the default value to return if there's no data in the configures.

fun:
get datas from configures
*/
extern char* get_str_value(const PSCONFIG config, const char* section_name,
                           const char* option_name, char* def_value);
extern int  get_int_value(const PSCONFIG config, const char* section_name,
                          const char* option_name, int def_value);
extern long get_long_value(const PSCONFIG config, const char* section_name,
                           const char* option_name, long def_value);
extern double get_double_value(const PSCONFIG config, const char* section_name,
                               const char* option_name, double def_value);
extern bool get_bool_value(const PSCONFIG config, const char* section_name,
                           const char* option_name, bool def_value);

/*
fun: print the memery, this is just for testing
*/
extern void print_config_values(const PSCONFIG config);


# endif

