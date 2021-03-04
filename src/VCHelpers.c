/**
 * @author Dung Dau 0924222
 * @date Oct 9 2020
 */

#include "VCHelpers.h"


char *printParamString(void*str) {
    return (char*)str;
}

int comparParameters(const void *a, const void *b) {
    char *string1 = (char*)a;
    char *string2 = (char*)b;
    return string1-string2;
}

bool parseParameter(char *str, char **name, char **value) {
    if(strstr(str, "=") == NULL) {
        return false;
    }
    int indexOfEquals = strcspn(str, "=");
    *name = calloc(indexOfEquals+1, sizeof(char));
    *value = calloc(strlen(str)-indexOfEquals, sizeof(char));
    for(int i = 0; i < indexOfEquals; i++) {
        (*name)[i] = str[i];
    }
    (*name)[indexOfEquals] = '\0';

    for(int i = indexOfEquals + 1; i < strlen(str); i++) {
        (*value)[i-(indexOfEquals + 1)] = str[i];
    }
    (*value)[strlen((*value))] = '\0';
    return true;
}

List *splitOnSemicolon(char *str) {
    List *paramList = initializeList(&printParamString, &free, &comparParameters);

    //looks for the number of semicolons
    int numSemicolons = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ';') {
            numSemicolons++;
        }
    }

    //looks for the locations of the semicolons
    int semiColonIndexes[numSemicolons];
    int iter = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ';') {
            semiColonIndexes[iter] = i;
            iter++;
        }
    }

    if(numSemicolons > 0) {
        //get the first parameter
        char **parameters = calloc(numSemicolons+1, sizeof(char*));
        parameters[0] = calloc(semiColonIndexes[0]+1, sizeof(char));
        for(int i = 0; i < semiColonIndexes[0]; i++) {
            parameters[0][i] = str[i];
        }
        parameters[0][semiColonIndexes[0]] = '\0';
        insertBack(paramList, (void*)parameters[0]);

        //gets the remaining parameters
        if(numSemicolons >= 2) {
            for(int i = 0; i < numSemicolons-1; i++) {
                parameters[i+1] = calloc(semiColonIndexes[i+1]-semiColonIndexes[i], sizeof(char));
                for(int j = semiColonIndexes[i]+1; j < semiColonIndexes[i+1]; j++) {
                    parameters[i+1][j-(semiColonIndexes[i]+1)] = str[j];
                }
                parameters[i+1][semiColonIndexes[i+1]-semiColonIndexes[i]-1] = '\0';
                insertBack(paramList, (void*)parameters[i+1]);
            }
        }

        //get the last parameter
        parameters[numSemicolons] = calloc(strlen(str)-semiColonIndexes[numSemicolons-1], sizeof(char));
        for(int i = semiColonIndexes[numSemicolons-1]+1; i < strlen(str); i++) {
            parameters[numSemicolons][i-(semiColonIndexes[numSemicolons-1]+1)] = str[i];
        }
        parameters[numSemicolons][strlen(str)-semiColonIndexes[numSemicolons-1]-1] = '\0';
        insertBack(paramList, (void*)parameters[numSemicolons]);
        free(parameters);
    }
    if(numSemicolons == 0) {
        char *parameter = calloc(strlen(str)+1, sizeof(char));
        strcpy(parameter, str);
        insertBack(paramList, (void*)parameter);
    }
    return paramList;
}

List *splitMultipleParameters(char *str) {
    List *paramList = initializeList(&printParamString, &free, &comparParameters);

    //looks for the number of semicolons
    int numSemicolons = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ';') {
            numSemicolons++;
        }
    }

    //looks for the locations of the semicolons
    int semiColonIndexes[numSemicolons];
    int iter = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ';') {
            semiColonIndexes[iter] = i;
            iter++;
        }
    }

    if(numSemicolons > 0) {
        //get the first parameter
        char **parameters = calloc(numSemicolons+1, sizeof(char*));
        parameters[0] = calloc(semiColonIndexes[0]+1, sizeof(char));
        for(int i = 0; i < semiColonIndexes[0]; i++) {
            parameters[0][i] = str[i];
        }
        parameters[0][semiColonIndexes[0]] = '\0';
        insertBack(paramList, (void*)parameters[0]);

        //gets the remaining parameters
        if(numSemicolons >= 2) {
            for(int i = 0; i < numSemicolons-1; i++) {
                parameters[i+1] = calloc(semiColonIndexes[i+1]-semiColonIndexes[i], sizeof(char));
                for(int j = semiColonIndexes[i]+1; j < semiColonIndexes[i+1]; j++) {
                    parameters[i+1][j-(semiColonIndexes[i]+1)] = str[j];
                }
                parameters[i+1][semiColonIndexes[i+1]-semiColonIndexes[i]-1] = '\0';
                insertBack(paramList, (void*)parameters[i+1]);
            }
        }

        //get the last parameter
        parameters[numSemicolons] = calloc(strlen(str)-semiColonIndexes[numSemicolons-1], sizeof(char));
        for(int i = semiColonIndexes[numSemicolons-1]+1; i < strlen(str); i++) {
            parameters[numSemicolons][i-(semiColonIndexes[numSemicolons-1]+1)] = str[i];
        }
        parameters[numSemicolons][strlen(str)-semiColonIndexes[numSemicolons-1]-1] = '\0';

        //checks for invalid parameters
        for(int i = 0; i < numSemicolons+1; i++) {
            if(strstr(parameters[i], "=") == NULL) {
                for(int j = 0; j < numSemicolons+1; j++) {
                    free(parameters[j]);
                }
                free(parameters);
                freeList(paramList);
                return NULL;
            }
        }
        insertBack(paramList, (void*)parameters[numSemicolons]);
        free(parameters);
    }
    if(numSemicolons == 0) {
        //if invalid parameter is found
        if(strstr(str, "=") == NULL) {
            freeList(paramList);
            return NULL;
        }
        char *parameter = calloc(strlen(str)+1, sizeof(char));
        strcpy(parameter, str);
        insertBack(paramList, (void*)parameter);
    }
    return paramList;
}

void createDateTimeProperty(char *dateTimeValue, bool *UTC, bool *isText, char **date, char **time, char **text, char *parameterString) {
    List *splitParameters = NULL;
    ListIterator iter;
    char *currentParameter = NULL;
    if (parameterString != NULL) {
        splitParameters = splitMultipleParameters(parameterString);
        iter = createIterator(splitParameters);
        currentParameter = nextElement(&iter);
    }

    //checks if it is a UTC time
    if(dateTimeValue[strlen(dateTimeValue)-1] == 'Z') {
        *UTC = true;
    } else {
        *UTC = false;   
    }

    //checks if isText is true
    *isText = false;
    if(*UTC == false) {
        while(currentParameter != NULL) {
            if(strcmp(currentParameter, "VALUE=text") == 0) {
                *isText = true;
                break; 
            }
            currentParameter = nextElement(&iter);
        }
    }

    //text format
    if(*isText == true) {
        *isText = true;
        *date = calloc(1, sizeof(char));
        strcpy(*date, "");
        *time = calloc(1, sizeof(char));
        strcpy(*time, "");
        *text = calloc(strlen(dateTimeValue)+1, sizeof(char));
        strcpy(*text, dateTimeValue);
    }

    //add date cases        
    if(dateTimeValue[0] == 'T' && *UTC == true) {
        //UTC time
        createUTCTime(dateTimeValue, date, time, text);
    } else if(dateTimeValue[0] == 'T' && *UTC == false) {
        //non-UTC time
        createNonUTCTime(dateTimeValue, date, time, text);
    } else if(strstr(dateTimeValue, "T") != NULL && *UTC == true) {
        //UTC Date-and-or-Time
        createUTCDateAndOrTime(dateTimeValue, date, time, text);
    } else if(strstr(dateTimeValue, "T") != NULL && *UTC == false) {
        //non-UTC Date-and-or-Time
        createNonUTCDateAndOrTime(dateTimeValue, date, time, text);
    } else if(strstr(dateTimeValue, "T") == NULL && *isText == false) {
        //date only
        createDateOnly(dateTimeValue, date, time, text);
    }
    freeList(splitParameters);
}

void createUTCTime(char *dateTimeValue, char **date, char **time, char **text) {
    *date = calloc(1, sizeof(char));
    strcpy(*date, "");
    *text = calloc(1, sizeof(char));
    strcpy(*text, "");

    int indexOfZ = strcspn(dateTimeValue, "Z");
    *time = calloc(indexOfZ+1, sizeof(char));
    for(int i = 0; i < indexOfZ-1; i++) {
        (*time)[i] = dateTimeValue[i+1];
    }
    (*time)[indexOfZ] = '\0';
}

void createNonUTCTime(char *dateTimeValue, char **date, char **time, char **text) {
    *date = calloc(1, sizeof(char));
    strcpy(*date, "");
    *text = calloc(1, sizeof(char));

    strcpy(*text, "");
    int indexOfTerminatingCharacter = strcspn(dateTimeValue, "\0");
    *time = calloc(indexOfTerminatingCharacter+1, sizeof(char));
        
    for(int i = 0; i < indexOfTerminatingCharacter-1; i++) {
        (*time)[i] = dateTimeValue[i+1];
    }
    (*time)[indexOfTerminatingCharacter] = '\0';
}

void createUTCDateAndOrTime(char *dateTimeValue, char **date, char **time, char **text) {
    int indexOfT = strcspn(dateTimeValue, "T");
    char *readInDate = calloc(indexOfT+1, sizeof(char));

    for(int i = 0; i < indexOfT; i++) {
        readInDate[i] = dateTimeValue[i];
    }
    readInDate[indexOfT] = '\0';

    int indexOfZ = strcspn(dateTimeValue, "Z");
    int sizeOfDate = indexOfZ-indexOfT;
    char *readInTime = calloc(sizeOfDate, sizeof(char));

    for(int i = 0; i < sizeOfDate; i++) {
        readInTime[i] = dateTimeValue[indexOfT + i + 1];
    }
    readInTime[sizeOfDate-1] = '\0';

    *date = calloc(strlen(readInDate)+1, sizeof(char));
    strcpy(*date, readInDate);
    *time = calloc(strlen(readInTime)+1, sizeof(char));
    strcpy(*time, readInTime);
    *text = calloc(1, sizeof(char));
    strcpy(*text, "");

    free(readInDate);
    free(readInTime);
}

void createNonUTCDateAndOrTime(char *dateTimeValue, char **date, char **time, char **text) {
    int indexOfT = strcspn(dateTimeValue, "T");
    char *readInDate = calloc(indexOfT+1, sizeof(char));

    for(int i = 0; i < indexOfT; i++) {
        readInDate[i] = dateTimeValue[i];
    }
    readInDate[indexOfT] = '\0';

    int sizeOfDate = strlen(dateTimeValue) - indexOfT;
    char *readInTime = calloc(sizeOfDate, sizeof(char));

    for(int i = 0; i < sizeOfDate; i++) {
        readInTime[i] = dateTimeValue[indexOfT + i + 1];
    }
    readInTime[sizeOfDate-1] = '\0';

    *date = calloc(strlen(readInDate)+1, sizeof(char));
    strcpy(*date, readInDate);
    *time = calloc(strlen(readInTime)+1, sizeof(char));
    strcpy(*time, readInTime);
    *text = calloc(1, sizeof(char));
    strcpy(*text, "");

    free(readInDate);
    free(readInTime);
}

void createDateOnly(char *dateTimeValue, char **date, char **time, char **text) {
    *text = calloc(1, sizeof(char));
    strcpy(*text, "");
    *time = calloc(1, sizeof(char));
    strcpy(*time, "");
    *date = calloc(strlen(dateTimeValue)+1, sizeof(char));
    strcpy(*date, dateTimeValue);
}

void removeLineFeed(char **str) {
    if(strstr(*str, "\r\n")) {
        (*str)[strcspn(*str, "\r\n")] = '\0';
    } else if(strstr(*str, "\n")) {
        (*str)[strcspn(*str, "\n")] = '\0';
    }
}

void parseGroupAndPropertyName(char **propName, char **group) {
    if(strstr(*propName, ".") == NULL) {
        return;
    }
    int periodIndex = strcspn(*propName, ".");
    char *newPropName = calloc(strlen(*propName)-periodIndex, sizeof(char));
    for(int i = periodIndex+1; i < strlen(*propName); i++) {
        newPropName[i-(periodIndex+1)] = (*propName)[i];
    }
    newPropName[(strlen(*propName)-periodIndex)-1] = '\0';
    if(*group == NULL) {
        *group = calloc(periodIndex+1, sizeof(char));
    } else {
        free(*group);
        *group = calloc(periodIndex+1, sizeof(char));
    }
    for(int i = 0; i < periodIndex; i++) {
        (*group)[i] = (*propName)[i];
    }
    (*group)[periodIndex] = '\0';
    strcpy(*propName, newPropName);
    free(newPropName);
}

char *nameValueToJSON(char *name, char *value) {
    char *ret;
    if(value != NULL) {
        ret = calloc(6+strlen(name)+strlen(value), sizeof(char));
    } else {
        ret = calloc(7+strlen(name), sizeof(char));
    }
    strcat(ret, "\"");
    strcat(ret, name);
    strcat(ret, "\":\"");
    if(value != NULL) {
        strcat(ret, value);
    }
    else {
        strcat(ret, "");
    }
    strcat(ret, "\"");
    return ret;
}

char *nameBooleanToJSON(char *name, bool value) {
    char *ret;
    char *valueAsString = NULL;

    if(value == true) {
        valueAsString = calloc(5, sizeof(char));
        strcpy(valueAsString, "true");
    } else {
        valueAsString = calloc(6, sizeof(char));
        strcpy(valueAsString, "false");
    }

    ret = calloc(4+strlen(name)+strlen(valueAsString), sizeof(char));
    strcat(ret, "\"");
    strcat(ret, name);
    strcat(ret, "\":");
    strcat(ret, valueAsString);

    free(valueAsString);

    return ret;
}