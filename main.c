#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct result
{
    char title[256];
    char time[50];
    char clientList[1024 * 30];
    char routingTable[1024 * 30];
    struct header *header;
} *Result;

typedef struct header
{
    char clientList[512];
    char routingTable[512];
} *Header;

typedef struct schema
{
    char name[21];
    char *format;
} *ClientSchema;

char jsonFormatInt[] = "\"%s\":%s";
char jsonFormatStr[] = "\"%s\":\"%s\"";

struct schema clientSchema[] = {
    {{"commonName"}, jsonFormatStr},
    {{"realAddress"}, jsonFormatStr},
    {{"virtualAddress"}, jsonFormatStr},
    {{"virtualIPv6Address"}, jsonFormatStr},
    {{"bytesReceived"}, jsonFormatInt},
    {{"bytesSent"}, jsonFormatInt},
    {{"connectedSince"}, jsonFormatStr},
    {{"connectedSinceTime_t"}, jsonFormatInt},
    {{"username"}, jsonFormatStr},
    {{"clientId"}, jsonFormatInt},
    {{"peerId"}, jsonFormatInt},
};

struct schema routingSchema[] = {
    {{"virtualAddress"}, jsonFormatStr},
    {{"commonName"}, jsonFormatStr},
    {{"realAddress"}, jsonFormatStr},
    {{"lastRef"}, jsonFormatStr},
    {{"lastRefTime_t"}, jsonFormatInt},
};

char **splittingLine(int cnt, char **result, char str[])
{
    int resultIndex = 0;
    char *start = str;
    char *end;
    end = strstr(start, ",");
    while (end)
    {
        *end = '\0';
        result[resultIndex] = (char *)malloc(sizeof(char *) * (strlen(start) + 1));
        if (result[resultIndex] == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        strcpy(result[resultIndex], start);
        start = end + strlen(",");
        end = strstr(start, ",");
        resultIndex++;
    }

    if (resultIndex < cnt)
    {
        result[resultIndex] = (char *)malloc(sizeof(char *) * (strlen(start) + 1));
        strcpy(result[resultIndex], start);
    }
    return result;
}

int handlingString(int cnt, char **split, char *result)
{
    if (cnt < 2)
        return 0;
    strcpy(result, split[1]);
}

int handlingArray(int cnt, char **split, char *result)
{
    if (cnt < 3)
        return 0;
    char buffer[256] = {'\0'};
    char bufferArray[256 * 4] = {'\0'};

    for (int i = 1; i < cnt; i++)
    {
        sprintf(buffer, "\"%s\"", split[i]);
        if (i < cnt - 1)
        {
            strcat(buffer, ",");
        }
        strcat(bufferArray, buffer);
    }
     strcpy(result, bufferArray);
}

int handlingObject(int cnt, char **split, char *result, struct schema schema[])
{
    if (cnt < 1)
        return 0;
    char buffer[256 * 4] = {'\0'};
    char bufferObject[256 * 4] = {'\0'};

    for (int i = 1; i < cnt; i++)
    {
        sprintf(buffer, schema[i - 1].format, schema[i - 1].name, split[i]);
        if (i < cnt - 1)
        {
            strcat(buffer, ",");
        }
        strcat(bufferObject, buffer);
    }
    if (strlen(result) > 0)
        strcat(result, ",");
    sprintf(buffer, "{%s}", bufferObject);
    strcat(result, buffer);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stdout, "Не указан путь к файлу лога\n");
        return 0;
    }

    if (strcmp("-v", argv[1]) == 0 || strcmp("--version", argv[1]) == 0)
    {
        fprintf(stdout, "v0.0.6\n");
        return 0;
    }

    char row[256];
    FILE *statusLog = fopen(argv[1], "r");
    if (statusLog == NULL)
    {
        fprintf(stdout, "Ошибка открытия файла %s\n", argv[1]);
        return 0;
    }

    Result result = malloc(sizeof(struct result));
    if (result == NULL)
    {
        fprintf(stdout, "Ошибка выделения памяти под структуру\n");
        exit(1);
    }
    result->header = malloc(sizeof(struct header));

    while (fgets(row, 256, statusLog) != NULL)
    {
        row[strcspn(row, "\n")] = 0;
        char *t2 = row;
        int cnt = 1;
        while (*(t2++))
            if (*t2 == ',' && *(t2 + 1) != 0)
                cnt++;

        char **columnArr = malloc(cnt * sizeof(char *));
        if (columnArr == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }

        splittingLine(cnt, columnArr, row);

        if (cnt > 0)
        {
            if (strcmp("TITLE", columnArr[0]) == 0)
            {
                handlingString(cnt, columnArr, result->title);
            }
            if (strcmp("TIME", columnArr[0]) == 0)
            {
                handlingArray(cnt, columnArr, result->time);
            }
            if (strcmp("HEADER", columnArr[0]) == 0 && strcmp("CLIENT_LIST", columnArr[1]) == 0)
            {
                handlingArray(cnt - 1, columnArr + 1, result->header->clientList);
            }
            if (strcmp("HEADER", columnArr[0]) == 0 && strcmp("ROUTING_TABLE", columnArr[1]) == 0)
            {
                handlingArray(cnt - 1, columnArr + 1, result->header->routingTable);
            }
            if (strcmp("CLIENT_LIST", columnArr[0]) == 0)
            {
                handlingObject(cnt, columnArr, result->clientList, clientSchema);
            }
            if (strcmp("ROUTING_TABLE", columnArr[0]) == 0)
            {
                handlingObject(cnt, columnArr, result->routingTable, routingSchema);
            }
        }

        for (int i = 0; i < cnt; i++)
        {
            free(columnArr[i]);
        }
        free(columnArr);
    }

    Header header = result->header;
    fprintf(stdout, "{\"title\":\"%s\",\"time\":[%s],\"clientList\":[%s],\"routingTable\":[%s],\"header\":{\"clientList\":[%s],\"routingTable\":[%s]}}",
            result->title, result->time, result->clientList, result->routingTable, header->clientList, header->routingTable);

    free(result->header);
    free(result);
    fclose(statusLog);
}