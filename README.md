# Unix-ls-Command
Implementing a version of Unix ls command which supports a limited set of options. 

## Usage

### Run the program
To compile and run the program, enter on terminal:

```bash
make
./myls [options] [file list]
```
For example:

```bash
./myls -i -l
./myls -i -R -l
./myls -l
./myls ..
```

### Erase All
To erase all built products:

```bash
make clean
```

### Supported Options
The following options are supported:
- **-i**: Print the index number of each file
- **-l**: Use a long listing format
- **-R**: List subdirectories recursively

*Notes: The long version of the option names are NOT supported. *

## Author
Wendy Huang

Date: August 10, 2020

## Acknowledgements
- Assignment created by Profs Harinder Khangura and Brian Fraser 
- [SFU Academic Integrity](http://www.sfu.ca/students/academicintegrity.html)
