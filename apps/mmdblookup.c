#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "MMDB.h"
#include "MMDB_Helper.h"
#include "getopt.h"
/* dummy content */

void usage(char *prg)
{
    fprintf(stderr, "Usage: %s -f database addr\n", prg);
    exit(1);
}

int main(int argc, char *const argv[])
{
    int verbose = 0;
    int character;
    char *fname = NULL;

    while ((character = getopt(argc, argv, "vf:")) != -1) {
        switch (character) {
        case 'v':
            verbose = 1;
            break;
        case 'f':
            fname = strdup(optarg);
            break;
        default:
        case '?':
            usage(argv[0]);
        }
    }
    argc -= optind;
    argv += optind;

    MMDB_s *mmdb =
        MMDB_open(fname ? fname :
                  "/usr/local/share/GeoIP2/reference-database.dat",
                  MMDB_MODE_MEMORY_CACHE);

    char *ipstr = argv[0];
    struct in_addr ip;
    if (ipstr == NULL || 1 != addr_to_num(ipstr, &ip)) {
        fprintf(stderr, "Invalid IP\n");
        exit(1);
    }

    MMDB_root_entry_s root = {.entry.mmdb = mmdb };
    uint32_t ipnum = htonl(ip.s_addr);
    int err = MMDB_lookup_by_ipnum(ipnum, &root);
    if (err == MMDB_SUCCESS) {
        double dlat, dlon;
        char *city, *country, *region_name, code2[3];
        if (root.entry.offset > 0) {
#if defined BROKEN_SEARCHTREE
            // correct offset
            root.entry.offset -= mmdb->segments;
#endif
            MMDB_return_s res_location;
            MMDB_get_value(&root.entry, &res_location, "location", NULL);
            // TODO handle failed search somehow.
            MMDB_return_s lat, lon, field;
            MMDB_entry_s location = {.mmdb = mmdb,.offset = res_location.offset
            };
            if (res_location.offset) {
                MMDB_get_value(&location, &lat, "lat", NULL);
                MMDB_get_value(&location, &lon, "lon", NULL);
                dlat = lat.double_value;
                dlon = lon.double_value;
            } else {
                dlon = dlat = 0;
            }

            //     printf( "%u %f %f\n", ipnum , dlat, dlon);

            MMDB_return_s res;
            MMDB_get_value(&root.entry, &res, "city", "name", "ascii", NULL);
            city = bytesdup(&res);
            MMDB_get_value(&root.entry, &res, "country", "name", "ascii", NULL);
            country = bytesdup(&res);

            printf("%u %f %f %s %s\n", ipnum, dlat, dlon, city, country);

            if (city)
                free(city);
            if (country)
                free(country);

        } else {
            puts("Sorry, nothing found");       // not found
        }
    }

    if (fname)
        free(fname);

    return (0);
}