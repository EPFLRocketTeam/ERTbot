#ifndef SHEETAPI_H
#define SHEETAPI_H

void sheetApi(char *query, char *url, char *requestType);

void batchUpdateSheet(char *sheetId, char *range, char *values);

void batchGetSheet(char *sheetId, char *range);

void refreshOAuthToken();

#endif