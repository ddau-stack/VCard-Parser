/**
 * @author Dung Dau 0924222
 * @date Oct 9 2020
 */

#ifndef VCHELPERS_H
#define VCHELPERS_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "LinkedListAPI.h"

char *printParamString(void*str);
int comparParameters(const void *a, const void *b);
bool parseParameter(char *str, char **name, char **value);
List *splitOnSemicolon(char *str);
List *splitMultipleParameters(char *str);

void createDateTimeProperty(char *dateTimeValue, bool *UTC, bool *isText, char **date, char **time, char **text, char *parameterString);
void createUTCTime(char *dateTimeValue, char **date, char **time, char **text);
void createNonUTCTime(char *dateTimeValue, char **date, char **time, char **text);
void createUTCDateAndOrTime(char *dateTimeValue, char **date, char **time, char **text);
void createNonUTCDateAndOrTime(char *dateTimeValue, char **date, char **time, char **text);
void createDateOnly(char *dateTimeValue, char **date, char **time, char **text);
void removeLineFeed(char **str);
void parseGroupAndPropertyName(char **propName, char **group);
char *nameValueToJSON(char *name, char *value);
char *nameBooleanToJSON(char *name, bool value);
#endif