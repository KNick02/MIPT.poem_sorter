#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
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
//! Comparator for qsort(), compares first letters of strings 
//!
//! @param [in]     str1              pointer to the first structure
//! @param [in]     str2              pointer to the second structure
//! 
//! @return Returns positive, negative or zero number if first string's letter 
//! code is more, less or equal to the second string's letter respectively
//-----------------------------------------------------------------------------


int comp_bgn(const struct String* str1, const struct String* str2);



//-----------------------------------------------------------------------------
//! Comparator for qsort(), compares last letters of strings 
//!
//! @param [in]     str1              pointer to the first structure
//! @param [in]     str2              pointer to the second structure
//! 
//! @return Returns positive, negative or zero number if first string's letter 
//! code is more, less or equal to the second string's letter respectively
//-----------------------------------------------------------------------------


int comp_end(const struct String* str1, const struct String* str2);



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



int main()
{
    char file_name[MAXNAMELEN] = "";
    size_t len = 0;
    size_t num_str = 0;

	setlocale(LC_ALL, "Rus");

    IntroPrint(file_name);

    char* buf = ReadFile(&len, file_name);
    if (buf == NULL)
        return EACCES;

    DeleteSpaces(buf, len);

    struct String* strings = FindStrings(buf, &num_str);

    qsort(strings, num_str, sizeof(struct String), comp_bgn);

    size_t err = CreateFile(strings, Res_name, num_str, NEWFILE);
    if (err == EACCES)
        return EACCES;

    qsort(strings, num_str, sizeof(struct String), comp_end);
	
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



int comp_bgn(const struct String* str1, const struct String* str2)
{
    size_t i = 0;
    while (*(str1->bgn - i) == *(str2->bgn - i))
        i++;

    return (*(str1->bgn - i) - *(str2->bgn - i));
}


int comp_end(const struct String* str1, const struct String* str2)
{
    size_t i = 0;
    while (*(str1->end_ltr - i) == *(str2->end_ltr - i))
        i++;

    return (*(str1->end_ltr - i) - *(str2->end_ltr - i));

}












