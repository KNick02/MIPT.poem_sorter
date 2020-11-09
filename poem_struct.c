#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
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
//! Contains pointers to the first and last letter, to the '\n' symbol of string
//-----------------------------------------------------------------------------


struct String
{
    char* bgn;      //! pointer to the beginning of string
    char* end_ltr;  //! pointer to the last letter of string
    char* nsym;     //! pointer to the '\n' symbol at the end of string
};



//-----------------------------------------------------------------------------
//! Prints an introduction and requests the name of the file
//!
//! @param [out]     file_name        name of poem's file
//-----------------------------------------------------------------------------


void IntroPrint(char file_name[MAXNAMELEN]);



//-----------------------------------------------------------------------------
//! Reads a poem from the txt file, counts the number of symbols inside
//! and puts it in buffer
//!
//! @param [in]      File_name        name of the poem's file
//! @param [out]     len              number of symbols in the file
//!
//! @return Returns pointer to the buffer
//-----------------------------------------------------------------------------


char* ReadFile(size_t* len, const char File_name[]);



//-----------------------------------------------------------------------------
//! Deletes all extra symbols (space, tab, enter) from the end_ltr of buffer
//!
//! @param [in]      buf              pointer to the buffer
//! @param [in]      len              number of symbols in the file
//-----------------------------------------------------------------------------


void DeleteSpaces(char* buf, size_t len);



//-----------------------------------------------------------------------------
//! Counts number of strings in the file, saves pointers to the first and to
//! the last letters of each string to the structure
//!
//! @param [in]      buf              pointer to the buffer
//! @param [out]     num_str          number of strings
//!
//! @return Returns pointer to an array of structures
//-----------------------------------------------------------------------------


struct String* FindStrings(char* buf, size_t* num_str);



//-----------------------------------------------------------------------------
//! Sorts the strings in ascending order of its first letter's code with bubble
//! sort algorithm
//!
//! @param [in]     strings           pointer to an array of structures
//! @param [in]     num_str           number of strings
//-----------------------------------------------------------------------------


void SortByBgn(struct String* strings, size_t num_str);



//-----------------------------------------------------------------------------
//! Sorts the strings in descending order of its last letter's code with bubble
//! sort algorithm
//!
//! @param [in]     strings           pointer to an array of structures
//! @param [in]     num_str           number of strings
//-----------------------------------------------------------------------------


void SortByEnd(struct String* strings, size_t num_str);



//-----------------------------------------------------------------------------
//! Creates the txt file of sorted strings or adds it to the end of existing file
//!
//! @param [in]     strings           pointer to an array of structures
//! @param [in]     Res_name          name of the result's file
//! @param [in]     num_str           number of strings
//! @param [in]     add               0 if it has to create new file and 1 if it
//!                                   has to add text to the existing file
//-----------------------------------------------------------------------------


size_t CreateFile(struct String* strings, const char Res_name[], size_t num_str, int add);



//-----------------------------------------------------------------------------
//! Makes two strings to exchange their pointers
//! 
//! @param [in]    str1               pointer to the first string's structure
//! @param [in]    str2               pointer to the second string's structure
//-----------------------------------------------------------------------------


void Swap(struct String* str1, struct String* str2);



int main()
{
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
    size_t end_ltr = len - 2;

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
        if ((buf[buf_ind-1] == '\n') && (buf[buf_ind-2] != '\n'))
        {
            strings[str_ind].bgn = buf + buf_ind;

            // skip punctuation
            letter = buf_ind-1;
			strings[str_ind-1].nsym = buf + letter;
			while (!isalpha(buf[letter]))
               	letter--;

            strings[str_ind-1].end_ltr = buf + letter;
            str_ind++;
        }

        buf_ind++;
    }

    // skip punctuation
    letter = buf_ind-1;
	strings[str_ind-1].nsym = buf + letter;
    while (!isalpha(buf[letter]))
        letter--;

    strings[str_ind-1].end_ltr = buf + letter;

    return strings;
}



void SortByBgn(struct String* strings, size_t num_str)
{
    size_t k = 0, i = 0;
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
    size_t k = 0, i = 0;
    for (size_t k = 0; k < num_str-1; k++)
    {
        for (size_t i = 0; i < num_str-1; i++)
        {
            if (*(strings[i].end_ltr) > *(strings[i+1].end_ltr))
                Swap(strings+i, strings+i+1);

            else if (*(strings[i].end_ltr) == *(strings[i+1].end_ltr))
            {
                // if strings have same end_ltring
                size_t prev_letter = 1;
                while (*(strings[i].end_ltr - prev_letter) == *(strings[i+1].end_ltr - prev_letter))
                    prev_letter++;

                if (*(strings[i].end_ltr - prev_letter) > *(strings[i+1].end_ltr - prev_letter))
                    Swap(strings+i, strings+i+1);
            }
        }
    }
}



size_t CreateFile(struct String* strings, const char Res_name[], size_t num_str, int add)
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
	
	size_t str = 0;
    for (str = 0; str < num_str; str++)
        fwrite(strings[str].bgn, sizeof(char), strings[str].nsym - strings[str].bgn + 1, file);

    fclose(file);
    return 0;
}



void Swap(struct String* str1, struct String* str2)
{
    char* help = str1->bgn;
    str1->bgn = str2->bgn;
    str2->bgn = help;

    help = str1->end_ltr;
    str1->end_ltr = str2->end_ltr;
    str2->end_ltr = help;

    help = str1->nsym;
    str1->nsym = str2->nsym;
    str2->nsym = help;
}
