#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAXSTRLEN 60
#define MAXNUMSTR 30
#define LETTER 65

#define OPENERR -1
#define READERR -2

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
//! pointers to the first and to the last letters of each (except the last)
//! string in an array
//!
//! @param [in]     str_bgn          array of pointers to the first letters
//! @param [in]     str_end          array of pointers to the last letters
//! @param [in]     buf              data buffer
//! @param [out]    buf_ind          index of '\0' symbol in buffer
//! @param [out]    str_ind          number of the last string
//!
//! @return  Returns number of strings in the poem
//-----------------------------------------------------------------------------


size_t ProcFile(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], char buf[Input_size]);



//-----------------------------------------------------------------------------
//! This is a subfunction of ProcFile that processes last string by adding
//! '\n' symbol to the end and saves previous symbol to the array
//!
//! @param [in]     str_bgn          array of pointers to the first letters
//! @param [in]     str_end          array of pointers to the last letters
//! @param [in]     buf              data buffer
//!
//! @return  Returns number of strings in the poem
//-----------------------------------------------------------------------------


size_t LastString(char buf[Input_size], char* str_end[MAXNUMSTR], size_t buf_ind, size_t str_ind);



//-----------------------------------------------------------------------------
//! Sorts the strings in ascending order of its end code with bubble sort algorithm
//!
//! @param [in]      Num_str         number of strings in the poem
//! @param [out]     str_bgn         sorted array of pointers to the first letters
//! @param [out]     str_end         sorted array of pointers to the last letters
//-----------------------------------------------------------------------------


void BubbleSort(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], size_t Num_str);



//-----------------------------------------------------------------------------
//! Creates the txt file of strings sorted in ascending order of its end letter
//!
//! @param [in]     Res_name         name of the result's file
//! @param [in]     str_bgn          sorted array of pointers to the first letters
//! @param [in]     Num_str          number of strings in the poem
//-----------------------------------------------------------------------------


void CreateFile(char* str_bgn[MAXNUMSTR], const char Res_name[], const size_t Num_str);



//-----------------------------------------------------------------------------
//! Makes two pointers to exchange its values
//-----------------------------------------------------------------------------


void Swap(char** ptr1, char** ptr2);



//-----------------------------------------------------------------------------
//! Prints a conclusion if the sorting was successful
//!
//! @param [in]     Num_str          number of read strings or error code
//-----------------------------------------------------------------------------


void ConclPrint(const char Res_name[], const size_t Num_str);



int main()
    {
    char* str_bgn[MAXNUMSTR] = {0};
    char* str_end[MAXNUMSTR] = {0};
    char buf[Input_size] = {0};

    const size_t Num_str = ReadFile(str_bgn, str_end, buf, File_name);

    BubbleSort(str_bgn, str_end, Num_str);

    CreateFile(str_bgn, Res_name, Num_str);

    ConclPrint(Res_name, Num_str);

    return 0;
    }



int ReadFile(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], char buf[Input_size], const char File_name[])
    {
    FILE* file = NULL;
    size_t check_read = 0;

    if ((file = fopen(File_name, "r")) == NULL)
        {
        printf("File opening error. There is no file with name '%s'\n", File_name);
        return OPENERR;
        }

    check_read = fread(buf, sizeof(char), Input_size, file);
    if (check_read == 0)
        {
        printf("File reading error. The file '%s' is empty\n", File_name);
        return READERR;
        }

    else if (check_read == MAXNUMSTR * MAXSTRLEN)
        {
        printf("File reading error. The file '%s' is too long\n", File_name);
        return READERR;
        }

    fclose(file);

    size_t num_str = ProcFile(str_bgn, str_end, buf);

    return num_str;
    }



size_t ProcFile(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], char buf[Input_size])
    {
    str_bgn[0] = &(buf[0]);

    size_t str_ind = 1;
    size_t buf_ind = 1;
    size_t trash_ind = 0;

    while (buf[buf_ind] != '\0')
        {
        if (buf[buf_ind-1] == '\n')
            {
            str_bgn[str_ind] = buf + buf_ind;

            trash_ind = buf_ind-2;
            if (buf[trash_ind] != '\n')
                {
                while (buf[trash_ind] <= LETTER)
                    trash_ind--;
                }

            str_end[str_ind-1] = buf + trash_ind;
            str_ind++;
            }

        buf_ind++;
        }

    size_t num_str = LastString(buf, str_end, buf_ind, str_ind);

    return num_str;
    }



size_t LastString(char buf[Input_size], char* str_end[MAXNUMSTR], size_t buf_ind, size_t str_ind)
    {
    if (buf[buf_ind-1] != '\n')
        {
        buf[buf_ind] = '\n';
        buf[buf_ind+1] = '\0';
        }

    size_t trash_ind = buf_ind-1;
    while (buf[trash_ind] <= LETTER)
        trash_ind--;

    str_end[str_ind-1] = buf + trash_ind;

    return str_ind;
    }



void BubbleSort(char* str_bgn[MAXNUMSTR], char* str_end[MAXNUMSTR], const size_t Num_str)
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
                size_t cnd = 1;
                while (*(str_end[i] - cnd) == *(str_end[i+1] - cnd))
                    cnd++;

                if (*(str_end[i] - cnd) > *(str_end[i+1] - cnd))
                    {
                    Swap(&str_end[i], &str_end[i+1]);
                    Swap(&str_bgn[i], &str_bgn[i+1]);
                    }
                }
            }
        }
    }



void CreateFile(char* str_bgn[MAXNUMSTR], const char Res_name[], const size_t Num_str)
    {
    FILE* file;

    file = fopen(Res_name, "w");

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



void ConclPrint(const char Res_name[], const size_t Num_str)
    {
    if (Num_str > 0)
        printf("Sorting was done successfully. Check the file '%s' in the program's directory.\n", Res_name);
    }
