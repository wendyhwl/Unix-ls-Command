/*************
 * Date: Aug 10, 2020
 * File: myls.c
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
#include "myls.h"

// simple sorting function that takes O(n^2)
void sort(char **a, int len)
{
  for (int i = 0; i < len; ++i) {
    for (int j = 0; j + i + 1 < len; ++j) {
      if (strcmp(a[j], a[j + 1]) > 0) {
        char *tmp = a[j];
        a[j] = a[j + 1];
        a[j + 1] = tmp;
      }
    }
  }
}

// function for parsing arguments
bool parse_arguments(int argc, char **argv, struct option_t *option)
{
  int opt;

  // taking command line options one by one
  // test if there is i/l/R
  while ((opt = getopt(argc, argv, "ilR")) != -1) {
    switch (opt) {
    case 'i':
      option->has_i = true;
      break;
    case 'l':
      option->has_l = true;
      break;
    case 'R':
      option->has_R = true;
      break;
    default:
      return false;
    }
  }

  // if index of next element < argc
  if (optind < argc) {
    option->num_path = argc - optind;
    option->paths = argv + optind;
    sort(option->paths, option->num_path);
  }

  // no issue return true
  return true;
}

// function for printing program usage
void print_usage(const char *prog)
{
  fprintf(stderr, "Usage: %s [OPTION]\n", prog);
  fprintf(stderr, "  -i: Print the index number of each file\n");
  fprintf(stderr, "  -l: Use a long listing format\n");
  fprintf(stderr, "  -R: List subdirectories recursively\n");
}

// function for S_ISREG
// test if path is a regular file
bool isreg(const char *path)
{
  struct stat st;
  // if status error, exit with EXIT_FAILURE
  if (stat(path, &st)) {
    perror("stat");
    exit(EXIT_FAILURE);
  }
  return S_ISREG(st.st_mode);
}

// function for S_ISDIR
// test if path is a directory
bool isdir(const char *path)
{
  struct stat st;
  // if status error, exit with EXIT_FAILURE
  if (stat(path, &st)) {
    perror("stat");
    exit(EXIT_FAILURE);
  }
  return S_ISDIR(st.st_mode);
}

// function to get file permission string
void get_perm_str(struct stat *st, char *out)
{
  int idx = 0;
  out[idx++] = S_ISDIR(st->st_mode) ? 'd' : '-';
  out[idx++] = (S_IRUSR & st->st_mode) ? 'r' : '-';
  out[idx++] = (S_IWUSR & st->st_mode) ? 'w' : '-';
  out[idx++] = (S_IXUSR & st->st_mode) ? 'x' : '-';
  out[idx++] = (S_IRGRP & st->st_mode) ? 'r' : '-';
  out[idx++] = (S_IWGRP & st->st_mode) ? 'w' : '-';
  out[idx++] = (S_IXGRP & st->st_mode) ? 'x' : '-';
  out[idx++] = (S_IROTH & st->st_mode) ? 'r' : '-';
  out[idx++] = (S_IWOTH & st->st_mode) ? 'w' : '-';
  out[idx++] = (S_IXOTH & st->st_mode) ? 'x' : '-';
  out[idx++] = '\0';
}

// function to get date and time for the latest modification
void get_time_str(struct stat *st, char *out)
{
  struct tm tm;
  tzset();
  localtime_r(&(st->st_mtim.tv_sec), &tm);
  strftime(out, TIME_LEN, "%b %e %G %H:%M", &tm);
}

// function to get length of a file
int get_size_length(const char *file)
{
  struct stat st;
  stat(file, &st);
  int len = 0;
  int n = st.st_size;
  do {
    ++len;
    n /= 10;
  } while (n);
  return len;
}

// function to get maximum length
int get_max_size(struct dirent **namelist, int n)
{
  int max = 0;
  while (n--) {
    const char *name = namelist[n]->d_name;
    if (strcmp(name, ".") && strcmp(name, "..")) {
      max = MAX(max, get_size_length(name));
    }
  }
  return max;
}

// function to get file name
const char *get_file_name(const char *path)
{
  for (int i = (int)strlen(path) - 1; i >= 0; --i) {
    if (path[i] == '/') {
      return path + i + 1;
    }
  }
  return path;
}

// function for dealing with file paths
void lf(const char *file, struct option_t *opt, int width)
{
  struct stat st;
  // if status error, exit with EXIT_FAILURE
  if (stat(file, &st)) {
    perror("stat");
    exit(EXIT_FAILURE);
  }

  // if command option include -i
  if (opt->has_i) {
    // print inode number
    fprintf(stdout, "%ld ", st.st_ino);
  }

  // if command option include -l
  if (opt->has_l) {
    // file permission string
    char perm[PERM_LEN + 1];
    get_perm_str(&st, perm);

    // get time string
    char tm[TIME_LEN + 1];
    get_time_str(&st, tm);

    // print output
    fprintf(stdout, "%s ", perm);
    fprintf(stdout, "%ld ", st.st_nlink);
    fprintf(stdout, "%s ", getpwuid(st.st_uid)->pw_name);
    fprintf(stdout, "%s ", getgrgid(st.st_gid)->gr_name);
    fprintf(stdout, "%*ld ", width, st.st_size);
    fprintf(stdout, "%s ", tm);
  }
  fprintf(stdout, "%s\n", get_file_name(file));
}

// function for dealing with all paths
void ls(const char *path, struct option_t *opt)
{
  // if path is a regular file, run with lf
  if (isreg(path)) {
    lf(path, opt, 0);
  }
  else {
    struct dirent **namelist;
    int n;

    // reference: How to properly use scandir() in c
    // https://stackoverflow.com/questions/18402428/how-to-properly-use-scandir-in-c
    n = scandir(path, &namelist, NULL, alphasort);
    if (n == -1) {
      perror("scandir");
      return;
    }

    // if command option include -R
    if (opt->has_R) {
      fprintf(stdout, "%s:\n", path);
    }

    char nxt_path[MAX_LEN];
    strcpy(nxt_path, path);
    if (path[strlen(path) - 1] != '/') {
      strcat(nxt_path, "/");
    }
    char *name_start = nxt_path + strlen(nxt_path);

    int width = get_max_size(namelist, n);
    for (int i = 0; i < n; ++i) {
      const char *name = namelist[i]->d_name;
      if (strcmp(name, ".") && strcmp(name, "..")) {
        strcpy(name_start, name);
        lf(nxt_path, opt, width);
      }
    }
    if (opt->has_R) {
      fprintf(stdout, "\n");
    }

    for (int i = 0; i < n; ++i) {
      const char *name = namelist[i]->d_name;
      strcpy(name_start, name);
      if (strcmp(name, ".") && strcmp(name, "..") && isdir(nxt_path)) {
        ls(nxt_path, opt);
      }
      free(namelist[i]);
    }
    free(namelist);
  }
}

// function for running ls
void run_ls(struct option_t *opt)
{
  if (opt->paths) {
    for (int i = 0; i < opt->num_path; ++i) {
      if (opt->num_path > 1 && isdir(opt->paths[i])) {
        fprintf(stdout, "%s:\n", opt->paths[i]);
      }
      // if file exists
      if (access(opt->paths[i], F_OK)) {
        fprintf(stdout, "\"%s\" command not found\n", opt->paths[i]);
      }
      ls(opt->paths[i], opt);
    }
  }
  else {
    ls(".", opt);
  }
}

// main function to run the program
int main(int argc, char **argv)
{
  // create struct for command options
  struct option_t opt = (struct option_t){false, false, false, 0, NULL};

  // if input error, print usage, exit with EXIT_FAILURE
  if (!parse_arguments(argc, argv, &opt)) {
    print_usage(*argv);
    exit(EXIT_FAILURE);
  }

  // no error, run ls
  run_ls(&opt);

  return 0;
}
