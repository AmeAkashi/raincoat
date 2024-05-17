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
#include <string.h>
#include "argsparse.h"

int args_parse(int argc, char *argv[], char *location, char *days)
{
    int loc_flag = 0;
    int days_flag = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-V") || !strcmp(argv[i],"--version")) {
            print_version();
            return 1;
        }
        if (!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")) {
            print_help(argv[0]);
            return 1;
        }
        if (!strcmp(argv[i],"-1") || !strcmp(argv[i],"-2") || !strcmp(argv[i],"-3")) {
            if (!days_flag) {
                days[0] = argv[i][1];
                days[1] = '\0';
                days_flag = 1;
            }
        } else if (!loc_flag) {
            int size = strlen(argv[i]);
            int spacecounter = 0;

            for (int j = 0; j < size; j++) {
                if (argv[i][j] == ' ') {
                    spacecounter++;
                }
            }

            char *temp = (char *) calloc(size + (3*spacecounter + 1), sizeof(char));

            if (temp == NULL) {
                perror("Error while allocating memory");
                return 3;
            }

            int index = 0;

            for (int j = 0; j < size; j++) {
                if (argv[i][j] == ' ') {
                    temp[index] = '%';
                    temp[index+1] = '2';
                    temp[index+2] = '0';
                    index += 3;
                } else {
                    temp[index] = argv[i][j];
                    index++;
                }
            }

            if (strlen(temp) > 40) {
                fprintf(stderr, "Location name can have a maximum length of 40 characters\n");
                free(temp);
                return 2;
            }
            strcpy(location, temp);
            free(temp);
            loc_flag = 1;
        }
    }
    
    return 0;
}

int print_version(void)
{
    printf("raincoat 0.1.1\n\n");
    printf("Copyright (C) 2024 Shadman Saquib Abir\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://www.gnu.org/licenses/>\n");
    printf("This program is free software: see the source for copying conditions. There\n");
    printf("is NO WARRANTY; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
    printf("Written by Shadman Saquib Abir\n");
    return 0;
}

int print_help(char *self)
{
    printf("Usage: %s [OPTION] [LOCATION]\n\n", self);
    printf("raincoat displays hourly weather (temperature, probability of rain, condition)\n");
    printf("forecasts for up to 3 days at the specified LOCATION (default is Dhaka). The\n");
    printf("forecast data is collected from <https://www.weatherapi.com>\n\n");
    printf("Options:\n");
    printf("  -1                    display forecast data for the current date\n");
    printf("  -2                    display forecast data for the current date and the next date\n");
    printf("  -3                    display forecast data for 3 days\n");
    printf("  -V, --version         display version information and exit\n");
    printf("  -h, --help            display this help file and exit\n\n");
    printf("*Location*\n");
    printf("  LOCATION parameter can have a maximum length of 40 characters. It supports any of\n");
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
    printf("  raincoat                  display hourly weather forecasts of Dhaka\n");
    printf("  raincoat Bonn             display hourly weather forecasts of Bonn\n");
    printf("  raincoat -2 Hanoi         display 2 days hourly weather forecasts of Hanoi\n");
    printf("  raincoat 60.1699,24.9384  display hourly weather forecasts of Helsinki\n\n");
    printf("Exit code:\n");
    printf("  0     Successful program execution\n");
    printf("  1     Usage or syntax error\n");
    printf("  2     Data transfer and connection related error\n");
    printf("  3     Invalid request or json parsing error\n");
    printf("  4     Memory allocation error\n\n");
    printf("Report bugs to <https://github.com/AmeAkashi/raincoat/issues>\n");

    return 0;
}
