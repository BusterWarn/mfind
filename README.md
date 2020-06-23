# mfind
Mfind is a program that offers that finds files, repositories or links in a unix based directory system using multiple threads. Assignement in course Systemnära Programmering.

# .c code encrypted
All .c code (except for parser.c) are encrypted with Encryption-Decryption.py. This is so that students taking the course will not plagiarize the material. If you want acces to the key do decrypt the files, please contact me.

Credit to https://github.com/Raviteja-Ainampudi/Dynamic-Encryption-And-Decryption-of-Any-files for making the encryption script.

# Execution

After compilation with `make`, mfind is run as following:

```bash
$ ./mfind [-t type] [-p nrthr] start1 [start2 ...] target
```
`-t`		Type of target to find. f=file, d=directory, l=link. If empty,
mfind will search for any of these.

`-p`		Number of threads that mfind will use to search for the target.
Default value is 1. Must be a positive integer.

`start`		Starting directory to begin search from. Must be one or more
starting directories.

`target`	The name of the target file/directory/link that mfind will search
for. There must be exactly one target.

## Example 
The following example will find the file "mfind.c" in the current directory can with 10 threads
```bash
$ ./mfind -tf -p10 . mfind.c
```