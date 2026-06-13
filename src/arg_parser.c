/*
  raincoat - displays hourly weather forecasts
 
  Copyright (C) 2024 Shadman Saquib Abir 

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "arg_parser.h"

int parse_args(int argc, char *argv[], char *location, char *days)
{
    int opt;
    int flag = 0;

    while (1) {
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"version", no_argument, 0, 'V'},
            {"city", required_argument, 0, 'c'},
            {"days", required_argument, 0, 'd'}
        };

        opt = getopt_long(argc, argv, "hVc:d:", long_options, NULL);

        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'h':
                print_help(argv[0]);
                return 1;

            case 'V':
                print_version();
                return 1;

            case 'c':
                if (flag & 1) {
                    fprintf(stderr, "Usage: %s [-h] [-V] [-c location] [-d days]\n", argv[0]);
                    return 2;
                }

                flag = flag | 1;
                int l_return = parse_location(location, optarg);
                
                if (l_return == 1) {
                    return 2;
                }
                
                if (l_return == 2) {
                    return 3;
                }
                break;

            case 'd':
                if ((flag & 2) >> 1) {
                    fprintf(stderr, "Usage: %s [-h] [-V] [-c location] [-d days]\n", argv[0]);
                    return 2;
                }

                flag = flag | 2;
                int d_return = parse_days(days, optarg);
                
                if (d_return == 1) {
                    return 2;
                }
                break;

            default:
                fprintf(stderr, "Usage: %s [-h] [-V] [-c location] [-d days]\n", argv[0]);
                return 2;
        }
    }

    return 0;
}

int parse_location(char *location, char *optarg)
{
    int length = strlen(optarg);

    if (!(length) || (optarg[0] == '-'))  {
        fprintf(stderr, "\'-c, --city\' option is missing proper argument\n");
        return 1;
    }

    int space_counter = 0;

    for (int i = 0; i < length; i++) {
        if (optarg[i] == ' ') {
         space_counter++;
        }
    }

    // spaces are getting replaced by %20, which is 2 extra bytes per space character
    // 1 extra byte added is for the null terminator
    int size = length + 2*space_counter + 1;

    if (size > 41) {
        fprintf(stderr, "Location name can have a maximum length of 40 characters\n");
        return 1;
    }

    char *temp = (char *) calloc(size, sizeof(char));

    if (temp == NULL) {
        perror("parse_location");
        return 2;
    }

    int temp_index = 0;

    for (int i = 0; i < length; i++) {
        if (optarg[i] == ' ') {
            temp[temp_index] = '%';
            temp[temp_index+1] = '2';
            temp[temp_index+2] = '0';
            temp_index += 3;
        } else {
            temp[temp_index] = optarg[i];
            temp_index++;
        }
    }

    strcpy(location, temp);
    free(temp);

    return 0;
}

int parse_days(char *days, char *optarg) {
    int length = strlen(optarg);

    if (!(length) || (optarg[0] == '-')) {
        fprintf(stderr, "\'-d, --days\' option is missing proper argument\n");
        return 1;
    }

    if (strcmp(optarg, "1") && strcmp(optarg, "2") && strcmp(optarg, "3")) {
        fprintf(stderr, "\'-d, --days\' option takes 1, 2 or 3 as argument\n");
        return 1;
    }

    strcpy(days, optarg);

    return 0;
}

int print_version(void)
{
    printf("raincoat 0.2.1\n");
    printf("Copyright (C) 2024 Shadman Saquib Abir\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://www.gnu.org/licenses/>\n");
    printf("This program is free software: see the source for copying conditions. There\n");
    printf("is NO WARRANTY; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
    printf("Written by Shadman Saquib Abir\n");

    return 0;
}

int print_help(char *self)
{
    printf("Usage: %s [-h] [-V] [-c location] [-d days]\n\n", self);
    printf("raincoat displays hourly weather (temperature, probability of rain, condition)\n");
    printf("forecasts for up to 3 days at the specified location (default is Dhaka). The\n");
    printf("forecast data is collected from <https://www.weatherapi.com>\n\n");
    printf("Options:\n");
    printf("  -V, --version         display version information and exit\n");
    printf("  -h, --help            display this help file and exit\n");
    printf("  -c, --city=location   display forecast data of the city specified by the \'location\'\n");
    printf("                        argument\n");
    printf("  -d, --days=days       display forecast data for the number of days specified by the\n");
    printf("                        \'days\' argument. Valid values are 1, 2 or 3.\n\n");
    printf("*Location*\n");
    printf("  location argument can have a maximum length of 40 characters. It supports any of\n");
    printf("  the following formats:\n");
    printf("  • City name e.g.: Bonn\n");
    printf("  • Use double quotes for names containing whitespaces e.g.: \"Cape Town\"\n");
    printf("  • Latitude and Longitude (Decimal degree) e.g.: 60.1699,24.9384\n");
    printf("  • US zip e.g.: 10001\n");
    printf("  • UK zip e.g.: SW1\n");
    printf("  • Canada postal code e.g.: G2J\n");
    printf("  • metar:<metar code> e.g.: metar:VGZR\n");
    printf("  • iata:<3 digit airport code> e.g.: iata:JFK\n");
    printf("  • auto:ip IP lookup e.g.: auto:ip\n");
    printf("  • IP address (IPv4 and IPv6 supported) e.g.: 100.0.0.1\n\n");
    printf("Examples:\n");
    printf("  raincoat                     display hourly weather forecasts of Dhaka\n");
    printf("  raincoat -c Bonn             display hourly weather forecasts of Bonn\n");
    printf("  raincoat -d 2 --city=Hanoi   display 2 days hourly weather forecasts of Hanoi\n");
    printf("  raincoat -c 60.1699,24.9384  display hourly weather forecasts of Helsinki\n\n");
    printf("Exit code:\n");
    printf("  0     Successful program execution\n");
    printf("  1     Usage or syntax error\n");
    printf("  2     Data transfer and connection related error\n");
    printf("  3     Invalid request or json parsing error\n");
    printf("  4     Memory allocation error\n\n");
    printf("Report bugs to <https://github.com/AmeAkashi/raincoat/issues>\n");

    return 0;
}
