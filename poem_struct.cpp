#include <stdio.h>
#include <ctype.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <errno.h>

#define MAXNAMELEN 20
#define SEPARATE "-------------------------------------------------\n"
#define NEWFILE 0
#define ADDFILE 1


const char Res_name[] = "sorted_poem.txt";


//-----------------------------------------------------------------------------
//! Contains a pointer to the buffer of read characters and arrays of pointers
//! to the first and last letters of each string
//-----------------------------------------------------------------------------


struct String
    {
    char* bgn;      //! pointer to the beginning of string
    char* end;      //! pointer to the ending of string
    };



//-----------------------------------------------------------------------------
//! Prints an introduction and requests the name of the file
//!
//! @param [out]     file_name        name of poem's file
//-----------------------------------------------------------------------------


void IntroPrint(char file_name[MAXNAMELEN]);



//-----------------------------------------------------------------------------
//! Reads a poem from the txt file and puts it in buffer, saves
//! pointers to the first and to the last letters of each string in an array
//!
//! @param [in]      File_name        name of the poem's file
//! @param [out]     strings poem     poem's data structure
//!
//! @return Returns number of strings in the poem if it was successful
//-----------------------------------------------------------------------------


char* ReadFile(size_t* len, const char File_name[]);

void DeleteSpaces(char* buf, size_t len);

//-----------------------------------------------------------------------------
//! This is a subfunction of ReadFile that saves
//! pointers to the first and to the last letters of each string in an array
//!
//! @param [out]     strings poem poem's data structure
//!
//! @return Returns number of strings in the poem
//-----------------------------------------------------------------------------


struct String* FindStrings(char* buf, size_t* num_str);


//-----------------------------------------------------------------------------
//! Sorts the strings in ascending order of its first letter's code with bubble
//! sort algorithm
//!
//! @param [in]     Num_str         number of strings in the poem
//! @param [in]     strings poem    poem's data structure
//-----------------------------------------------------------------------------


void SortByBgn(struct String* strings, size_t num_str);



//-----------------------------------------------------------------------------
//! Sorts the strings in ascending order of its last letter's code with bubble
//! sort algorithm
//!
//! @param [in]     Num_str         number of strings in the poem
//! @param [in]     strings poem    poem's data structure
//-----------------------------------------------------------------------------


void SortByEnd(struct String* strings, size_t num_str);



//-----------------------------------------------------------------------------
//! Creates the txt file of sorted strings or adds it to the end of existing file
//!
//! @param [in]     Res_name         name of the result's file
//! @param [in]     strings poem     poem's data structure
//! @param [in]     Num_str          number of strings in the poem
//! @param [in]     add              0 if it has to create new file and 1 if it
//!                                  has to add text to the existing file
//-----------------------------------------------------------------------------


size_t CreateFile(struct String* strings, const char Res_name[], size_t num_str, bool add);



//-----------------------------------------------------------------------------
//! Makes two pointers to exchange its values
//-----------------------------------------------------------------------------


void Swap(struct String* str1, struct String* str2);


int main()
    {
    setlocale(LC_ALL, "Rus");

    char file_name[MAXNAMELEN] = "";
    size_t len = 0;
    size_t num_str = 0;

    IntroPrint(file_name);

    char* buf = ReadFile(&len, file_name);
    if (buf == NULL)
        return EACCES;

    DeleteSpaces(buf, len);
    struct String* strings = FindStrings(buf, &num_str);

    SortByBgn(strings, num_str);

    size_t err = CreateFile(strings, Res_name, num_str, NEWFILE);
    if (err == EACCES)
        return EACCES;

    SortByEnd(strings, num_str);

    err = CreateFile(strings, Res_name, num_str, ADDFILE);
    if (err == EACCES)
        return EACCES;

    printf("Sorting was done successfully. Check the file '%s' in the program's directory\n", Res_name);

    free(buf);
    free(strings);

    return 0;
    }



void IntroPrint(char file_name[MAXNAMELEN])
    {
    printf("<<English poem sorter>>\nPlease, enter the file name: ");
    scanf("%s", file_name);

    while ((strlen(file_name) > MAXNAMELEN) || (access(file_name, 0) == -1))
        {
        if (strlen(file_name) > MAXNAMELEN)
            printf("File name is too long. Enter again: ");

        else
            printf("File '%s' was not found. Enter again: ", file_name);

        fflush(stdin);
        scanf("%s", file_name);
        }
    }


char* ReadFile(size_t* len, const char File_name[])
    {
    if (access(File_name, 3) == -1)
        {
        printf("Reading error: file '%s' reading access denied\n", File_name);
        return NULL;
        }

    FILE* file = NULL;
    file = fopen(File_name, "r");

    fseek(file, 0, SEEK_END);
    size_t num_char = ftell(file) + 1;
    fseek(file, 0, SEEK_SET);

    char* buf = (char*)calloc(num_char + 1, sizeof(char));
    fread(buf, sizeof(char), num_char, file);

    fclose(file);

    *len = num_char;
    return buf;
    }


void DeleteSpaces(char* buf, size_t len)
    {
    size_t end_ltr = len - 1;
    while (isspace(buf[end_ltr]))
        end_ltr--;

    buf[end_ltr+1] = '\n';
    buf[end_ltr+2] = '\0';
    }


struct String* FindStrings(char* buf, size_t* num_str)
    {
    size_t letter = 0;

    // count strings number
    for (letter = 0; buf[letter] != '\0'; letter++)
        if ((buf[letter] == '\n') && (buf[letter-1] != '\n'))
            (*num_str)++;

    struct String* strings = (struct String*)calloc(*num_str, sizeof(struct String));
    strings[0].bgn = buf;

    size_t str_ind = 1;
    size_t buf_ind = 1;

    while (buf[buf_ind] != '\0')
        {
        if (buf[buf_ind-1] == '\n')
            {
            strings[str_ind].bgn = buf + buf_ind;

            // skip punctuation
            letter = buf_ind-2;
            if (buf[letter] != '\n')
                {
                while (!isalpha(buf[letter]))
                    letter--;
                }

            strings[str_ind-1].end = buf + letter;
            str_ind++;
            }

        buf_ind++;
        }

    // skip punctuation
    letter = buf_ind-1;
    while (!isalpha(buf[letter]))
        letter--;

    strings[str_ind-1].end = buf + letter;

    return strings;
    }



void SortByBgn(struct String* strings, size_t num_str)
    {
    for (size_t k = 0; k < num_str-1; k++)
        {
        for (size_t i = 0; i < num_str-1; i++)
            {
            if (*(strings[i].bgn) > *(strings[i+1].bgn))
                Swap(strings+i, strings+i+1);

            else if (*(strings[i].bgn) == *(strings[i+1].bgn))
                {
                // if strings have same beginnings
                size_t next_letter = 1;
                while (*(strings[i].bgn + next_letter) == *(strings[i+1].bgn + next_letter))
                    next_letter++;

                if (*(strings[i].bgn + next_letter) > *(strings[i+1].bgn + next_letter))
                    Swap(strings+i, strings+i+1);
                }
            }
        }
    }



void SortByEnd(struct String* strings, size_t num_str)
    {
    for (size_t k = 0; k < num_str-1; k++)
        {
        for (size_t i = 0; i < num_str-1; i++)
            {
            if (*(strings[i].end) > *(strings[i+1].end))
                Swap(strings+i, strings+i+1);

            else if (*(strings[i].end) == *(strings[i+1].end))
                {
                // if strings have same ending
                size_t prev_letter = 1;
                while (*(strings[i].end - prev_letter) == *(strings[i+1].end - prev_letter))
                    prev_letter++;

                if (*(strings[i].end - prev_letter) > *(strings[i+1].end - prev_letter))
                    Swap(strings+i, strings+i+1);
                }
            }
        }
    }



size_t CreateFile(struct String* strings, const char Res_name[], size_t num_str, bool add)
    {
    if (access (Res_name, 2) == -1)
        {
        printf("Writing error: file '%s' writing access denied");
        return EACCES;
        }

    FILE* file = NULL;

    if (!add)
        file = fopen(Res_name, "w");
    else
        file = fopen(Res_name, "a");

    fputs(SEPARATE, file);

    for (size_t str = 0; str < num_str; str++)
        {
        fwrite(strings[str].bgn, sizeof(char), strings[str].end - strings[str].bgn + 1, file);
        fputc('\n', file);
        }

    fclose(file);
    return 0;
    }



void Swap(struct String* str1, struct String* str2)
    {
    char* help = str1->bgn;
    str1->bgn = str2->bgn;
    str2->bgn = help;

    help = str1->end;
    str1->end = str2->end;
    str2->end = help;
    }

