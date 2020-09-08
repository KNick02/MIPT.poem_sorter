#include <stdio.h>

#define INFILE 8
#define OUTFILE 9

const int max_num_str = 20;
const int str_size = 100;

int ReadFile(char rec[max_num_str][str_size], char file_name[INFILE]);

void DeleteTrash(char data[max_num_str][str_size], int num_strings);

void BubbleSort(char data[max_num_str][str_size], int num_strings);

void Swap(char str1[str_size], char str2[str_size]);

int EndString(char data[max_num_str][str_size], int str_number);

void Copy(char from[str_size], char to[str_size]);

void CreateFile(char data[max_num_str][str_size], char file2_name[OUTFILE], int num_strings);


int main()
    {
    char name[] = "poem.txt";
    char save[] = "poem2.txt";

    char data[max_num_str][str_size] = {{0}};
    int num_strings = 0;

    num_strings = ReadFile(data, name);

    DeleteTrash(data, num_strings);

    BubbleSort(data, num_strings);

    CreateFile(data, save, num_strings);

    return 0;
    }


int ReadFile(char rec[max_num_str][str_size], char file_name[INFILE])
    {
    FILE* file;
    char c;

    int i = 0, j = 0;

    file = fopen(file_name, "r");

    while ((c = fgetc(file)) != EOF)
        {
        if (c != '\n')
            {
            rec[i][j] = c;
            j++;
            }

        else if (j > 0)
            {
            rec[i][j] = '\0';
            i++;
            j = 0;
            }
        }

    fclose(file);

    return (i+1);
    }


void DeleteTrash(char data[max_num_str][str_size], int num_strings)
    {
    int str, sym;

    for (str = 0; str < num_strings; str++)
        {
        sym = EndString(data, str);

        if ((data[str][sym]) < 65)
            data[str][sym] = '\0';
        }
    }


int EndString(char data[max_num_str][str_size], int str_number)
    {
    int i = 0;

    while (data[str_number][i] != '\0')
        i++;

    return (i-1);
    }



void BubbleSort(char data[max_num_str][str_size], int num_strings)
    {
    int k, i;

    for (k = 0; k < num_strings-1; k++)
        {
        for (i = 0; i < num_strings-1; i++)
            {
            if (data[i][EndString(data,i)] < data[i+1][EndString(data,i+1)])
                Swap(data[i], data[i+1]);
            }
        }
    }


void Swap(char str1[str_size], char str2[str_size])
    {
    char help[str_size];

    Copy(str1, help);
    Copy(str2, str1);
    Copy(help, str2);
    }




void Copy(char from[str_size], char to[str_size])
    {
    int i;

    for (i = 0; from[i] != '\0'; i++)
        to[i] = from[i];

    to[i] = '\0';
    }



void CreateFile(char data[max_num_str][str_size], char file2_name[OUTFILE], int num_strings)
    {
    FILE* file2;

    int i, j = 0;

    file2 = fopen(file2_name, "w");

    for (i = 0; i < num_strings; i++)
        {
        fputs(data[i], file2);
        fputc('\n', file2);
        }


    fclose(file2);
    }





