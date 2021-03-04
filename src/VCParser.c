/**
 * @author Dung Dau 0924222
 * @date October 9 2020
 */
#include "VCParser.h"
#include "VCHelpers.h"
//TODO: property names are case insensitive
//copy the code for toString() stuff to replace function calls
//fix JSONtoStrList() when it fails the fifth one

static VCardErrorCode validatePropertyName(char *name) {
    if(name == NULL) {
        return INV_PROP;
    }

    if(strcmp(name, "") == 0) {
        return INV_PROP;
    }
    return OK;
}

static VCardErrorCode validatePropertyGroup(char *group) {
    if(group == NULL) {
        return INV_PROP;
    }
    return OK;
}

static VCardErrorCode validateParameter(Parameter *param) {
    if(param->name == NULL) {
        return INV_PROP;
    }
    if(param->value == NULL) {
        return INV_PROP;
    }
    if(strcmp(param->name, "") == 0) {
        return INV_PROP;
    }
    if(strcmp(param->value, "") == 0) {
        return INV_PROP;
    }
    return OK;
}

static VCardErrorCode validateParameterList(List *paramList) {
    if(paramList == NULL) {
        return INV_PROP;
    }
    VCardErrorCode code;
    ListIterator iter = createIterator(paramList);
    Parameter *temp = (Parameter*)nextElement(&iter);

    while(temp != NULL) {
        code = validateParameter(temp);
        if(code != OK) {
            return code;
        }
        temp = (Parameter*)nextElement(&iter);
    }
    return OK;
}

static VCardErrorCode validateValueList(List *valueList) {
    if(valueList == NULL) {
        return INV_PROP;
    }
    if(valueList->length < 1) {
        return INV_PROP;
    }
    ListIterator iter = createIterator(valueList);
    char *currValue = (char*)nextElement(&iter);
    if(currValue == NULL) {
        return INV_PROP;
    }
    while (currValue != NULL) {
        if(currValue == NULL) {
            return INV_PROP;
        }
        currValue = (char*)nextElement(&iter);
    }
    return OK;
}

static VCardErrorCode validateProperty(Property *property) {
    VCardErrorCode code;
    code = validatePropertyName(property->name);
    if(code != OK) {
        return code;
    }
    code = validatePropertyGroup(property->group);
    if(code != OK) {
        return code;
    }
    code = validateParameterList(property->parameters);
    if(code != OK) {
        return code;
    }
    code = validateValueList(property->values);
    if(code != OK) {
        return code;
    }
    return OK;
}

static VCardErrorCode validateFN(Property *fn) {
    if(fn == NULL) {
        return INV_CARD;
    }
    return validateProperty(fn);
}

static VCardErrorCode validateOptionalProperties(List *list) {
    if(list == NULL) {
        return INV_CARD;
    }

    ListIterator iter = createIterator(list);
    void *currProp;

    while((currProp = nextElement(&iter)) != NULL) {
        Property *tmpProp = (Property*)currProp;
        validateProperty(tmpProp);
    }
    return OK;
}

static VCardErrorCode validateDateTime(DateTime *dateTime) {
    if(dateTime == NULL) {
        return OK;
    }
    if(dateTime->date == NULL) {
        return INV_DT;
    }
    if(dateTime->time == NULL) {
        return INV_DT;
    }
    if(dateTime->text == NULL) {
        return INV_DT;
    }
    if(dateTime->UTC && strcmp(dateTime->time, "") == 0) {
        return INV_DT;
    }
    if(dateTime->isText == true) {
        if(dateTime->UTC == true) {
            return INV_DT;
        }
        if(strcmp(dateTime->date, "") != 0) {
            return INV_DT;
        } 
        if(strcmp(dateTime->time, "") != 0) {
            return INV_DT;
        }
        if(strcmp(dateTime->text, "") == 0) {
            return INV_DT;
        }
    } else {      
        if(strcmp(dateTime->text, "") != 0) {
            return INV_DT;
        }
        //should check this assumption with Dennis
        if(strcmp(dateTime->date, "") == 0 && strcmp(dateTime->time, "") == 0) {
            return INV_DT;
        }
    }
    return OK;
}

static VCardErrorCode validateOptionalPropertyNames(List *props) {
    if(getLength(props) == 0) {
        return OK;
    }
    char *generalProperties[3] = {"SOURCE", "KIND", "XML"};
    char *identificationProperties[5] = {"FN", "N", "NICKNAME", "PHOTO", "GENDER"};
    char *deliveryAddressingProperties[1] = {"ADR"};
    char *communicationProperties[4] = {"TEL", "EMAIL", "IMPP", "LANG"};
    char *geographicalProperties[2] = {"TZ", "GEO"};
    char *organizationalProperties[6] = {"TITLE", "ROLE", "LOGO", "ORG", "MEMBER", "RELATED"};
    char *explanatoryProperties[8] = {"CATEGORIES", "NOTE", "PRODID", "REV", "SOUND", "UID", "CLIENTPIDMAP", "URL"};
    char *securityProperties[1] = {"KEY"};
    char *calendarProperties[3] = {"FBURL", "CALADRURI", "CALURI"};

    ListIterator iter = createIterator(props);
    Property *prop = (Property*)nextElement(&iter);
    bool found = false;
    bool kindFound = false;
    bool nFound = false;
    bool genderFound = false;
    bool prodidFound = false;
    bool revFound = false;
    bool uidFound = false;

    while(prop != NULL) {
        if(strcmp(prop->name, "VERSION") == 0) {
            return INV_CARD;
        }
        if(strcmp(prop->name, "BEGIN") == 0) {
            return INV_CARD;
        }if(strcmp(prop->name, "END") == 0) {
            return INV_CARD;
        }
        if(strcmp(prop->name, "BDAY") == 0) {
            return INV_DT;
        }
        if(strcmp(prop->name, "ANNIVERSARY") == 0) {
            return INV_DT;
        }
        for(int i = 0; i < 3; i++) {
            if(strcmp(prop->name, generalProperties[i]) == 0) {
                if(kindFound == true) {
                    return INV_PROP;
                }
                if(strcmp(prop->name, "KIND") == 0) {
                    kindFound = true;
                }
                found = true;
            }
        }
        for (int i = 0; i < 5; i++) {
            if(strcmp(prop->name, identificationProperties[i]) == 0) {
                if(nFound == true) {
                    return INV_PROP;
                }
                if(genderFound == true) {
                    return INV_PROP;
                }
                if(strcmp(prop->name, "N") == 0) {
                    nFound = true;
                    if(getLength(prop->values) != 5) {
                        return INV_PROP;
                    }
                }
                if(strcmp(prop->name, "GENDER") == 0) {
                    genderFound = true;
                }
                found = true;
            }
        }
        for (int i = 0; i < 1; i++) {
            if(strcmp(prop->name, deliveryAddressingProperties[i]) == 0) {
                found = true;
            }
        }
        for (int i = 0; i < 4; i++) {
            if(strcmp(prop->name, communicationProperties[i]) == 0) {
                found = true;
            }
        }
        for (int i = 0; i < 2; i++) {
            if(strcmp(prop->name, geographicalProperties[i]) == 0) {
                found = true;
            }
        }
        for (int i = 0; i < 6; i++) {
            if(strcmp(prop->name, organizationalProperties[i]) == 0) {
                found = true;
            }
        }
        for (int i = 0; i < 8; i++) {
            if(strcmp(prop->name, explanatoryProperties[i]) == 0) {
                if(prodidFound == true) {
                    return INV_PROP;
                }
                if(revFound == true) {
                    return INV_PROP;
                }
                if(uidFound == true) {
                    return INV_PROP;
                }
                if(strcmp(prop->name, "PRODID") == 0) {
                    prodidFound = true;
                }
                if(strcmp(prop->name, "REV") == 0) {
                    revFound = true;
                }
                if(strcmp(prop->name, "UID") == 0) {
                    uidFound = true;
                }
                found = true;
            }
        }
        for (int i = 0; i < 1; i++) {
            if(strcmp(prop->name, securityProperties[i]) == 0) {
                found = true;
            }
        }
        for (int i = 0; i < 3; i++) {
            if(strcmp(prop->name, calendarProperties[i]) == 0) {
                found = true;
            }
        }
        if(found == false) {
            return INV_PROP;
        }
        prop = (Property*)nextElement(&iter);
    }
    if(found == false) {
        return INV_PROP;
    }
    return OK;
}

static VCardErrorCode createOptionalProperty(Card **card, char *name, char *theParameters, char *value) {
    Property *newProp = calloc(1, sizeof(Property));
    newProp->name = calloc(strlen(name)+1, sizeof(char));
    if(newProp->name == NULL) {
        return OTHER_ERROR;
    }
    strcpy(newProp->name, name);
    newProp->group = calloc(1,sizeof(char));
    if(newProp->group == NULL) {
        return OTHER_ERROR;
    }
    strcpy(newProp->group, "");
    parseGroupAndPropertyName(&(newProp->name), &(newProp->group));
    newProp->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
    if(theParameters != NULL) {
        List *splitParameters = splitMultipleParameters(theParameters);
        if(splitParameters == NULL) {
            free(newProp->name);
            free(newProp->group);
            free(newProp->parameters);
            free(newProp);
            deleteCard(*card);
            free(name);
            free(theParameters);
            free(value);
            *card = NULL;
            return INV_PROP;
        }
        ListIterator stringIterator = createIterator(splitParameters);
        char *currentParameter = nextElement(&stringIterator);
        Parameter *parameters[getLength(splitParameters)];
        int location = 0;

        while(currentParameter != NULL) {
            parameters[location] = calloc(1, sizeof(Parameter));
            bool ret = parseParameter((char*)currentParameter, &((parameters[location])->name), &((parameters[location])->value));
            if(ret == false) {
                free(name);
                free(value);
                free(theParameters);
                for(int i = 0; i < getLength(splitParameters); i++) {
                    if(parameters[i] != NULL) {
                        free(parameters[i]);
                    }
                }
                deleteProperty(newProp);
                freeList(splitParameters);
                deleteCard(*card);
                *card = NULL;
                return INV_PROP;
            }
            insertBack(newProp->parameters, (void*)parameters[location]);
            currentParameter = nextElement(&stringIterator);
        }
        free(theParameters);
        freeList(splitParameters);
    }
    if(newProp->parameters == NULL) {
        return OTHER_ERROR;
    }
    newProp->values = splitOnSemicolon(value);
    insertBack((*card)->optionalProperties, (void*)newProp);
    return OK;
}

static void deepCopyList(List **destination, const List *source) {
    const List *temp = source;

    Node *curr = temp->head;
    while(curr != NULL) {
        void *value = curr->data;
        char *valueCopy = calloc(strlen((char*)value)+1, sizeof(char));
        strcpy(valueCopy, (char*)value);
        insertBack(*destination, (void*)valueCopy);
        curr = curr->next;
    }
}

static List *splitJSONStringByComma(char *str) {
    int numCommas = 0;

    //counts the number of commas
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ',') {
            numCommas++;
        }
    }

    //records the locations of the commas
    int commaIndexes[numCommas];
    int currentIndex = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] ==',') {
            commaIndexes[currentIndex] = i;
            currentIndex++;
        }
    }

    //computes the lengths the strings for each split atrribute
    int lengths[numCommas+1];
    for(int i = 0; i < numCommas; i++) {
        lengths[i] = 0;
    }
    lengths[0] = commaIndexes[0] - 1;
    for(int i = 1; i < numCommas; i++) {
        lengths[i] = commaIndexes[i] - commaIndexes[i-1] - 1;
    }
    lengths[numCommas] = strlen(str) - commaIndexes[numCommas-1] -2;

    List *ret = initializeList(&valueToString, &deleteValue, &compareValues);
    char *firstValue = calloc(lengths[0]+1, sizeof(char));
    for(int i = 1; i < commaIndexes[0]; i++) {
        firstValue[i-1] = str[i];
    }
    firstValue[commaIndexes[0]-1] = '\0';
    insertBack(ret, firstValue);

    for(int i = 1; i < numCommas; i++) {
        char *value = calloc(lengths[i]+1, sizeof(char));
        for(int j = commaIndexes[i-1]+1; j < commaIndexes[i]; j++) {
            value[j-(commaIndexes[i-1]+1)] = str[j];
        }
        value[lengths[i]] = '\0';
        insertBack(ret, value);
    }

    char *value = calloc(lengths[numCommas-1], sizeof(char));
    for(int i = commaIndexes[numCommas-1]+1; i < strlen(str)-1; i++) {
        value[i-(commaIndexes[numCommas-1]+1)] = str[i];
    }
    value[lengths[numCommas-1]-1] = '\0';
    insertBack(ret, value);

    return ret;
}

static char *extractValue(char *str) {
    int colonIndex = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ':') {
            colonIndex = i;
        }
    }

    char *ret = calloc(strlen(str)-colonIndex, sizeof(char));
    int currIndex = 0;
    for(int i = colonIndex+1; i < strlen(str); i++) {
        if(str[i] == '\"') {
            continue;
        } else {
            ret[currIndex] = str[i];
            currIndex++;
        }
    }
    ret[strlen(str)-colonIndex-1] = '\0';
    return ret;
}

VCardErrorCode createCard(char* fileName, Card** newCardObject) {
    //file name is NULL
    if(fileName == NULL) {
        return INV_FILE;
    }
    
    //looks for .vcf extension
    char vcfExtension[5];
    vcfExtension[4] = '\0';
    int j = 1;
    for(int i = 3; i >= 0; i--) {
        vcfExtension[i] = fileName[strlen(fileName)-j];
        j++;
    }
    j = 1;

    //looks for .vcard extension
    char vcardExtension[7];
    vcardExtension[6] = '\0';
    for(int i = 5; i >= 0; i--) {
        vcardExtension[i] = fileName[strlen(fileName)-j];
        j++;
    }

    if(strcmp(vcfExtension, ".vcf") != 0 && strcmp(vcardExtension, ".vcard") != 0) {
        return INV_FILE;
    }

    FILE *fp = fopen(fileName, "r");
    if(fp == NULL) {
        if(newCardObject != NULL) {
            if(*newCardObject != NULL) {
                deleteCard(*newCardObject);
            }
        }
        return INV_FILE;
    }
    *newCardObject = calloc(1,sizeof(Card));
    if(*newCardObject == NULL) {
        return OTHER_ERROR;
    }
    (*newCardObject)->fn = calloc(1,sizeof(Property));
    if((*newCardObject)->fn == NULL) {
        return OTHER_ERROR;
    }
    (*newCardObject)->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareValues);
    if((*newCardObject)->optionalProperties == NULL) {
        return OTHER_ERROR;
    }
    (*newCardObject)->birthday = calloc(1, sizeof(DateTime));
    if((*newCardObject)->birthday == NULL) {
        return OTHER_ERROR;
    }
    (*newCardObject)->anniversary = calloc(1, sizeof(DateTime));
    if((*newCardObject)->anniversary == NULL) {
        return OTHER_ERROR;
    }

    char tempString[1000];
    bool beginFound = false;
    bool endOfFile = false;
    bool verFound = false;
    bool fnFound = false;
    bool bdayFound = false;
    bool anniversaryFound = false;
    while(fgets(tempString, 1000, fp)) {
        char nextLine[1000];
        while(fgets(nextLine, 1000, fp)) {
            if(nextLine[0] == ' ') {
                char fixedLine[1000];
                for(int i = 1; i < strlen(nextLine)+1; i++) {
                    fixedLine[i-1] = nextLine[i];
                }
                if(strstr(tempString, "\r\n")) {
                    (tempString)[strcspn(tempString, "\r\n")] = '\0';
                } else if(strstr(tempString, "\n")) {
                    (tempString)[strcspn(tempString, "\n")] = '\0';
                }
                strcat(tempString, fixedLine);
            } else {
                long offset = (long)(strlen(nextLine) * -1);
                fseek(fp, offset, SEEK_CUR);
                break;
            }
        }

        if(strstr(tempString, "\r\n") == NULL) {
            deleteCard(*newCardObject);
            fclose(fp);
            return INV_CARD;
        }

        //gets index of ":" and ";"
        if(strstr(tempString, ":") == NULL) {
            deleteCard(*newCardObject);
            fclose(fp);
            *newCardObject = NULL;
            return INV_PROP;
        }
        int colonIndex = strcspn(tempString, ":");
        int semicolonIndex = strcspn(tempString, ";");

        char *propName;
        char *propParameters = NULL;
        char *propValue;
        if(colonIndex < semicolonIndex) {
            //stores everything up to : in a string
            propName = calloc(colonIndex+1, sizeof(char));

            for(int i = 0; i < colonIndex; i++) {
                propName[i] = tempString[i];
            }
            propName[colonIndex] = '\0';

            //stores everything after the : in a string
            int valueLen = strlen(tempString) - colonIndex;
            propValue = calloc(valueLen+1, sizeof(char));

            for(int i = 0; i < valueLen; i++) {
                propValue[i] = tempString[colonIndex + i + 1];
            }
            propValue[valueLen] = '\0';
            removeLineFeed(&propValue);
        } else {
            //stores everything up to ; in a string
            propName = calloc(semicolonIndex+1, sizeof(char));
            for(int i = 0; i < semicolonIndex; i++) {
                propName[i] = tempString[i];
            }
            propName[semicolonIndex] = '\0';

            propParameters = calloc(colonIndex-semicolonIndex, sizeof(char));

            //stores everything after the first semicolon up to the : in a string
            for(int i = semicolonIndex+1; i < colonIndex; i++) {
                propParameters[i-(semicolonIndex+1)] = tempString[i];
            }
            propParameters[colonIndex-semicolonIndex-1] = '\0';

            //stores everything after the : in a string
            int valueLen = strlen(tempString) - colonIndex;
            propValue = calloc(valueLen+1, sizeof(char));
            for(int i = 0; i < valueLen; i++) {
                propValue[i] = tempString[colonIndex + i + 1];
            }
            removeLineFeed(&propValue);
        }

        if(strcmp(propName, "") == 0 || strcmp(propValue, "") == 0) {
            free(propName);
            free(propValue);
            deleteCard(*newCardObject);
            *newCardObject = NULL;
            fclose(fp);
            return INV_PROP;
        }

        if(strcmp(propName, "BEGIN") == 0) {
            if(beginFound == true) {
                createOptionalProperty(newCardObject, propName, propParameters, propValue);
            }
            if(beginFound == false) {
                beginFound = true;  
            }
            free(propName);
            free(propValue);        
        } else if(strcmp(propName, "VERSION") == 0) {
            if(beginFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(strcmp(propValue, "4.0") != 0) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(verFound == true) {
                createOptionalProperty(newCardObject, propName, propParameters, propValue);
            }
            verFound = true;
            free(propName);
            free(propValue);
        } else if(strcmp(propName, "FN") == 0) {
            if(beginFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(verFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(fnFound == true) {
                //inserts duplicate FN as optional property
                VCardErrorCode code = createOptionalProperty(newCardObject, propName, propParameters, propValue);
                if(code != OK) {
                    fclose(fp);
                    return code;
                }
                free(propName);
                free(propValue);
            } else {
                fnFound = true;
                (*newCardObject)->fn->name = calloc(strlen(propName)+1, sizeof(char));
                if((*newCardObject)->fn->name == NULL) {
                    return OTHER_ERROR;
                }
                strcpy((*newCardObject)->fn->name, propName);
                (*newCardObject)->fn->group = calloc(1,sizeof(char));
                if((*newCardObject)->fn->group == NULL) {
                    return OTHER_ERROR;
                }
                strcpy((*newCardObject)->fn->group, "");
                parseGroupAndPropertyName(&((*newCardObject)->fn->name), &((*newCardObject)->fn->group));
                (*newCardObject)->fn->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
                if((*newCardObject)->fn->parameters == NULL) {
                    return OTHER_ERROR;
                }
                (*newCardObject)->fn->values = initializeList(&valueToString, &deleteValue, &compareValues);
                if((*newCardObject)->fn->values == NULL) {
                    return OTHER_ERROR;
                }
                char *newValue = malloc(sizeof(char)*(strlen(propValue)+1));
                strcpy(newValue, propValue);
                insertFront((*newCardObject)->fn->values, (void*)newValue);
                free(propName);
                free(propValue);
            }
        } else if(strcmp(propName, "BDAY") == 0 || strcmp(propName, "ANNIVERSARY") == 0) {
            if(beginFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(verFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(fnFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            
            if(strcmp(propName, "BDAY") == 0) {
                if(bdayFound == true) {
                    //create a new property object and insert it into optional properties
                    createOptionalProperty(newCardObject, propName, propParameters, propValue);
                }
                if(bdayFound == false) {
                    bool *bdayUTC = &((*newCardObject)->birthday->UTC);
                    bool *bdayIsText = &((*newCardObject)->birthday->isText);
                    char **bdayDate = &((*newCardObject)->birthday->date);
                    char **bdayTime = &((*newCardObject)->birthday->time);
                    char **bdayText = &((*newCardObject)->birthday->text);
                    createDateTimeProperty(propValue, bdayUTC, bdayIsText, bdayDate, bdayTime, bdayText, propParameters);
                    if(*bdayIsText == false && strcmp(*bdayDate, "") == 0 && strcmp(*bdayTime, "") == 0 && strcmp(*bdayText, "") == 0) {
                        deleteCard(*newCardObject);
                        *newCardObject = NULL;
                        fclose(fp);
                        return INV_PROP;
                    }
                }
                if(propParameters != NULL) {
                    free(propParameters);
                }
                bdayFound = true;
            } else {
                if(anniversaryFound == true) {
                    //create a new property object and insert it into optional properties
                    createOptionalProperty(newCardObject, propName, propParameters, propValue);
                }
                if(anniversaryFound == false) {
                    bool *anniversaryUTC = &((*newCardObject)->anniversary->UTC);
                    bool *anniversaryIsText = &((*newCardObject)->anniversary->isText);
                    char **anniversaryDate = &((*newCardObject)->anniversary->date);
                    char **anniversaryTime = &((*newCardObject)->anniversary->time);
                    char **anniversaryText = &((*newCardObject)->anniversary->text);
                    createDateTimeProperty(propValue, anniversaryUTC, anniversaryIsText, anniversaryDate, anniversaryTime, anniversaryText, propParameters);
                    if(*anniversaryIsText == false && strcmp(*anniversaryDate, "") == 0 && strcmp(*anniversaryTime, "") == 0 && strcmp(*anniversaryText, "") == 0) {
                        deleteCard(*newCardObject);
                        *newCardObject = NULL;
                        fclose(fp);
                        return INV_PROP;
                    }
                }
                anniversaryFound = true;
                if(propParameters != NULL) {
                    free(propParameters);
                }
            }
            free(propName);
            free(propValue);
        } else if(strcmp(propName, "END") == 0) {
            if(beginFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(verFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(fnFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(feof(fp)) {

            } else {
                createOptionalProperty(newCardObject, propName, propParameters, propValue);
            }
            endOfFile = true;
            free(propName);
            free(propValue);
        } else {
            if(beginFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(verFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            if(fnFound == false) {
                deleteCard((*newCardObject));
                *newCardObject = NULL;
                free(propName);
                free(propValue);
                fclose(fp);
                return INV_CARD;
            }
            VCardErrorCode code = createOptionalProperty(newCardObject, propName, propParameters, propValue);
            if(code != OK) {
                fclose(fp);
                return code;
            }
            free(propName);
            free(propValue);
        }
    }

    if(endOfFile == false) {
        deleteCard((*newCardObject));
        *newCardObject = NULL;
        fclose(fp);
        return INV_CARD;
    }
    if(bdayFound == false) {
        free(((*newCardObject)->birthday));
        (*newCardObject)->birthday = NULL;
    }
    if(anniversaryFound == false) {
        free(((*newCardObject)->anniversary));
        (*newCardObject)->anniversary = NULL;
    }
    fclose(fp);
    return OK;
}

void deleteCard(Card* obj) {
    if(!obj) {
        return;
    }
    deleteProperty((void*)obj->fn);
    freeList((void*)obj->optionalProperties);
    deleteDate((void*)obj->birthday);
    deleteDate((void*)obj->anniversary);
    free(obj);
}

char* cardToString(const Card* obj) {
    // if(obj == NULL) {
    //     return NULL;
    // }

    // //BEGIN and VERSION
    // char *intro = calloc(29, sizeof(char));
    // strcpy(intro, "BEGIN:VCARD\r\nVERSION:4.0\r\n");

    // //FN
    // char *fn = propertyToString(obj->fn);
    // fn = realloc(fn, strlen(fn)+3);
    // strcat(fn, "\r\n");

    // //BDAY
    // char *bday = calloc(6, sizeof(char));
    // strcpy(bday, "BDAY:");
    // if(obj->birthday != NULL) {
    //     char *bdayToString = dateToString(obj->birthday);
    //     bday = realloc(bday, strlen(bdayToString)+strlen(bday)+3);
    //     strcat(bday, bdayToString);
    //     free(bdayToString);
    // }

    // //ANNIVERSARY
    // char *anniversary = calloc(13, sizeof(char));
    // strcpy(anniversary, "ANNIVERSARY:");
    // if(obj->anniversary != NULL) {
    //     char *annivToString = dateToString(obj->anniversary);
    //     anniversary = realloc(anniversary, strlen(annivToString)+strlen(anniversary)+3);
    //     strcat(anniversary, annivToString);
    //     free(annivToString);
    // }

    // //END
    // char *end = calloc(12, sizeof(char));
    // strcpy(end, "END:VCARD\r\n");

    // //FINAL
    // int length = strlen(intro) + strlen(fn) + strlen(end) + 1;
    // if(obj->birthday != NULL) {
    //     length += strlen(bday);
    // }
    // if(obj->anniversary != NULL) {
    //     length += strlen(anniversary);
    // }
    
    // char *ret = calloc(length, sizeof(char));
    // strcat(ret, intro);
    // strcat(ret, fn);
    // if(obj->birthday != NULL) {
    //     strcat(ret, bday);
    // }
    // if(obj->anniversary != NULL) {
    //     strcat(ret, anniversary);
    // }
    // strcat(ret, end);
    // free(intro);
    // free(fn);
    // if(obj->birthday != NULL) {
    //     free(bday);
    // }
    // if(obj->anniversary != NULL) {
    //     free(anniversary);
    // }
    // free(end);
    char * ret = calloc(14, sizeof(char));
    strcpy(ret, "Hello world!\n");
    return ret;
}

char* errorToString(VCardErrorCode err) {
    char *toReturn;
    char *string;
    if(err == OK) {
        string = "OK";
        toReturn = calloc(strlen(string)+1, sizeof(char));
        strcpy(toReturn, string);
        return toReturn;
    } else if(err == INV_FILE) {
        string = "INV_FILE";
        toReturn = calloc(strlen(string)+1, sizeof(char));
        strcpy(toReturn, string);
        return toReturn;
    } else if(err == INV_PROP) {
        string = "INV_PROP";
        toReturn = calloc(strlen(string)+1, sizeof(char));
        strcpy(toReturn, string);
        return toReturn;
    } else if(err == INV_CARD) {
        string = "INV_CARD";
        toReturn = calloc(strlen(string)+1, sizeof(char));
        strcpy(toReturn, string);
        return toReturn;
    } else if(err == INV_DT) {
        string = "INV_DT";
        toReturn = calloc(strlen(string)+1, sizeof(char));
        strcpy(toReturn, string);
        return toReturn;
    } else if(err == WRITE_ERROR) {
        string = "WRITE_ERROR";
        toReturn = calloc(strlen(string)+1, sizeof(char));
        strcpy(toReturn, string);
        return toReturn;
    } else if(err == OTHER_ERROR) {
        string = "OTHER_ERROR";
        toReturn = calloc(strlen(string)+1, sizeof(char));
        strcpy(toReturn, string);
        return toReturn;
    }
    return NULL;
}

VCardErrorCode writeCard(const char* fileName, const Card* obj) {
    if(obj == NULL) {
        return WRITE_ERROR;
    }
    if(fileName == NULL) {
        return WRITE_ERROR;
    }
    char vcfExtension[5];
    vcfExtension[4] = '\0';
    int j = 1;
    for(int i = 3; i >= 0; i--) {
        vcfExtension[i] = fileName[strlen(fileName)-j];
        j++;
    }
    j = 1;

    //looks for .vcard extension
    char vcardExtension[7];
    vcardExtension[6] = '\0';
    for(int i = 5; i >= 0; i--) {
        vcardExtension[i] = fileName[strlen(fileName)-j];
        j++;
    }

    if(strcmp(vcfExtension, ".vcf") != 0 && strcmp(vcardExtension, ".vcard") != 0) {
        return WRITE_ERROR;
    }

    FILE *fp = fopen(fileName, "w");
    fclose(fp);
    fp = fopen(fileName, "a");
    if(fp == NULL) {
        return OTHER_ERROR;
    }

    fprintf(fp, "BEGIN:VCARD\r\n");
    fprintf(fp, "VERSION:4.0\r\n");

    char *propertyString = propertyToString(obj->fn);
    fprintf(fp, "%s\r\n", propertyString);
    free(propertyString);

    if(obj->birthday != NULL) {
        fprintf(fp, "BDAY");
        if(obj->birthday->isText == true) {
            fprintf(fp, ";VALUE=text");
        }
        char *dateString = dateToString(obj->birthday);
        fprintf(fp, ":%s\r\n", dateString);
        free(dateString);
    }
    if(obj->anniversary != NULL) {
        fprintf(fp, "ANNIVERSARY");
        if(obj->anniversary->isText == true) {
            fprintf(fp, ";VALUE=text");
        }
        char *anniversaryString =  dateToString(obj->anniversary);
        fprintf(fp, ":%s\r\n", anniversaryString);
        free(anniversaryString);
    }

    ListIterator optionalPropIter = createIterator(obj->optionalProperties);
    Property *currProp = (Property*)nextElement(&optionalPropIter);

    while(currProp != NULL) {
        propertyString = propertyToString(currProp);
        fprintf(fp, "%s\r\n", propertyString);
        free(propertyString);
        currProp = (Property*)nextElement(&optionalPropIter);
    }

    fprintf(fp, "END:VCARD\r\n");

    fclose(fp);
    return OK;
}

VCardErrorCode validateCard(const Card* obj) {
    if(obj == NULL) {
        return INV_CARD;
    }

    VCardErrorCode code;
    code = validateFN(obj->fn);
    if(code != OK) {
        return code;
    }
    code = validateOptionalProperties(obj->optionalProperties);
    if(code != OK) {
        return code;
    }
    code = validateOptionalPropertyNames(obj->optionalProperties);
    if(code != OK) {
        return code;
    }
    code = validateDateTime(obj->birthday);
    if(code != OK) {
        return code;
    }
    code = validateDateTime(obj->anniversary);
    if(code != OK) {
        return code;
    }
    return OK;
}

void deleteProperty(void* toBeDeleted) {
    if(!toBeDeleted) {
        return;
    }
    Property *temp = (Property*)toBeDeleted;
    free(temp->name);
    free(temp->group);
    freeList(temp->parameters);
    freeList(temp->values);
    free(toBeDeleted);
}

int compareProperties(const void* first,const void* second) {
    Property *a = (Property*)first;
    Property *b = (Property*)second;
    if(strcmp(a->name, b->name) != 0) {
        return strcmp(a->name, b->name);
    }
    if(strcmp(a->group, b->group) != 0) {
        return strcmp(a->group, b->group);
    }

    ListIterator iter1 = createIterator(a->parameters);
    ListIterator iter2 = createIterator(b->parameters);
    Parameter *param1 = (Parameter*)nextElement(&iter1);
    Parameter *param2 = (Parameter*)nextElement(&iter2);

    while(param1 != NULL && param2 != NULL) {
        if(strcmp(param1->name, param2->name) != 0) {
            return strcmp(param1->name, param2->name);
        }
        if(strcmp(param1->value, param2->value) != 0) {
            return strcmp(param1->value, param2->value);
        }
        param1 = (Parameter*)nextElement(&iter1);
        param2 = (Parameter*)nextElement(&iter2);
    }

    iter1 = createIterator(a->values);
    iter2 = createIterator(b->values);
    char *value1 = (char*)nextElement(&iter1);
    char *value2 = (char*)nextElement(&iter2);

    while(value1 != NULL && value2 != NULL) {
        if(strcmp(value1, value2) != 0) {
            return strcmp(value1, value2);
        }
        value1 = (char*)nextElement(&iter1);
        value2 = (char*)nextElement(&iter2);
    }
    return 0;
}

char* propertyToString(void* prop) {
    Property *property = (Property*)prop;
    int length = strlen(property->name);
    if(strcmp(property->group, "") != 0) {
        length += strlen(property->group)+1;
    }
    ListIterator parameterIterator = createIterator(property->parameters);
    ListIterator valueIterator = createIterator(property->values);
    Parameter *param = (Parameter*)nextElement(&parameterIterator);
    while(param != NULL) {
        length += strlen(param->name) + strlen(param->value)+2;
        param = (Parameter*)nextElement(&parameterIterator);
    }

    char *value = (char*)nextElement(&valueIterator);
    while(value != NULL) {
        length += strlen(value)+1;
        value = (char*)nextElement(&valueIterator);
    }

    char *ret = calloc(length+1, sizeof(char));
    if(strcmp(property->group, "") != 0) {
        strcpy(ret, property->group);
        strcat(ret, ".");
    }
    strcat(ret, property->name);

    parameterIterator = createIterator(property->parameters);
    valueIterator = createIterator(property->values);

    param = (Parameter*)nextElement(&parameterIterator);
    char *parameterString;
    while(param != NULL) {
        strcat(ret, ";");
        parameterString = parameterToString(param);
        strcat(ret, parameterString);
        free(parameterString);
        param = (Parameter*)nextElement(&parameterIterator);
    }

    int count = 0;
    value = (char*)nextElement(&valueIterator);
    while(value != NULL) {
        if(count == 0) {
            strcat(ret, ":");
        } else {
            strcat(ret, ";");
        }
        strcat(ret, value);
        value = (char*)nextElement(&valueIterator);
        count++;
    }
    return ret;
}

void deleteParameter(void* toBeDeleted) {
    Parameter *tempParam;

    if(!toBeDeleted) {
        return;
    }

    tempParam = (Parameter *)toBeDeleted;

    free(tempParam->name);
    free(tempParam->value);
    free(tempParam);
}

int compareParameters(const void* first,const void* second) {
    Parameter *a = (Parameter*)first;
    Parameter *b = (Parameter*)second;
    return strcmp(a->name, b->name) + strcmp(a->value, b->value);
}

char* parameterToString(void* param) {
    if(param == NULL) {
        char *empty = calloc(1, sizeof(char));
        strcpy(empty, "");
        return empty;
    }
    Parameter *tempParam = (Parameter *)param;
    int len = strlen(tempParam->name) + strlen(tempParam->value) + 4;
    char *result = calloc(len, sizeof(char));

    sprintf(result, "%s=%s", tempParam->name, tempParam->value);
    return result;
}

void deleteValue(void* toBeDeleted) {
    free((char*)toBeDeleted);
}

int compareValues(const void* first,const void* second) {
    return 0;
}

char* valueToString(void* val) {
    return (char*)val;
}

void deleteDate(void* toBeDeleted) {
    if(!toBeDeleted) {
        return;
    }

    DateTime *tmp = (DateTime*)toBeDeleted;
    free(tmp->date);
    free(tmp->time);
    free(tmp->text);
    free(tmp);
}

int compareDates(const void* first,const void* second) {
    DateTime *a = (DateTime*)first;
    DateTime *b = (DateTime*)second;

    char *date1 = a->date;
    char *date2 = b->date;
    char *time1 = a->time;
    char *time2 = b->time;
    char *text1 = a->text;
    char *text2 = b->text;
    if(strcmp(date1, date2) == 0 && strcmp(time1, time2) == 0 && strcmp(text1, text2) == 0) {
        return 0;
    } else {
        return 1;
    }
}

char* dateToString(void* date) {
    if(date == NULL) {
        char *empty = calloc(1, sizeof(char));
        strcpy(empty, "");
        return empty;
    }

    DateTime *theDate = (DateTime*)date;
    //for \0
    int length = 1;
    //size of the text
    if(strcmp(theDate->text, "") != 0) {
        length += strlen(theDate->text);
    }
    //size of date string
    if(strcmp(theDate->date, "") != 0) {
        length += strlen(theDate->date);
    }
    //size of time string + T
    if(strcmp(theDate->time, "") != 0) {
        length += strlen(theDate->time) + 1;
    }
    //for Z
    if(theDate->UTC == true) {
        length += 1;
    }
    char *ret = calloc(length, sizeof(char));
    if(theDate->isText == true) {
        strcpy(ret, theDate->text);
    } else {
        if(strcmp(theDate->date, "") != 0) {
            strcpy(ret, theDate->date);
        }
        if(strcmp(theDate->time, "") != 0) {
            strcat(ret, "T");
            strcat(ret, theDate->time);
        }
        if(theDate->UTC == true) {
            strcat(ret, "Z");
        }
    }
    return ret;
}

char* strListToJSON(const List* strList) {
    if(strList == NULL) {
        return NULL;
    }
    
    char *ret = NULL;
    List *copy = initializeList(&valueToString, &deleteValue, &compareValues);
    deepCopyList(&copy, strList);

    if(getLength(copy) == 0) {
        ret = calloc(3, sizeof(char));
        strcpy(ret, "[]");
    } else {
        int length = getLength(copy);
        int stringLength = 3 + (length *2) + (length-1);
        ListIterator iter = createIterator(copy);
        char *curr = (char*)nextElement(&iter);

        //get JSON string length
        while(curr != NULL) {
            stringLength += strlen(curr);
            curr = (char*)nextElement(&iter);
        }
        ret = calloc(stringLength, sizeof(char));
        
        //creates the JSON string
        strcat(ret, "[");
        iter = createIterator(copy);
        int counter = 0;
        char *value = (char*)nextElement(&iter);

        while(value != NULL) {
            strcat(ret, "\"");
            strcat(ret, value);
            strcat(ret, "\"");
            if(counter < length-1) {
                strcat(ret, ",");
            }
            counter++;
            value = (char*)nextElement(&iter);
        }
        strcat(ret, "]");
    }
    freeList(copy);
    return ret;
}
List* JSONtoStrList(const char* str) {
    if(str == NULL) {
        return NULL;
    }
    if(strcmp(str, "") == 0) {
        return NULL;
    }

    //finds the quotations
    int numQuotations = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == '\"') {
            numQuotations++;
        }
    }

    if(numQuotations % 2 != 0) {
        return NULL;
    }

    //marks the location of the indexes of the quotations
    int quotationIndexes[numQuotations];
    int currIndex = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == '\"') {
            quotationIndexes[currIndex] = i;
            currIndex++;
        }
    }

    //creates the string list by parsing the quotation marks
    List *ret = initializeList(&valueToString, &deleteValue, &compareValues);
    for(int i = 0; i < numQuotations; i+=2) {
        int first = quotationIndexes[i];
        int second = quotationIndexes[i+1];
        int size = second-first;
        char *value = calloc(size, sizeof(char));
        for(int j = quotationIndexes[i]+1; j < quotationIndexes[i+1]; j++) {
            value[j-(quotationIndexes[i]+1)] = str[j];
        }
        value[size-1] = '\0';
        insertBack(ret, (void*)value);
    }
    return ret;
}
char* propToJSON(const Property* prop) {
    if(prop == NULL) {
        char *ret;
        ret = calloc(1, sizeof(char));
        strcpy(ret, "");
        return ret;
    }
    char *ret;
    char *groupString = nameValueToJSON("group", prop->group);
    char *nameString = nameValueToJSON("name", prop->name);
    char *valueListString = strListToJSON(prop->values);
    char *valueString = calloc(10+strlen(valueListString), sizeof(char));
    strcpy(valueString, "\"values\":");
    strcat(valueString, valueListString);
    int retSize = strlen(groupString)+strlen(nameString)+strlen(valueString);
    ret = calloc(5+retSize, sizeof(char));
    strcat(ret, "{");
    strcat(ret, groupString);
    strcat(ret, ",");
    strcat(ret, nameString);
    strcat(ret, ",");
    strcat(ret, valueString);
    strcat(ret, "}");
    
    return ret;
}

Property* JSONtoProp(const char* str) {
    if(str == NULL) {
        return NULL;
    }

    if(strcmp(str, "") == 0) {
        return NULL;
    }

    //finds the quotations
    int numQuotations = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == '\"') {
            numQuotations++;
        }
    }

    if(numQuotations % 2 != 0) {
        return NULL;
    }

    //find colons
    int numColons = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ':') {
            numColons++;
        }
    }

    //record colon locations
    int colonIndexes[numColons];
    int currIndex = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == ':') {
            colonIndexes[currIndex] = i;
            currIndex++;
        }
    }

    //find value lengths except values string
    int stringLengths[numColons-1];
    for(int i = 0; i < numColons-1; i++) {
        stringLengths[i] = 0;
    }

    for(int i = 0; i < numColons-1; i++) {
        for(int j = colonIndexes[i]+2; j < colonIndexes[i+1]; j++) {
            if(str[j] != '\"') {
                stringLengths[i]++;
            } else {
                break;
            }
        }
    }

    //stores the values into strings except for values string
    char *strings[numColons-1];
    for(int i = 0; i < numColons-1; i++) {
        strings[i] = calloc(stringLengths[i]+1, sizeof(char));
        for(int j = colonIndexes[i]+2; j < colonIndexes[i+1]; j++) {
            if(str[j] == '\"') {
                strings[i][stringLengths[i]] = '\0';
                break;
            } else {
                strings[i][j-(colonIndexes[i]+2)] = str[j];
            }
        }
    }

    //creates the values string
    int valueStringLen = 0;
    for(int i = colonIndexes[numColons-1]+1; i < strlen(str); i++) {
        if(str[i] != '}') {
            valueStringLen++;
        }
    }

    char *valueString = calloc(valueStringLen+1, sizeof(char));
    for(int i = colonIndexes[numColons-1]+1; i < strlen(str); i++) {
        if(str[i] != '}') {
            valueString[i-(colonIndexes[numColons-1]+1)] = str[i];
        } else {
            valueString[i-(colonIndexes[numColons-1]+1)] = '\0';
            break;
        }
    }
    
    
    Property *prop = calloc(1, sizeof(Property));
    prop->group = calloc(stringLengths[0]+1, sizeof(char));
    strcpy(prop->group, strings[0]);
    prop->name = calloc(stringLengths[1]+1, sizeof(char));
    strcpy(prop->name, strings[1]);
    prop->values = JSONtoStrList(valueString);
    prop->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);

    for(int i = 0; i < numColons-1; i++) {
        free(strings[i]);
    }
    free(valueString);
    return prop;
}
char* dtToJSON(const DateTime* prop) {
    if(prop == NULL) {
        char * ret = calloc(1, sizeof(char));
        strcpy(ret, "");
        return ret;
    }
    char *isTextString = nameBooleanToJSON("isText", prop->isText);
    char *dateString = nameValueToJSON("date", prop->date);
    char *timeString = nameValueToJSON("time", prop->time);
    char *textString = nameValueToJSON("text", prop->text);
    char *isUTCString = nameBooleanToJSON("isUTC", prop->UTC);
    int length = 7+strlen(isTextString)+strlen(dateString)+strlen(timeString)+strlen(textString)+strlen(isUTCString);
    char *ret = calloc(length, sizeof(char));

    strcat(ret, "{");
    strcat(ret, isTextString);
    strcat(ret, ",");
    strcat(ret, dateString);
    strcat(ret, ",");
    strcat(ret, timeString);
    strcat(ret, ",");
    strcat(ret, textString);
    strcat(ret, ",");
    strcat(ret, isUTCString);
    strcat(ret, "}");

    free(isTextString);
    free(dateString);
    free(timeString);
    free(textString);
    free(isUTCString);
    return ret;
}
DateTime* JSONtoDT(const char* str) {
    if(str == NULL) {
        return NULL;
    }

    if(strcmp(str, "") == 0) {
        return NULL;
    }

    int numQuotations = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == '\"') {
            numQuotations++;
        }
    }
    if(numQuotations % 2 != 0) {
        return NULL;
    }
    char *copy = calloc(strlen(str)+1, sizeof(char));
    strcpy(copy, str);
    List *list = splitJSONStringByComma(copy);
    ListIterator iter = createIterator(list);
    char *currSection = (char*)nextElement(&iter);
    char *currValue = extractValue(currSection);
    DateTime *ret = calloc(1, sizeof(DateTime));
    
    //isText
    if(strcmp(currValue, "true") == 0) {
        ret->isText = true;
    } else {
        ret->isText = false;
    }
    free(currValue);

    //date
    currSection = (char*)nextElement(&iter);
    currValue = extractValue(currSection);
    ret->date = calloc(strlen(currValue)+1, sizeof(char));
    strcpy(ret->date, currValue);
    free(currValue);

    //time
    currSection = (char*)nextElement(&iter);
    currValue = extractValue(currSection);
    ret->time = calloc(strlen(currValue)+1, sizeof(char));
    strcpy(ret->time, currValue);
    free(currValue);

    //text
    currSection = (char*)nextElement(&iter);
    currValue = extractValue(currSection);
    ret->text = calloc(strlen(currValue)+1, sizeof(char));
    strcpy(ret->text, currValue);
    free(currValue);

    //UTC
    currSection = (char*)nextElement(&iter);
    currValue = extractValue(currSection);
    if(strcmp(currValue, "true") == 0) {
        ret->UTC = true;
    } else {
        ret->UTC = false;
    }
    free(currValue);
    free(copy);
    freeList(list);

    return ret;
}
Card* JSONtoCard(const char* str) {
    if(str == NULL) {
        return NULL;
    }

    if(strcmp(str, "") == 0) {
        return NULL;
    }

    int numQuotations = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == '\"') {
            numQuotations++;
        }
    }
    if(numQuotations % 2 != 0) {
        return NULL;
    }

    char *copy = calloc(strlen(str)+1, sizeof(char));
    strcpy(copy, str);
    int currIndex = 0;
    char *noBrackets = calloc(strlen(str)-1, sizeof(char));
    for(int i = 0; i < strlen(str); i++) {
        if(copy[i] != '{' && copy[i] != '}') {
            noBrackets[currIndex] = copy[i];
            currIndex++;
        }
    }
    noBrackets[currIndex] = '\0';
    char *fnValue = extractValue(noBrackets);

    Card *ret = calloc(1, sizeof(Card));
    ret->fn = calloc(1, sizeof(Property));
    ret->fn->name = calloc(3, sizeof(char));
    strcpy(ret->fn->name, "FN");
    ret->fn->group = calloc(1, sizeof(char));
    strcpy(ret->fn->group, "");
    ret->fn->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
    ret->fn->values = initializeList(&valueToString, &deleteValue, &compareValues);
    insertBack(ret->fn->values, (void*)fnValue);

    ret->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties);

    free(noBrackets);
    free(copy);
    return ret;
} 
void addProperty(Card* card, const Property* toBeAdded) {
    if(card == NULL) {
        return;
    }
    if(toBeAdded == NULL) {
        return;
    }
    insertBack(card->optionalProperties, (void*)toBeAdded);
}