#include <stdio.h>
#include <string.h>
#include <mysql.h>
#include <assert.h>
#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "object.hpp"
#include "window.hpp"
#include <list>
#include <unordered_map>



#define NO_ID (0xffffffff)



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

void query_connect_database()
{
    mysql_init(&mysql);
    mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"your_prog_name");
    if (!mysql_real_connect(&mysql,"localhost","root","unixrocks1","media_archive",0,NULL,0))
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n",
              mysql_error(&mysql));
    }
   
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

    // Only attribute
    if (op == OPERATOR_NONE) {
        snprintf(sql_cond, sql_cond_size, "WHERE (avp.attribute = '%s')", user_cond);
        return true;
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

  return true;
}

void query_print_attribute_list()
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

char* get_object_avp_value(uint32_t obj_id, const char* attribute)
{
    char sql[1024];

    snprintf(sql, sizeof(sql), "SELECT avp.value FROM avp INNER JOIN tag ON avp.id = tag.avp_id WHERE tag.obj_id = %d and avp.attribute = \"%s\";",
    (int)obj_id, attribute);

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

    char avp_value[512] = {0};

    while ((row = mysql_fetch_row(result)))
    {
        sscanf(row[0], "%s", avp_value);
        snprintf(avp_value, sizeof(avp_value), row[0], strlen(row[0]));
    }

    mysql_free_result(result);

    return strdup(avp_value);
} 

void zero_integer_string(char** str) {
    free(*str);
    *str = strdup("0");
}

void retrieve_audio_metadata(object& obj) {
    char* artist = get_object_avp_value(obj.get_obj_id(), "Audio.Artist");
    char* album = get_object_avp_value(obj.get_obj_id(), "Audio.Album");
    char* title = get_object_avp_value(obj.get_obj_id(), "Audio.Title");
    char* duration = get_object_avp_value(obj.get_obj_id(), "Audio.Duration");
    char* genre = get_object_avp_value(obj.get_obj_id(), "Audio.Genre");
    char* producer = get_object_avp_value(obj.get_obj_id(), "Audio.Producer");
    char* total_discs = get_object_avp_value(obj.get_obj_id(), "Audio.TotalDiscs");
    char* total_tracks = get_object_avp_value(obj.get_obj_id(), "Audio.TotalTracks");
    char* disc = get_object_avp_value(obj.get_obj_id(), "Audio.Disc");
    char* track = get_object_avp_value(obj.get_obj_id(), "Audio.Track");
    char* total_timestamps = get_object_avp_value(obj.get_obj_id(), "Audio.TotalTimestamps");

    if (!strlen(total_discs)) {
        zero_integer_string(&total_discs);
    }

    if (!strlen(total_tracks)) {
        zero_integer_string(&total_tracks);
    }

    if (!strlen(disc)) {
        zero_integer_string(&disc);
    }

    if (!strlen(track)) {
        zero_integer_string(&track);
    }

    if (!strlen(total_timestamps)) {
        zero_integer_string(&total_timestamps);
    }

    obj.set_audio_metadata(std::string(artist),
                           std::string(album),
                           std::string(title),
                           std::string(duration),
                           std::string(genre),
                           std::string(producer),
                           atoi(total_discs),
                           atoi(total_tracks),
                           atoi(disc),
                           atoi(track),
                           atoi(total_timestamps));
    free(artist);
    free(album);
    free(title);
    free(duration);
    free(genre);
    free(total_discs);
    free(total_tracks);
    free(disc);
    free(track);
    free(total_timestamps);
}

void get_object_results(const char* sql, std::list<object>& obj_list, std::unordered_map<uint32_t, object>& obj_map)
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
        char suffixes[256] = {0};
        unsigned int access_counter = 0;
        unsigned int obj_id = 0;
        sscanf(row[0], "%u", &obj_id);
        sscanf(row[1], "%s", (char*)&sha1);
        sscanf(row[2], "%s", (char*)&suffixes);
        sscanf(row[3], "%u", &access_counter);

        object o = object(obj_id, std::string(sha1), std::string(suffixes), access_counter);

        if (o.get_object_type() == OBJECT_TYPE_AUDIO) {
            retrieve_audio_metadata(o);
        }

        obj_list.push_back(o);
        obj_map[obj_id] = o;
    }

    mysql_free_result(result);
}

void query_execute(std::string user_query, std::list<object>& obj_list, std::unordered_map<uint32_t, object>& obj_map)
{
        char user_cond[2048] = {0};
        char sql_cond[2048] = {0};
        char sql_query[2048] = {0};

        snprintf(user_cond, sizeof(user_cond), "%s", user_query.c_str());

        // Conditions expressed as e.g. "Time.Weekday == Monday && Time.Year == 2017"
        // are converted into the SQL equivalent conditions, i.e.
        // (avp.attribute = 'Time.Weekday' and avp.value = 'Monday') and (avp.attribute = 'Time.Year' and avp.value = '2017')
        translate_cond(user_cond, sizeof(user_cond), sql_cond, sizeof(sql_cond));

        snprintf(sql_query, sizeof(sql_query), "SELECT object.id, object.sha1, suffix.suffix, object.access_counter FROM avp INNER JOIN tag on avp.id = tag.avp_id INNER JOIN object on tag.obj_id = object.id INNER JOIN suffix on object.suffix_id = suffix.id %s ORDER BY object.access_counter DESC LIMIT 10000;" , sql_cond);
        printf("%s\n", sql_query);

        // For now we have enough to start using the system. Long term we need to store object info in hash table and
        // for fast retrieval when operator tree has been processed and the final objects are determined.
        // For now, we can generate resource URL to the data so that the data can be presented...
        // So, next step is the GTK application... or command line application

        get_object_results(sql_query, obj_list, obj_map);
}

void query_increment_access_counter(uint32_t obj_id)
{
   char sql[1024];
    snprintf(sql, sizeof(sql), "UPDATE object SET access_counter=access_counter+1 WHERE id=%u;", obj_id);
    printf("%s\n", sql);
    mysql_query(&mysql, sql);
}




