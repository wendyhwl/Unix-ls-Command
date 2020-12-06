/*************
 * Date: Aug 10, 2020
 * File: myls.h
**************/

#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>

#ifndef _MYLS_H_
#define _MYLS_H_
#define PERM_LEN 10
#define TIME_LEN 20
#define MAX_LEN 512
#define MAX(x, y) ((x) > (y) ? (x) : (y))

// data structure for recognizing i/l/R
struct option_t
{
    bool has_i; // if command has -i
    bool has_l; // if command has -l
    bool has_R; // if command has -R

    int num_path; // number of paths
    char **paths; // paths
};

// simple sorting function that takes O(n^2)
void sort(char **a, int len);

// function for parsing arguments
bool parse_arguments(int argc, char **argv, struct option_t *option);

// function for printing program usage
void print_usage(const char *prog);

// function for S_ISREG, test if path is a regular file
bool isreg(const char *path);

// function for S_ISDIR, test if path is a directory
bool isdir(const char *path);

// function to get file permission string
void get_perm_str(struct stat *st, char *out);

// function to get date and time for the latest modification
void get_time_str(struct stat *st, char *out);

// function to get length of a file
int get_size_length(const char *file);

// function to get maximum length
int get_max_size(struct dirent **namelist, int n);

// function to get file name
const char *get_file_name(const char *path);

// function for dealing with all paths
void ls(const char *path, struct option_t *opt);

// function for dealing with file paths
void lf(const char *file, struct option_t *opt, int width);

// function for running ls
void run_ls(struct option_t *opt);

#endif