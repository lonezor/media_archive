
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
#include <curl/curl.h>


typedef enum {
    LONG_OPTION_LIST_ATTRIBUTES = 1000,
    LONG_OPTION_PLAY,
    LONG_OPTION_ACCOUNT_TOKEN,
    LONG_OPTION_QUERY,
    LONG_OPTION_GUI,
    LONG_OPTION_LOG_LEVEL,
    LONG_OPTION_HELP,
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
static bool opt_play = false;
static bool opt_account_token = false;
static bool opt_gui = false;

 static struct option long_options[] =
        {
          {"list-attributes", no_argument, 0, LONG_OPTION_LIST_ATTRIBUTES},
          {"play",  required_argument, 0, LONG_OPTION_PLAY},
          {"account-token",  required_argument, 0, LONG_OPTION_ACCOUNT_TOKEN},
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
    printf(" -l  --list-attributes       List available attributes\n");
    printf(" -p --play=STRING            Play media based on search results (FFmpeg)\n");
    printf(" -a  --account-token=STRING   Backblaze account authorization token\n");
    printf(" -q --query=STRING           Query\n");
    printf("    --log-level=INTEGER      Log level\n");
    printf(" -h --help                   This screen\n");    
}

 CURL *curl = NULL;


std::string curl_resp = "";

size_t curl_write_cb(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t bytes = nmemb* size;
    char* str = (char*)calloc(1, bytes+1);
    memcpy(str, ptr, bytes);
    curl_resp += std::string(str);
    free(str);

    return bytes;
}

std::string authorize_object_url(std::string sha1, std::string account_token)
{
  CURL *curl;
  CURLcode res;
 
  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */ 
    curl_easy_setopt(curl, CURLOPT_URL, "https://api002.backblazeb2.com/b2api/v1/b2_get_download_authorization");

    struct curl_slist *list = NULL;
    char account_authorization[1024];
    snprintf(account_authorization, sizeof(account_authorization), "Authorization: %s", account_token.c_str());
    list = curl_slist_append(list, account_authorization);
    list = curl_slist_append(list, "Content-Type: text/plain");
 
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    /* Now specify the POST data */ 

    char json[1024];
    snprintf(json, sizeof(json), "{\"bucketId\": \"0c7a9cbff0c615066ec50619\", \"fileNamePrefix\": \"%s\", \"validDurationInSeconds\": 604800}",
                                 sha1.c_str());

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);

 
    /* Perform the request, res will get the return code */ 
    curl_resp = "";
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }

    curl_slist_free_all(list); /* free the list again */
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    printf("curl_resp: '%s'\n", curl_resp.c_str());

    size_t pos = curl_resp.find("authorizationToken");
    if (pos != std::string::npos) {
        pos += strlen("authorizationToken\": \"");
        size_t end = curl_resp.find_first_of ('\"', pos);
        return curl_resp.substr(pos, end - pos);
    }
    else {
        return "";
    }
  }
}

std::string get_object_url(std::string sha1, std::string auth_token)
{
    char url[2048];
    snprintf(url, sizeof(url), "%s%s?Authorization=%s",
    "https://f002.backblazeb2.com/file/audio-objects/",
    sha1.c_str(),
    auth_token.c_str());

    return std::string(url);
}

void play_object(object& obj, std::string account_token)
{
    std::cout << "Play " << obj.audio_metadata.title << std::endl;

    // Authorize
    std::string auth_token = authorize_object_url(obj.get_sha1(), account_token);

    // Get URL
    std::string url = get_object_url(obj.get_sha1(), auth_token);

    // Play with ffmpeg with cache setting
    char cmd[4096];
    snprintf(cmd, sizeof(cmd), "mplayer -quiet -cache 1048576 %s", url.c_str());
    system(cmd);

    query_increment_access_counter(obj.get_obj_id());
}

int main(int argc, char* argv[])
{
    char user_cond[2048] = {0};
    char account_token[2048] = {0};

    std::list<object> obj_list;
    std::unordered_map<uint32_t, object> obj_map;

    while(1) {
        int option_index = 0;

        int c = getopt_long (argc, argv, "a:ghlp:q:",
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
            case 'p':
            case LONG_OPTION_PLAY:
                opt_play = true;
                snprintf(user_cond, sizeof(user_cond), "%s", optarg);
                break;
            case 'a':
            case LONG_OPTION_ACCOUNT_TOKEN:
                snprintf(account_token, sizeof(account_token), "%s", optarg);
                opt_account_token = true;
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
    else if (opt_play) {
        query_execute(std::string(user_cond), obj_list, obj_map);

        for (std::list<object>::iterator it=obj_list.begin(); it != obj_list.end(); ++it) {
                object_type_t object_type = (*it).get_object_type();
                if (object_type == OBJECT_TYPE_AUDIO) {
                   play_object(*it, std::string(account_token));
                }
        }

    }
    else if (opt_query) {
        query_execute(std::string(user_cond), obj_list, obj_map);

        if (opt_gui) {
      //      window w = window(std::string(user_cond));
        //    w.show();   
        } else {
            for (std::list<object>::iterator it=obj_list.begin(); it != obj_list.end(); ++it) {
                object_type_t object_type = (*it).get_object_type();
                if (object_type == OBJECT_TYPE_AUDIO) {
                    std::cout << "obj_id: " << (*it).get_obj_id();

                    if ((*it).audio_metadata.artist.size()) {
                        std::cout << ", artist: " << (*it).audio_metadata.artist;
                    }

                    if ((*it).audio_metadata.album.size()) {
                        std::cout << ", album: " << (*it).audio_metadata.album;
                    }

                    if ((*it).audio_metadata.title.size()) {
                        std::cout << ", title: " << (*it).audio_metadata.title;
                    }

                    if ((*it).audio_metadata.duration.size()) {
                        std::cout << ", duration: " << (*it).audio_metadata.duration;
                    }

                    if ((*it).audio_metadata.genre.size()) {
                        std::cout << ", genre: " << (*it).audio_metadata.genre;
                    }

                    if ((*it).audio_metadata.producer.size()) {
                        std::cout << ", producer: " << (*it).audio_metadata.producer;
                    }

                    if ((*it).audio_metadata.total_discs) {
                        std::cout << ", total_discs: " << std::to_string((*it).audio_metadata.total_discs);
                    }

                    if ((*it).audio_metadata.total_tracks) {
                        std::cout << ", total_tracks: " << std::to_string((*it).audio_metadata.total_tracks);
                    }

                    if ((*it).audio_metadata.disc) {
                        std::cout << ", disc: " << std::to_string((*it).audio_metadata.disc);
                    }

                    if ((*it).audio_metadata.track) {
                        std::cout << ", track: " << std::to_string((*it).audio_metadata.track);
                    }

                    if ((*it).audio_metadata.total_timestamps) {
                        std::cout << ", total_timestamps: " << std::to_string((*it).audio_metadata.total_timestamps);
                    }
                    std::cout << std::endl;
                } else {
                    std::cout << "obj_id: " << (*it).get_obj_id() << " --> " << (*it).get_thumbnail_path(THUMBNAIL_SIZE_320, THUMBNAIL_QUALITY_HIGH) << std::endl;
                }
            }
        }
    }

    return 0;
}
