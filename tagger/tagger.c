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
    LONG_OPTION_OBJECT     = 1000,
    LONG_OPTION_ATTRIBUTE  = 1001,
    LONG_OPTION_VALUE      = 1002,
    LONG_OPTION_ADD_TAG    = 1003,
    LONG_OPTION_REMOVE_TAG = 1004,
    LONG_OPTION_LOG_LEVEL  = 1005,
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
static int opt_help = 0;
static int opt_add_tag = 0;
static int opt_remove_tag = 0;

static char* opt_object = NULL;
static char* opt_attribute = NULL;
static char* opt_value = NULL;

static void store_tag(uint32_t obj_id, uint32_t avp_id);
uint32_t store_avp(char* attribute, char* value);

#define NO_ID (0xffffffff)

 static struct option long_options[] =
        {
          {"object",     required_argument, 0,               LONG_OPTION_OBJECT},
          {"attribute",  required_argument, 0,               LONG_OPTION_ATTRIBUTE},
          {"value",      required_argument, 0,               LONG_OPTION_VALUE},
          {"add-tag",    no_argument,       &opt_add_tag,    LONG_OPTION_ADD_TAG},
          {"remove-tag", no_argument,       &opt_remove_tag, LONG_OPTION_REMOVE_TAG},
          {"log-level",  required_argument, 0,               LONG_OPTION_LOG_LEVEL},
          {0, 0, 0, 0}
        };

MYSQL mysql;

char* to_lower(char* str) {
    int i;
    for(i=0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }

    return str;
}

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

uint32_t find_object(const char* sha1) {
    char sql[1024];

    snprintf(sql, sizeof(sql), "SELECT obj_id FROM object WHERE sha1=\"%s\";", sha1 ? sha1 : "");
    printf("%s\n", sql);

    uint32_t obj_id = get_table_id(sql);

    return obj_id;
}

uint32_t add_object(const char* sha1)
{
char sql[1024];

    uint32_t obj_id = NO_ID;

    // Object table
    snprintf(sql, sizeof(sql),
             "INSERT INTO object (sha1,access_counter) VALUES (\"%s\", 0);",
             sha1 ? sha1 : "");
    printf("%s\n", sql);
    mysql_query(&mysql, sql);

    return find_object(sha1);
}

uint32_t find_avp(const char* attribute, const char* value)
{
      char sql[1024];

    uint32_t avp_id = NO_ID;  

        snprintf(sql, sizeof(sql), "SELECT avp_id FROM avp WHERE attribute = \"%s\" AND value = \"%s\";", 
        attribute ? attribute : "", value ? value : "");
    printf("%s\n", sql);

    avp_id = get_table_id(sql);

    return avp_id;
}

uint32_t find_tag(const char* sha1, const char* attribute, const char* value)
{
    char sql[1024];

    uint32_t tag_id = NO_ID;  

    uint32_t object_id = find_object( sha1);

    uint32_t avp_id = find_avp(attribute, value);

    snprintf(sql, sizeof(sql), "SELECT tag_id FROM tag WHERE obj_id = \"%u\" AND avp_id = \"%u\";", 
        object_id, avp_id);
    printf("%s\n", sql);

    tag_id = get_table_id(sql);

    return tag_id;
}

void remove_tag(uint32_t tag_id)
{
   char sql[1024];

   snprintf(sql, sizeof(sql), "DELETE FROM tag WHERE tag_id = %u;", tag_id);
   printf("%s\n", sql);
   mysql_query(&mysql, sql);
}

uint32_t add_avp(const char* attribute, const char* value)
{
    char sql[1024];

    uint32_t avp_id = NO_ID;

    // AVP table
    snprintf(sql, sizeof(sql),
             "INSERT INTO avp (attribute, value) VALUES (\"%s\",\"%s\");",
             attribute ? attribute : "", value ? value : "");
   mysql_query(&mysql, sql);
   printf("%s\n", sql);

   return find_avp(attribute, value);
}

void add_tag(uint32_t obj_id, uint32_t avp_id)
{
    char sql[1024];

    snprintf(sql, sizeof(sql), "INSERT INTO tag (obj_id, avp_id) VALUES (%u, %u);",
        obj_id, avp_id);
    printf("%s\n", sql);
    mysql_query(&mysql, sql);

}

uint32_t store_avp(char* attribute, char* value) {
    uint32_t avp_id;

    // To allow query of available attributes, store AVP with empty value
    if (value) {
        (void)add_avp(attribute, "");
    }

    // Store requested AVP
    avp_id = add_avp(attribute, value);

    return avp_id;
}

static void store_tag(uint32_t obj_id, uint32_t avp_id)
{
    add_tag(obj_id, avp_id);
}

void print_help()
{
    printf("Usage: [OPTIONS] [ .info file paths]\n");
    printf("\n");

    printf("Options:\n");
    printf("    --object=STRING      SHA1 sum identifying media object\n");
    printf("    --attribute=STRING   Attribute string\n");
    printf("    --value=STRING       Value string\n");
    printf("    --add-tag            Add tag based on given object and AVP. If not present, AVP will be created\n");
    printf("    --remove-tag         Remove tag based on given object and AVP. Only tag affected (not AVP itself)\n");
    printf("    --log-level=INTEGER  Log level\n");
    printf(" -h --help               This screen\n");    
}

int main(int argc, char* argv[])
{
    while(1) {
        int option_index = 0;

        int c = getopt_long (argc, argv, "h",
                           long_options, &option_index);

        printf("opt c %d\n", c);

        // No more options to parse
        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help();
                return 0;
                break;
            case LONG_OPTION_OBJECT:
                opt_object = optarg;
                break;
            case LONG_OPTION_ATTRIBUTE:
                opt_attribute = optarg;
                break;
            case LONG_OPTION_VALUE:
                opt_value = optarg;
                break;
            case LONG_OPTION_ADD_TAG:
                // opt_add_tag has been set
                break;
            case LONG_OPTION_REMOVE_TAG:
                // opt_remove_tag has been set
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

    if (opt_add_tag && opt_object && opt_attribute) 
    {
        uint32_t object_id = find_object(opt_object);
        if (object_id == NO_ID) {
            printf("Add tag: object id '%s' doesn't exist!\n", opt_object);
            return 1;
        }

        // Ensure AVP is stored
        uint32_t avp_id = store_avp(opt_attribute, opt_value);

        // Add tag or no action if already present
        store_tag(object_id, avp_id);

        return 0;
    }
    else if (opt_remove_tag && opt_object && opt_attribute && opt_value)
    {
        // Only object tag is removed. The AVP remains since it may be used for other objects

        uint32_t object_id = find_object(opt_object);
        if (object_id == NO_ID) {
            printf("Remove tag: object id '%s' doesn't exist!\n", opt_object);
            return 1;
        }

        uint32_t avp_id = find_avp(opt_attribute, opt_value);
        if (avp_id == NO_ID) {
            printf("Remove tag: AVP '%s : %s' doesn't exist!\n", opt_attribute, opt_value);
            return 1;
        }

        uint32_t tag_id = find_tag(opt_object, opt_attribute, opt_value);
        if (tag_id == NO_ID) {
            printf("Remove tag: Tag doesn't exist for object %s and AVP '%s : %s'\n", opt_object, opt_attribute, opt_value);
            return 1;
        }

        remove_tag(tag_id);
        return 0;
    }


    int i;
    for (i=optind; i < argc; i++) {
        char path[1024];
        snprintf(path, sizeof(path), "%s", argv[i]);
        char* baseName = basename(path);
        char* pos = strstr(path, ".info");

        if (!pos) {
            continue;
        }
        *pos = 0;

        const char* sha1 = baseName;
        const char* infoPath = argv[i];

        printf("Processing %s...\n", infoPath);

        char line[2048];
        char suffexes[1024];
        FILE* infoFile = fopen(infoPath, "r");
        uint32_t obj_id = 0;

        obj_id = add_object(sha1);

        if (infoFile) {
        while (fgets(line, sizeof(line), infoFile)) {
            int len = strlen(line);
            line[len-1] = 0;

            char tokStr[2048];
            snprintf(tokStr, sizeof(tokStr), "%s", line);

            char* rest = tokStr; 
            char* attribute = strtok_r(rest, " ", &rest);
            char* value = NULL;
            if (attribute) {
                printf("line len %d attribute len %d\n", len-1, (int)strlen(attribute));
                value = line + strlen(attribute) + 1;
            }

            uint32_t avp_id = store_avp(attribute, value);
            store_tag(obj_id, avp_id);
        }
        fclose(infoFile);
        }
    }

    return 0;
}
