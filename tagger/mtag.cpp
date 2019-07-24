

void printHelp()
{
    printf("Options:\n");

    printf("This tool allows addition and removal of media object tags\n");
    printf("The tag is can be a string or key value pair. Also, they can\n");
    printf("have hierarchical structure. They are always written in lower case.\n");

    printf("\n");
    printf("Examples:\n");

artist.name
artist.album

time.year (2017)
time.month 8
time.day 22
time.weekday Tuesday
time.date (2017-08-22)
time.epoch

orientation (Landscape, Portrait)
size (nr of bytes)
environment (city, countryside, forest, sea)

location.galaxy (Milkyway) - relevant if uploading NASA images etc
location.planet (Earth)
location.continent (Asia, Africa, North America, South America, Antarctica, Europe, Australia)
location.ocean (Arctic, Atlantic, Indian, Pacific, Southern)
location.country
location.region
location.city
location.district
location.street
location.house

To allow query of commonly re-used tags, some avp entries in the db use attribute string together with empty string. This allows
presentation of these attributes as an indication of what can be used. These avps are also used to tag broadly, e.g. 'image'


    printf(" - loc")
    printf(" - willam\n");
    printf(" - willam.birthday\n");
    printf(" - willam.age=4 (default years)\n");
    printf(" - willam.baby_month=2.\n");

    printf(" -s --sha1=SUM      SHA1 identifier of media object\n");
    printf(" -a --add-tag=TAG   Add tag association for media object\n");
    printf(" -d --delete-tag=TAG   Remove tag association for media object\n");
    printf(" -f --tag-file=PATH    Path to meta data file containing tags\n");
    printf(" -t --timestamp=EPOCH  Generate timestamp info based on epoch and exit\n");
    


}