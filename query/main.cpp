
#include <stdio.h>
#include <string.h>
#include <my_global.h>
#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "object.hpp"
#include "query.hpp"
#include "window.hpp"
#include <list>
#include <unordered_map>


typedef enum {
    LONG_OPTION_LIST_ATTRIBUTES = 1000,
    LONG_OPTION_QUERY = 1001,
    LONG_OPTION_GUI = 1002,
    LONG_OPTION_LOG_LEVEL  = 1003,
    LONG_OPTION_HELP = 1004,
} long_option_t;

typedef enum {
    LOG_LEVEL_EMERG = 0,
    LOG_LEVEL_ALERT = 1,
    LOG_LEVEL_CRITICAL = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_WARNING = 4,
    LOG_LEVEL_NOTICE = 5,
    LOG_LEVEL_INFO = 6,
    LOG_LEVEL_DEBUG = 7,
} log_level_t;

static int opt_log_level = LOG_LEVEL_NOTICE;
static bool opt_list_attributes = false;
static bool opt_query = false;
static bool opt_gui = false;

 static struct option long_options[] =
        {
          {"list-attributes", no_argument, 0, LONG_OPTION_LIST_ATTRIBUTES},
          {"query",  required_argument, 0, LONG_OPTION_QUERY},
          {"gui", no_argument, 0, LONG_OPTION_GUI},
          {"log-level",  required_argument, 0, LONG_OPTION_LOG_LEVEL},
          {"help", no_argument, 0, LONG_OPTION_HELP},
          {0, 0, 0, 0}
        };

void print_help()
{
    printf("Usage: mquery [OPTIONS]\n");
    printf("\n");

    printf("Based on the given logical expression, relevant entries\n");
    printf("from the database are extracted. This is done by stating\n");
    printf("attributes themselves or the full attribute-value pairs\n");
    printf("using the query language operators\n");
    printf("\n");
    printf("Example:\n");
    printf("Query: 'Image && Time.Weekday == Monday && Time.Year == 2017'\n");
    printf("\n");
    printf("The query language supports traditional operators but also expands\n");
    printf("the expressive power with additional ones\n");
    printf("\n");
    printf("~~~~~~~~~~\n");
    printf("Operators:\n");
    printf("~~~~~~~~~~\n");
    printf(" '=='  : Equality (exact match)\n");
    printf(" '!='  : Inequality (exact match)\n");
    printf(" '~='  : Fuzzy match (Soundex phonetic similarity)\n");
    printf(" '*='  : Regexp match (MySQL supported scope)\n");
    printf(" '>'   : Greater than\n");
    printf(" '>='  : Greater than or equal to\n");
    printf(" '<'   : Lesser than\n");
    printf(" '<='  : Lesser than or equal to\n");
    printf(" '!'   : Logical NOT\n");
    printf(" '||'  : Logical OR\n");
    printf(" '^^'  : Logical XOR\n");
    printf("\n");    
    printf("Options:\n");
    printf(" -l  --list-attributes   List available sttributes\n");
    printf(" -q --query=STRING       Query\n");
    printf("    --log-level=INTEGER  Log level\n");
    printf(" -h --help               This screen\n");    
}

int main(int argc, char* argv[])
{
    char user_cond[2048] = {0};

    std::list<object> obj_list;
    std::unordered_map<uint32_t, object> obj_map;

    while(1) {
        int option_index = 0;

        int c = getopt_long (argc, argv, "ghlq:",
                           long_options, &option_index);

        // No more options to parse
        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
            case LONG_OPTION_HELP:
                print_help();
                return 0;
                break;
            case 'l':
            case LONG_OPTION_LIST_ATTRIBUTES:
                opt_list_attributes = true;
                break;
            case 'q':
            case LONG_OPTION_QUERY:
                opt_query = true;
                snprintf(user_cond, sizeof(user_cond), "%s", optarg);
                break;
            case 'g':
            case LONG_OPTION_GUI:
                opt_gui = true;
                break;
            case LONG_OPTION_LOG_LEVEL:
                opt_log_level = atoi(optarg);
                if (opt_log_level < 0 || opt_log_level > LOG_LEVEL_DEBUG) {
                    printf("Error: invalid log level!\n");
                    return 1;
                }
                printf("Log level %d\n", opt_log_level);
                break;
        }
    }

    query_connect_database();

    if (opt_list_attributes) {
        query_print_attribute_list();

    }
    else if (opt_query) {
        query_execute(std::string(user_cond), obj_list, obj_map);

        if (opt_gui) {
            window w = window();
            w.show();   
        } else {
            for (std::list<object>::iterator it=obj_list.begin(); it != obj_list.end(); ++it) {
                object_type_t object_type = (*it).get_object_type();
                if (object_type == OBJECT_TYPE_AUDIO) {
                    std::cout << "obj_id: " << (*it).get_obj_id() << " --> " << (*it).get_audio_path() << std::endl;
                } else {
                    std::cout << "obj_id: " << (*it).get_obj_id() << " --> " << (*it).get_thumbnail_path(THUMBNAIL_SIZE_320, THUMBNAIL_QUALITY_HIGH) << std::endl;
                }
                
            }
        }
    }

    return 0;
}
