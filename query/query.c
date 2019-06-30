#include <stdio.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>
#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

typedef enum {
    LONG_OPTION_LIST_ATTRIBUTES = 1000,
    LONG_OPTION_QUERY = 1001,
    LONG_OPTION_LOG_LEVEL  = 1002,
    LONG_OPTION_HELP = 1003,
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

#define NO_ID (0xffffffff)

 static struct option long_options[] =
        {
          {"list-attributes", no_argument, 0, LONG_OPTION_LIST_ATTRIBUTES},
          {"query",  required_argument, 0, LONG_OPTION_QUERY},
          {"log-level",  required_argument, 0, LONG_OPTION_LOG_LEVEL},
          {"help", no_argument, 0, LONG_OPTION_HELP},
          {0, 0, 0, 0}
        };

MYSQL mysql;

uint32_t get_table_id(const char* sql)
{
    uint32_t id = NO_ID;

    if (mysql_query(&mysql, sql))
    {
        assert(0);
    }

    MYSQL_RES *result = mysql_store_result(&mysql);

    if (result == NULL)
    {
        assert(0);
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        sscanf(row[0], "%u", &id);
    }

    mysql_free_result(result);

    return id;

}

void connect_database()
{
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"your_prog_name");
    if (!mysql_real_connect(&mysql,"localhost","root","unixrocks1","media_archive",0,NULL,0))
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n",
              mysql_error(&mysql));
    }
   
}

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



typedef enum {
  OPERATOR_EQUALITY,
  OPERATOR_INEQUALITY,
  OPERATOR_FUZZY_MATCH,
  OPERATOR_REGEXP_MATCH,
  OPERATOR_GREATER_THAN,
  OPERATOR_GREATER_THAN_OR_EQUAL_TO,
  OPERATOR_LESSER_THAN,
  OPERATOR_LESSER_THAN_OR_EQUAL_TO,
  OPERATOR_LOGICAL_NOT,
  OPERATOR_LOGICAL_OR,
  OPERATOR_LOGICAL_XOR,
  OPERATOR_NR_ENTRIES,
  OPERATOR_NONE,
} operator_t;

const char* query_operator[OPERATOR_NR_ENTRIES] = {
    "==", "!=", "~=", "*=", ">", ">=", "<", "<=", "!", "||", "^^",
};

operator_t next_operator(char* expr) 
{
    char* match = strstr(expr, query_operator[OPERATOR_EQUALITY]);
    if (match) {
        return OPERATOR_EQUALITY;
    }

    match = strstr(expr, query_operator[OPERATOR_INEQUALITY]);
    if (match) {
        return OPERATOR_INEQUALITY;
    }

    match = strstr(expr, query_operator[OPERATOR_FUZZY_MATCH]);
    if (match) {
        return OPERATOR_FUZZY_MATCH;
    }

    match = strstr(expr, query_operator[OPERATOR_REGEXP_MATCH]);
    if (match) {
        return OPERATOR_REGEXP_MATCH;
    }

    match = strstr(expr, query_operator[OPERATOR_GREATER_THAN_OR_EQUAL_TO]); // before '>'
    if (match) {
        return OPERATOR_GREATER_THAN_OR_EQUAL_TO;
    }

    match = strstr(expr, query_operator[OPERATOR_GREATER_THAN]);
    if (match) {
        return OPERATOR_GREATER_THAN;
    }

    match = strstr(expr, query_operator[OPERATOR_LESSER_THAN_OR_EQUAL_TO]); // before '<'
    if (match) {
        return OPERATOR_LESSER_THAN_OR_EQUAL_TO;
    }

    match = strstr(expr, query_operator[OPERATOR_LESSER_THAN]);
    if (match) {
        return OPERATOR_LESSER_THAN;
    }

    match = strstr(expr, query_operator[OPERATOR_LOGICAL_NOT]);
    if (match) {
        return OPERATOR_LOGICAL_NOT;
    }

    match = strstr(expr, query_operator[OPERATOR_LOGICAL_OR]);
    if (match) {
        return OPERATOR_LOGICAL_OR;
    }

    match = strstr(expr, query_operator[OPERATOR_LOGICAL_XOR]);
    if (match) {
        return OPERATOR_LOGICAL_XOR;
    }

    return OPERATOR_NONE;
}

char* trim_expression(char* str) {
    int len = strlen(str);

    int i;
    int start = -1;
    int end = -1;

    // Determine start of block
    for(i=0; i < len; i++)
     {
         if (str[i] != ' ' && str[i] != '\'') {
             start = i;
             break;
         }
     }

    // Determine end of block
    for(i=len-1; i >= 0; i--)
    {
        if (str[i] != ' ' && str[i] != '\'') {
            end = i;
            break;
        }
    }

     if (start != -1 && end != -1) {
         int count = end - start + 1;
         memmove(str, str + start, count);
         str[end+1-start] = 0;
     }

     return str;
}

char* attribute_from_expr(char* expr, operator_t op)
{
    char* rest = strdup(expr); 
    char* attribute = strtok_r(rest, query_operator[op], &rest);

    return trim_expression(attribute);
}

char* value_from_expr(char* expr, operator_t op)
{
    char* rest = strdup(expr); 
    (void)strtok_r(rest, query_operator[op], &rest);
    char* value = rest + 1;
    return trim_expression(value);
}

static bool
translate_cond(char* user_cond, int user_cond_size, char* sql_cond, int sql_cond_size)
{
    // Design:
    // Based on user expression, build tree structure representing the operators. For
    // each expression, run query and store object results in hash table for later lookup
    // and as object list. Then for compound expressions e.g. A && B, determine new
    // object list based on the logical operator and use it for next step. In the end,
    // a final object list respresent the results of the query

    // First round of implementation: only supporting a single AVP constraint. It is
    // the building block for everything else

    // Assumption: attribute <operator> value (optional single quotes around value)
    // Time.Weekday == Monday

    operator_t op = next_operator(user_cond);
    if (op == OPERATOR_NONE) {
        return false;
    }

    char* attribute = attribute_from_expr(user_cond, op);
    char* value = value_from_expr(user_cond, op);
    if (!attribute || !value) {
        free(attribute);
        free(value);
        return false;
    }

    switch(op) {
        case OPERATOR_EQUALITY:
            snprintf(sql_cond, sql_cond_size, "WHERE (avp.attribute = '%s' and avp.value = '%s')", attribute, value);
            break;
        case OPERATOR_INEQUALITY:
            snprintf(sql_cond, sql_cond_size, "WHERE not (avp.attribute = '%s' and avp.value = '%s')", attribute, value);
            break;
        case OPERATOR_FUZZY_MATCH:
            snprintf(sql_cond, sql_cond_size, "WHERE (avp.attribute = '%s' and avp.value SOUNDS LIKE '%s')", attribute, value);
            break;
        case OPERATOR_REGEXP_MATCH:
            snprintf(sql_cond, sql_cond_size, "WHERE (avp.attribute = '%s' and avp.value REGEXP '%s')", attribute, value);
            break;
        case OPERATOR_GREATER_THAN:
            snprintf(sql_cond, sql_cond_size, "WHERE (avp.attribute = '%s' and avp.value > '%s')", attribute, value);
            break;
        case OPERATOR_GREATER_THAN_OR_EQUAL_TO:
            snprintf(sql_cond, sql_cond_size, "WHERE (avp.attribute = '%s' and avp.value >= '%s')", attribute, value);
            break;
        case OPERATOR_LESSER_THAN:
            snprintf(sql_cond, sql_cond_size, "WHERE (avp.attribute = '%s' and avp.value < '%s')", attribute, value);
            break;
        case OPERATOR_LESSER_THAN_OR_EQUAL_TO:
            snprintf(sql_cond, sql_cond_size, "WHERE (avp.attribute = '%s' and avp.value <= '%s')", attribute, value);
            break;
    }

printf("sql_cond: %s\n", sql_cond);
}

void print_attribute_list()
{
    uint32_t id = NO_ID;

    char sql[1024];
    snprintf(sql, sizeof(sql), "SELECT DISTINCT(attribute) from avp order by attribute ASC;");

    if (mysql_query(&mysql, sql))
    {
        assert(0);
    }

    MYSQL_RES *result = mysql_store_result(&mysql);

    if (result == NULL)
    {
        assert(0);
    }

    printf("Available attributes:\n");

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        char attribute[1024] = {0};
        sscanf(row[0], "%s", (char*)&attribute);
        printf(" - %s\n", attribute);
    }

    mysql_free_result(result);
}

void print_query_results(const char* sql)
{
    uint32_t id = NO_ID;

    if (mysql_query(&mysql, sql))
    {
        assert(0);
    }

    MYSQL_RES *result = mysql_store_result(&mysql);

    if (result == NULL)
    {
        assert(0);
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;
    while ( num_fields == 4 && (row = mysql_fetch_row(result)))
    {
        char sha1[256] = {0};
        char suffix[256] = {0};
        char counter[256] = {0};
        char avp_id[256] = {0};
        sscanf(row[0], "%s", (char*)&sha1);
        sscanf(row[1], "%s", (char*)&suffix);
        sscanf(row[2], "%s", (char*)&counter);
        sscanf(row[3], "%s", (char*)&avp_id);
        printf("%s %-5s %s %-5s %s%-5s %s\n", sha1, " ", suffix, " ", counter, " ", avp_id);
    }

    mysql_free_result(result);
}

int main(int argc, char* argv[])
{
    char query[2048] = {0};
    char user_cond[2048] = {0};
    char sql_cond[2048] = {0};

    while(1) {
        int option_index = 0;

        int c = getopt_long (argc, argv, "hlq:",
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

    connect_database();

    if (opt_list_attributes) {
        print_attribute_list();

    }
    else if (opt_query) {
        // Conditions expressed as e.g. "Time.Weekday == Monday && Time.Year == 2017"
        // are converted into the SQL equivalent conditions, i.e.
        // (avp.attribute = 'Time.Weekday' and avp.value = 'Monday') and (avp.attribute = 'Time.Year' and avp.value = '2017')
        translate_cond(user_cond, sizeof(user_cond), sql_cond, sizeof(sql_cond));

        snprintf(query, sizeof(query), "SELECT object.sha1, suffix.suffix, object.access_counter, tag.avp_id FROM avp INNER JOIN tag on avp.id = tag.avp_id INNER JOIN object on tag.obj_id = object.id INNER JOIN suffix on object.suffix_id = suffix.id %s ORDER BY object.access_counter DESC LIMIT 30;" , sql_cond);

        print_query_results(query);

        printf("%s\n", query);
    }


    // 
    return 0;
}


#if 0
    /*
WHERE (avp.attribute = 'Time.Weekday' and avp.value = 'Monday') and (avp.attribute = 'Time.Year' and avp.value = '2017')
select * from avp where value SOUNDS LIKE 'mmudday';
select * from avp where value REGEXP 'God|Fri' and attribute REGEXP 'itle';
    
    
     */
#endif