#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <io.h>
#include <stdlib.h>

#define MAXSTRLEN 60
#define MAXNUMSTR 30
#define SEPARATE "-------------------------------------------------\n"


const size_t Input_size = MAXNUMSTR * MAXSTRLEN;
const char File_name[] = "poem.txt";
const char Res_name[] = "sorted_poem.txt";


//-----------------------------------------------------------------------------
//! Reads a poem from the txt file and puts it in buffer, saves
//! pointers to the first and to the last letters of each string in an array
//!
//! @param [in]      File_name        name of the poem's file
//! @param [out]     buf              data buffer
//! @param [out]     str_bgn          array of pointers to the first letters
//! @param [out]     str_end          array of pointers to the last letters
//!
//! @return  Returns number of strings in the poem if it was successful,
//! -1 if there was an opening error, -2 if there was a reading error
//-----------------------------------------------------------------------------


int ReadFile(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], char buf[Input_size], const char File_name[]);



//-----------------------------------------------------------------------------
//! This is a subfunction of ReadFile that saves
//! pointers to the first and to the last letters of each string in an array
//!
//! @param [in]     buf              data buffer
//! @param [out]    str_bgn          array of pointers to the first letters
//! @param [out]    str_end          array of pointers to the last letters
//! @return  Returns number of strings in the poem
//-----------------------------------------------------------------------------


size_t FindStrings(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], char buf[Input_size], size_t num_char);



//-----------------------------------------------------------------------------
//! Sorts the strings in ascending order of its first letter's code with bubble
//! sort algorithm
//!
//! @param [in]     Num_str         number of strings in the poem
//! @param [in]     str_bgn         sorted array of pointers to the first letters
//! @param [in]     str_end         sorted array of pointers to the last letters
//-----------------------------------------------------------------------------


void BgnBubbleSort(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], size_t Num_str);



//-----------------------------------------------------------------------------
//! Sorts the strings in ascending order of its last letter's code with bubble
//! sort algorithm
//!
//! @param [in]     Num_str         number of strings in the poem
//! @param [in]     str_bgn         sorted array of pointers to the first letters
//! @param [in]     str_end         sorted array of pointers to the last letters
//-----------------------------------------------------------------------------


void EndBubbleSort(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], size_t Num_str);



//-----------------------------------------------------------------------------
//! Creates the txt file of sorted strings or adds it to the end of existing file
//!
//! @param [in]     Res_name         name of the result's file
//! @param [in]     str_bgn          sorted array of pointers to the first letters
//! @param [in]     Num_str          number of strings in the poem
//! @param [in]     add              0 if it has to create new file and 1 if it
//!                                  has to add text to the existing file
//-----------------------------------------------------------------------------


void CreateFile(char* str_bgn[MAXNUMSTR], const char Res_name[], const size_t Num_str, bool add);



//-----------------------------------------------------------------------------
//! Makes two pointers to exchange its values
//-----------------------------------------------------------------------------


void Swap(char** ptr1, char** ptr2);



//-----------------------------------------------------------------------------
//! Prints a conclusion if the sorting was successful
//!
//! @param [in]     Num_str          number of read strings or error code
//-----------------------------------------------------------------------------


void FinalPrint(const char Res_name[], const size_t Num_str);




//-----------------------------------------------------------------------------
//! The structure contains a buffer of read characters and information
//! about first and last letters of each string
//-----------------------------------------------------------------------------


struct strings
    {
    char* bgn[MAXNUMSTR];
    char* end[MAXNUMSTR];
    char buf[Input_size];
    };



int main()
    {
    struct strings poem = {{0}, {0}, {0}};

    const size_t Num_str = ReadFile(poem.bgn, poem.end, poem.buf, File_name);

    BgnBubbleSort(poem.bgn, poem.end, Num_str);

    CreateFile(poem.bgn, Res_name, Num_str, 0);

    EndBubbleSort(poem.bgn, poem.end, Num_str);

    CreateFile(poem.bgn, Res_name, Num_str, 1);

    FinalPrint(Res_name, Num_str);

    return 0;
    }


int ReadFile(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], char buf[Input_size], const char File_name[])
    {
    if (access(File_name, 0) == -1)
        {
        printf("Opening error: file '%s' was not found\n", File_name);
        exit(0);
        }

    if (access(File_name, 3) == -1)
        {
        printf("Reading error: file '%s' reading access denied\n", File_name);
        exit(0);
        }

    FILE* file = NULL;
    file = fopen(File_name, "r");

    size_t num_char = fread(buf, sizeof(char), Input_size, file);
    if (num_char == 0)
        {
        printf("Reading error: file '%s' is empty\n", File_name);
        exit(0);
        }

    else if (num_char == MAXNUMSTR * MAXSTRLEN)
        {
        printf("Reading error: file '%s' is too long\n", File_name);
        exit(0);
        }

    fclose(file);

    size_t num_str = FindStrings(str_bgn, str_end, buf, num_char);

    return num_str;
    }



size_t FindStrings(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], char buf[Input_size], size_t num_char)
    {
    // delete all spaces at the end of file
    size_t find_ltr = num_char-1;
    while (isspace(buf[find_ltr]) != 0)
        find_ltr--;

    buf[find_ltr+1] = '\n';
    buf[find_ltr+2] = '\0';

    str_bgn[0] = &(buf[0]);

    size_t str_ind = 1;
    size_t buf_ind = 1;

    while (buf[buf_ind] != '\0')
        {
        if (buf[buf_ind-1] == '\n')
            {
            str_bgn[str_ind] = buf + buf_ind;

            // skip punctuation
            find_ltr = buf_ind-2;
            if (buf[find_ltr] != '\n')
                {
                while (isalpha(buf[find_ltr]) == 0)
                    find_ltr--;
                }

            str_end[str_ind-1] = buf + find_ltr;
            str_ind++;
            }

        buf_ind++;
        }

    // skip punctuation
    find_ltr = buf_ind-1;
    while (isalpha(buf[find_ltr]) == 0)
        find_ltr--;

    str_end[str_ind-1] = buf + find_ltr;

    return str_ind;
    }



void EndBubbleSort(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], const size_t Num_str)
    {
    for (size_t k = 0; k < Num_str-1; k++)
        {
        for (size_t i = 0; i < Num_str-1; i++)
            {
            if (*str_end[i] > *str_end[i+1])
                {
                Swap(&str_end[i], &str_end[i+1]);
                Swap(&str_bgn[i], &str_bgn[i+1]);
                }

            else if (*str_end[i] == *str_end[i+1])
                {
                // if strings have same endings
                size_t prev_letter = 1;
                while (*(str_end[i] - prev_letter) == *(str_end[i+1] - prev_letter))
                    prev_letter++;

                if (*(str_end[i] - prev_letter) > *(str_end[i+1] - prev_letter))
                    {
                    Swap(&str_end[i], &str_end[i+1]);
                    Swap(&str_bgn[i], &str_bgn[i+1]);
                    }
                }
            }
        }
    }



void BgnBubbleSort(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], const size_t Num_str)
    {
    for (size_t k = 0; k < Num_str-1; k++)
        {
        for (size_t i = 0; i < Num_str-1; i++)
            {
            if (*str_bgn[i] > *str_bgn[i+1])
                {
                Swap(&str_bgn[i], &str_bgn[i+1]);
                Swap(&str_end[i], &str_end[i+1]);
                }

            else if (*str_bgn[i] == *str_bgn[i+1])
                {
                // if strings have same beginning
                size_t next_letter = 1;
                while (*(str_bgn[i] + next_letter) == *(str_bgn[i+1] + next_letter))
                    next_letter++;

                if (*(str_bgn[i] + next_letter) > *(str_bgn[i+1] + next_letter))
                    {
                    Swap(&str_bgn[i], &str_bgn[i+1]);
                    Swap(&str_end[i], &str_end[i+1]);
                    }
                }
            }
        }
    }



void CreateFile(char* str_bgn[MAXNUMSTR], const char Res_name[], const size_t Num_str, bool add)
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
        for (char* check = str_bgn[str]; *check != '\n'; check++)
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
