#include <stdio.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>
#include <ctype.h>
#include <libgen.h>

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
    uint32_t id = 0;

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

uint32_t add_object(const char* sha1)
{
char sql[1024];

    uint32_t obj_id = 0;

    // Object table
    snprintf(sql, sizeof(sql),
             "INSERT INTO object (sha1) VALUES ('%s');",
             sha1 ? sha1 : "");
    printf("%s\n", sql);
    mysql_query(&mysql, sql);

    snprintf(sql, sizeof(sql), "select obj_id from object where sha1='%s';", sha1 ? sha1 : "");
    printf("sql %s\n", sql);

    obj_id = get_table_id(sql);

    return obj_id;
}

uint32_t add_avp(const char* attribute, const char* value)
{
    char sql[1024];

    uint32_t avp_id = 0;

    // AVP table
    snprintf(sql, sizeof(sql),
             "INSERT INTO avp (attribute, value) VALUES ('%s','%s');",
             attribute ? attribute : "", value ? value : "");
   mysql_query(&mysql, sql);
   printf("%s\n", sql);

   // 

    snprintf(sql, sizeof(sql), "select avp_id from avp where attribute = '%s' and value = '%s';", 
        attribute ? attribute : "", value ? value : "");
    printf("sql %s\n", sql);

    avp_id = get_table_id(sql);

    return avp_id;
}

void add_tag(uint32_t obj_id, uint32_t avp_id)
{
    char sql[1024];

    snprintf(sql, sizeof(sql), "INSERT INTO tag (obj_id, avp_id) VALUES (%llu, %llu);",
        obj_id, avp_id);
    printf("%s\n", sql);
    mysql_query(&mysql, sql);

}




int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Please specify <path to .info>\n");
    }

    char path[1024];
    snprintf(path, sizeof(path), "%s", argv[1]);
    char* baseName = basename(path);
    char* pos = strstr(path, ".info");

    if (!pos) {
        printf("No info file given!\n");
        return 0;
    }
    *pos = 0;

    connect_database();

    const char* sha1 = baseName;
    const char* infoPath = argv[1];

    printf("reading %s\n", infoPath);

    char line[2048];
    char suffexes[1024];
    FILE* infoFile = fopen(infoPath, "r");
    uint32_t obj_id = 0;

    obj_id = add_object(sha1);

    if (infoFile) {
        while (fgets(line, sizeof(line), infoFile)) {
            int len = strlen(line);
            line[len-1] = 0;

            char* rest = line; 
            char* attribute = strtok_r(rest, " ", &rest);
            char* value = strtok_r(rest, " ", &rest);

            uint32_t avp_id = add_avp(attribute, value);
            add_tag(obj_id, avp_id);
       }
       fclose(infoFile);
    }

    return 0;
}
