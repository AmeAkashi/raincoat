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
#include "cjson/cJSON.h"
#include "dataparse.h"

int print_data(char *jsondata) {
    const cJSON *location = NULL;
    const cJSON *name = NULL;
    const cJSON *country = NULL;
    const cJSON *tzone = NULL;
    const cJSON *localtime_epoch = NULL;
    const cJSON *current = NULL;
    const cJSON *temperature = NULL;
    const cJSON *condition = NULL;
    const cJSON *text = NULL;
    const cJSON *forecast = NULL;
    const cJSON *forecastday = NULL;
    const cJSON *error = NULL;
    const cJSON *message = NULL;
    const cJSON *item = NULL;

    cJSON *data = cJSON_Parse(jsondata);

    if(data == NULL) {
        fprintf(stderr, "print_data: Error while parsing the json data\n");
        cJSON_Delete(data);
        return 1;
    }

    error = cJSON_GetObjectItemCaseSensitive(data, "error");

    if(error != NULL) {
        message = cJSON_GetObjectItemCaseSensitive(error, "message");
        fprintf(stderr, "%s\n", message->valuestring);
        cJSON_Delete(data);
        return 1;
    }

    location = cJSON_GetObjectItemCaseSensitive(data, "location");
    name = cJSON_GetObjectItemCaseSensitive(location, "name");
    country = cJSON_GetObjectItemCaseSensitive(location, "country");
    tzone = cJSON_GetObjectItemCaseSensitive(location, "tz_id");
    localtime_epoch = cJSON_GetObjectItemCaseSensitive(location, "localtime_epoch");
    current = cJSON_GetObjectItemCaseSensitive(data, "current");
    temperature = cJSON_GetObjectItemCaseSensitive(current, "temp_c");
    condition = cJSON_GetObjectItemCaseSensitive(current, "condition");
    text = cJSON_GetObjectItemCaseSensitive(condition, "text");

    printf("%s, %s  Time Zone: %s  %.0f°C  %s\n",
            name->valuestring,
            country->valuestring,
            tzone->valuestring,
            temperature->valuedouble,
            text->valuestring);

    forecast = cJSON_GetObjectItemCaseSensitive(data, "forecast");
    forecastday = cJSON_GetObjectItemCaseSensitive(forecast, "forecastday");

    cJSON_ArrayForEach(item, forecastday) {
        cJSON *hour = cJSON_GetObjectItemCaseSensitive(item, "hour");
        cJSON *each = NULL;
        cJSON_ArrayForEach(each, hour) {
            cJSON *time_epoch = cJSON_GetObjectItemCaseSensitive(each, "time_epoch");
            if(time_epoch->valuedouble < localtime_epoch->valuedouble) {
                continue;
            }
            cJSON *time = cJSON_GetObjectItemCaseSensitive(each, "time");
            cJSON *htemperature = cJSON_GetObjectItemCaseSensitive(each, "temp_c");
            cJSON *hcondition = cJSON_GetObjectItemCaseSensitive(each, "condition");
            cJSON *htext = cJSON_GetObjectItemCaseSensitive(hcondition, "text");
            cJSON *chance_of_rain = cJSON_GetObjectItemCaseSensitive(each, "chance_of_rain");

            printf("%s  Temp:%4.0f°C  Rain:%3.0f%%  %s\n",
                    time->valuestring,
                    htemperature->valuedouble,
                    chance_of_rain->valuedouble,
                    htext->valuestring);
        }
    }

    cJSON_Delete(data);

    return 0;
}
