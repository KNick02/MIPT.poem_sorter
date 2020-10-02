#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <io.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

#define MAXSTRLEN 100
#define MAXNUMSTR 50
#define MAXNAMELEN 20
#define SEPARATE "-------------------------------------------------\n"
#define NEWFILE 0
#define ADDFILE 1


const size_t Input_size = MAXNUMSTR * MAXSTRLEN;
const char Res_name[] = "sorted_poem.txt";


//-----------------------------------------------------------------------------
//! Contains a pointer to the buffer of read characters and arrays of pointers
//! to the first and last letters of each string
//-----------------------------------------------------------------------------


struct strings
    {
    char* bgn[MAXNUMSTR];//! pointers to the beginning of strings
    char* end[MAXNUMSTR];//! pointers to the ending of strings
    char* ptr;           //! pointer to the buffer
    };



//-----------------------------------------------------------------------------
//! Prints an introduction and requests the name of poem's file
//!
//! @param [out]     file_name        name of the poem's file
//-----------------------------------------------------------------------------


void GetName(char file_name[MAXNAMELEN]);



//-----------------------------------------------------------------------------
//! Reads a poem from the txt file and puts it in buffer, saves
//! pointers to the first and to the last letters of each string in an array
//!
//! @param [in]      File_name        name of the poem's file
//! @param [in]      poem             pointer to the structure which contains
//!                                   data
//!
//! @return Returns number of strings in the poem if it was successful,
//! -1 if there was an opening error, -2 if there was a reading error
//-----------------------------------------------------------------------------


int ReadFile(struct strings* poem, const char File_name[]);



//-----------------------------------------------------------------------------
//! This is a subfunction of ReadFile that saves
//! pointers to the first and to the last letters of each string in an array
//!
//! @param [in]      poem             pointer to the structure which contains
//!                                   data
//!
//! @return Returns number of strings in the poem
//-----------------------------------------------------------------------------


size_t FindStrings(struct strings* poem, size_t num_char);



//-----------------------------------------------------------------------------
//! Sorts the strings in ascending order of its first letter's code with bubble
//! sort algorithm
//!
//! @param [in]      Num_str          number of strings in the poem
//! @param [in]      poem             pointer to the structure which contains
//!                                   data
//-----------------------------------------------------------------------------


void BgnBubbleSort(struct strings* poem, size_t Num_str);



//-----------------------------------------------------------------------------
//! Sorts the strings in ascending order of its last letter's code with bubble
//! sort algorithm
//!
//! @param [in]      Num_str          number of strings in the poem
//! @param [in]      poem             pointer to the structure which contains
//!                                   data
//-----------------------------------------------------------------------------


void EndBubbleSort(struct strings* poem, size_t Num_str);



//-----------------------------------------------------------------------------
//! Creates the txt file of sorted strings or adds it to the end of existing file
//!
//! @param [in]     poem              pointer to the structure which contains
//!                                   data
//! @param [in]     Res_name          name of the result's file
//! @param [in]     Num_str           number of strings in the poem
//! @param [in]     add               0 if it has to create new file and 1 if it
//!                                   has to add text to the existing file
//-----------------------------------------------------------------------------


void CreateFile(struct strings* poem, const char Res_name[], const size_t Num_str, bool add);



//-----------------------------------------------------------------------------
//! Makes two pointers to exchange its values
//-----------------------------------------------------------------------------


void Swap(char** ptr1, char** ptr2);



//-----------------------------------------------------------------------------
//! Prints a conclusion if the sorting was successful
//!
//! @param [in]     Num_str          number of read string
//-----------------------------------------------------------------------------


void FinalPrint(const char Res_name[], const size_t Num_str);



int main()
    {
    setlocale(LC_ALL, "Rus");

    struct strings data = {{0}, {0}, NULL};
    struct strings* poem = &data;

    poem->ptr = (char*)calloc(Input_size, sizeof(char));
    char File_name[MAXNAMELEN] = {0};

    GetName(File_name);

    const size_t Num_str = ReadFile(poem, File_name);

    BgnBubbleSort(poem, Num_str);

    CreateFile(poem, Res_name, Num_str, NEWFILE);

    EndBubbleSort(poem, Num_str);

    CreateFile(poem, Res_name, Num_str, ADDFILE);

    FinalPrint(Res_name, Num_str);

    free(poem->ptr);
    return 0;
    }



void GetName(char file_name[MAXNAMELEN])
    {
    printf("English poem sorter\nPlease, enter the file name: ");
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



int ReadFile(struct strings* poem, const char File_name[])
    {
    if (access(File_name, 3) == -1)
        {
        printf("Reading error: file '%s' reading access denied\n", File_name);
        exit(EACCES);
        }

    FILE* file = NULL;
    file = fopen(File_name, "r");

    size_t num_char = fread(poem->ptr, sizeof(char), Input_size, file);

    if (num_char == 0)
        {
        printf("Reading error: file '%s' is empty\n", File_name);
        exit(0);
        }

    else if (num_char == Input_size)
        {
        printf("Reading error: file '%s' is too long\n", File_name);
        exit(EFBIG);
        }

    fclose(file);

    size_t num_str = FindStrings(poem, num_char);

    return num_str;
    }



size_t FindStrings(struct strings* poem, size_t num_char)
    {
    // delete all spaces at the end of file
    size_t find_ltr = num_char-1;
    while (isspace(poem->ptr[find_ltr]) != 0)
        find_ltr--;

    poem->ptr[find_ltr+1] = '\n';
    poem->ptr[find_ltr+2] = '\0';

    poem->bgn[0] = &(poem->ptr[0]);

    size_t str_ind = 1;
    size_t buf_ind = 1;

    while (poem->ptr[buf_ind] != '\0')
        {
        if (poem->ptr[buf_ind-1] == '\n')
            {
            poem->bgn[str_ind] = poem->ptr + buf_ind;

            // skip punctuation
            find_ltr = buf_ind-2;
            if (poem->ptr[find_ltr] != '\n')
                {
                while (isalpha(poem->ptr[find_ltr]) == 0)
                    find_ltr--;
                }

            poem->end[str_ind-1] = poem->ptr + find_ltr;
            str_ind++;
            }

        buf_ind++;
        }

    // skip punctuation
    find_ltr = buf_ind-1;
    while (isalpha(poem->ptr[find_ltr]) == 0)
        find_ltr--;

    poem->end[str_ind-1] = poem->ptr + find_ltr;

    return str_ind;
    }



void EndBubbleSort(struct strings* poem, const size_t Num_str)
    {
    for (size_t k = 0; k < Num_str-1; k++)
        {
        for (size_t i = 0; i < Num_str-1; i++)
            {
            if (*poem->end[i] > *poem->end[i+1])
                {
                Swap(&poem->end[i], &poem->end[i+1]);
                Swap(&poem->bgn[i], &poem->bgn[i+1]);
                }

            else if (*poem->end[i] == *poem->end[i+1])
                {
                // if strings have same endings
                size_t prev_letter = 1;
                while (*(poem->end[i] - prev_letter) == *(poem->end[i+1] - prev_letter))
                    prev_letter++;

                if (*(poem->end[i] - prev_letter) > *(poem->end[i+1] - prev_letter))
                    {
                    Swap(&poem->end[i], &poem->end[i+1]);
                    Swap(&poem->bgn[i], &poem->bgn[i+1]);
                    }
                }
            }
        }
    }



void BgnBubbleSort(struct strings* poem, const size_t Num_str)
    {
    for (size_t k = 0; k < Num_str-1; k++)
        {
        for (size_t i = 0; i < Num_str-1; i++)
            {
            if (*poem->bgn[i] > *poem->bgn[i+1])
                {
                Swap(&poem->bgn[i], &poem->bgn[i+1]);
                Swap(&poem->end[i], &poem->end[i+1]);
                }

            else if (*poem->bgn[i] == *poem->bgn[i+1])
                {
                // if strings have same beginning
                size_t next_letter = 1;
                while (*(poem->bgn[i] + next_letter) == *(poem->bgn[i+1] + next_letter))
                    next_letter++;

                if (*(poem->bgn[i] + next_letter) > *(poem->bgn[i+1] + next_letter))
                    {
                    Swap(&poem->bgn[i], &poem->bgn[i+1]);
                    Swap(&poem->end[i], &poem->end[i+1]);
                    }
                }
            }
        }
    }



void CreateFile(struct strings* poem, const char Res_name[], const size_t Num_str, bool add)
    {
    if (access (Res_name, 2) == -1)
        {
        printf("Writing error: file '%s' writing access denied");
        exit(0);
        }

    FILE* file;

    if (!add)
        file = fopen(Res_name, "w");
    else
        file = fopen(Res_name, "a");

    fputs(SEPARATE, file);

    for (size_t str = 0; str < Num_str; str++)
        {
        for (char* check = poem->bgn[str]; *check != '\n'; check++)
            fputc(*check, file);

        fputc('\n', file);
        }

    fclose(file);
    }



void Swap(char** ptr1, char** ptr2)
    {
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);

    char* help = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = help;
    }



void FinalPrint(const char Res_name[], const size_t Num_str)
    {
    // printed if no errors
    if (Num_str > 0)
        printf("Sorting was done successfully. Check the file '%s' in the program's directory\n", Res_name);
    }
