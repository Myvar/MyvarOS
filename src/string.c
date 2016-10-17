#include <main.h>

int strlen(const char *str)
{
    int retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

char *strcpy(char *s1, const char *s2)
{
    char *s1_p = s1;
    while ((*s1++ = *s2++));
    return s1_p;
}

char *strcpyc(char *s1, char s2)
{
    char *s1_p = s1;
    *s1++ = s2;
    return s1_p;
}

int strcmp(char *a, char *b)
{
    while (*a && *b && *a == *b) { ++a; ++b; }
    return *a - *b;
}

void strappend(char* s, char c)
{

    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

void strremovelast(char* s)
{

    int len = strlen(s);
    s[len - 1] = '\0';
}

char *strclamp(char *str)
{
    char tmp[strlen(str)];

    strcpy(tmp, str);

    return tmp;
}

char *strdup(const char *str)
{
    char *result = kmalloc(strlen(str) + 1);
    char *psrc = str;
    char *pdst = result;
    while (*psrc != 0)
    {
        *pdst = *psrc;
        pdst++;
        psrc++;
    }
    *pdst = '\0';
    return result;
}

char* strtok(char* s, char* delm)
{
    static int currIndex = 0;
    if(!s || !delm || s[currIndex] == '\0')
    return -1;
    char *W = (char *)kmalloc(sizeof(char)*100);
    int i = currIndex, k = 0, j = 0;
    //char *ptr;
    //static char *Iterator = s;
    //ptr = s;

    /*if (s == NULL){
    s = Iterator;
    }*/
    while (s[i] != '\0'){
        j = 0;
        while (delm[j] != '\0'){
            if (s[i] != delm[j])
                W[k] = s[i];
            else goto It;
            j++;
        }
        //ptr++;
        i++;
        k++;
    }
It:
    W[i] = 0;
    currIndex = i+1;
    //Iterator = ++ptr;
    return W;
}

char ** strsplit(char* a_str, char a_delim)
{
    char** result    = 0;
    int count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = kmalloc(sizeof(char*) * count);

    if (result)
    {
        int idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            //assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        //assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}