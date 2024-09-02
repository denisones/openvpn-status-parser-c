#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct result
{
    char title[256];
    char time[50];
    char clientList[1024 * 30];
    char routingTable[1024 * 30];
} *Result;

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

int handlingTitle(int cnt, char **split, char *result)
{
    if (cnt < 2)
        return 0;
    strcpy(result, split[1]);
}

int handlingTime(int cnt, char **split, char *result)
{
    if (cnt < 3)
        return 0;
    char buffer[256] = {'\0'};
    sprintf(buffer, "[\"%s\", \"%s\"]", split[1], split[2]);
    strcpy(result, buffer);
}

int handlingList(int cnt, char **split, char *result, struct schema schema[])
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
        fprintf(stdout, "v0.0.5\n");
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
                handlingTitle(cnt, columnArr, result->title);
            }
            if (strcmp("TIME", columnArr[0]) == 0)
            {
                handlingTime(cnt, columnArr, result->time);
            }
            if (strcmp("CLIENT_LIST", columnArr[0]) == 0)
            {
                handlingList(cnt, columnArr, result->clientList, clientSchema);
            }
            if (strcmp("ROUTING_TABLE", columnArr[0]) == 0)
            {
                handlingList(cnt, columnArr, result->routingTable, routingSchema);
            }
        }

        for (int i = 0; i < cnt; i++)
        {
            free(columnArr[i]);
        }
        free(columnArr);
    }

    fprintf(stdout, "{\"title\":\"%s\",\"time\":%s,\"clientList\":[%s],\"routingTable\":[%s]}",
            result->title, result->time, result->clientList, result->routingTable);

    free(result);
    fclose(statusLog);
}