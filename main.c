#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct result
{
    char title[256];
    char time[30];
    char clientList[1024 * 10];
} *Result;

char clientListHead[11][21] = {
    "commonName",
    "realAddress",
    "virtualAddress",
    "VirtualIPv6Address",
    "bytesReceived",
    "bytesSent",
    "connectedSince",
    "connectedSinceTime_t",
    "username",
    "clientId",
    "peerId"};

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stdout, "Не указан путь к файлу лога\n");
        return 0;
    }

    char line[256];
    FILE *statusLog = fopen(argv[1], "r");
    Result result = malloc(sizeof(struct result));
    if (result == NULL)
    {
        fprintf(stdout, "Ошибка выделения памяти под структуру\n");
        exit(1);
    }
    if (statusLog == NULL)
    {
        fprintf(stdout, "Ошибка открытия файла %s\n", argv[1]);
        return 0;
    }

    while (fgets(line, sizeof(line), statusLog) != NULL)
    {
        char *chank;
        chank = strtok(line, ",");
        while (chank != NULL)
        {
            if (strcmp("TITLE", chank) == 0)
            {
                chank = strtok(NULL, ",");
                chank[strcspn(chank, "\n")] = 0;
                strcpy(result->title, chank);
                break;
            }
            if (strcmp("TIME", chank) == 0)
            {
                chank = strtok(NULL, ",");
                chank[strcspn(chank, "\n")] = 0;
                strcpy(result->time, chank);
                break;
            }
            if (strcmp("HEADER", chank) == 0)
            {
                break;
            }
            if (strcmp("CLIENT_LIST", chank) == 0)
            {
                char buffer[256 * 6] = {'\0'};
                char bufferClientObject[1024 * 8] = {'\0'};
                chank = strtok(NULL, ",");
                int cx;
                int indexHeader = 0;
                
                while (chank != NULL)
                {
                  //  printf("--- %d value %s\n", strlen(chank), chank);
                    chank[strcspn(chank, "\n")] = 0;
                    cx = snprintf(buffer, 256 * 6, "\"%s\":\"%s\"", clientListHead[indexHeader++], chank);
                    strcat(bufferClientObject, buffer);
                    chank = strtok(NULL, ",");
                    if (chank != NULL)
                        strcat(bufferClientObject, ",");
                }
                if (strlen(bufferClientObject) > 0)
                {
                    cx = snprintf(buffer, 1024 * 4, "{%s}", bufferClientObject);
                    if (strlen(result->clientList) > 0)
                    {
                        strcat(result->clientList, ",");
                    }
                    strcat(result->clientList, buffer);
                }
                break;
            }
            // printf("%s\n", chank);
            chank = strtok(NULL, ",");
        }
    }

    fprintf(stdout, "{\"title\":\"%s\",\"time\":\"%s\",\"clientList\":[%s]}",
            result->title, result->time, result->clientList);

    free(result);
    fclose(statusLog);
}