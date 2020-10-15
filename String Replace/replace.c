//
// Purpose is to replace all occurrences of the find parameter with replace if it is found in any text files
// of the current directory, if not check if the prefix parameter exists, and prepend find to all occurences of it.
//

#include "csapp.h"
#include <dirent.h>

int main(int argc, char **argv) {
    FILE *file;
    FILE *tempfile;
    int count = 1;
    int lineCount = 1;
    char buf[MAXLINE];
    char *pf, *find, *replace, *prefix, *p, *pBuf;
    struct dirent *de;
    struct stat buffer;

    //Opens current directory
    DIR *dr = opendir(".");

    if (argc != 4) {
        printf("Usage: ./program 'find' 'replace' 'prefix'\n");
        return -1;
    }

    find = argv[1];
    replace = argv[2];
    prefix = argv[3];

    while ((de = readdir(dr)) != NULL) {
        //Checks if the file has a .txt extension if it does it goes into the if branch.
        pf = strstr(de->d_name, ".txt");
        if (pf != NULL) {
            printf("Found Text File %s\nLooking For %s, if found it will replaced with %s\n", de->d_name, find, replace);
            file = fopen(de->d_name, "r");
            tempfile = fopen("tempfile", "w");

            //Reads file line by line and stores each line in buf, each time while is called. Stops once the file ends.
            while(fgets(buf, sizeof(buf), file)) {
                //Attempts the find the first occurrence of the find parameter in the buffer
                p = strstr(buf, find);
                //If there is a Occurrence in the buf then it runs the if branches. If nothing is found, it copies the line to the tempfile as is.
                if (p) {
                    //Temp line storage
                    pBuf = buf;
                    while (p) {
                        printf("Replacing Occurrence number %d of '%s' on line %d of file '%s' with '%s'\n", count, find, lineCount, de->d_name, replace);
                        //Gets the length of the line up to the find occurrence
                        int n = p - pBuf;
                        //Prints up to find occurence
                        fprintf(tempfile, "%.*s", n, pBuf);
                        //Prints the replace parameter where the find is found.
                        fprintf(tempfile, "%s", replace);
                        //pBuf now points to after the find parameter, so the remaining line which has yet to be searched.
                        pBuf = p + strlen(find);
                        //Searches for the next occurrence
                        p = strstr(pBuf, find);
                        count++;
                    }
                    //Prints the remainder of the line, after no more occurrences are found
                    fprintf(tempfile, "%s", pBuf);
                } else {
                    //Prints whole line to the tempfile, if there are no occurrences of the find parameter in the current line
                    fprintf(tempfile, "%s", buf);
                }
                lineCount++;
            }

            //If no find parameters were found, it begins to search for the prefix parameter and will then prepend find parameter to all occurences of prefix parameter
            if (count == 1) {
                printf("Found no occurences of '%s', now searching for '%s', if '%s' is found it will be prepended with '%s'\n", find, prefix, find, prefix);
                fclose(file);
                fclose(tempfile);
                remove(de->d_name);
                rename("tempfile", de->d_name);

                file = fopen(de->d_name, "r");
                tempfile = fopen("tempfile", "w");
                count = 1;
                lineCount = 1;
                while(fgets(buf, sizeof(buf), file)) {
                    lineCount++;
                    //Attempts to find first occurrence of prefix on the current line
                    p = strstr(buf, prefix);
                    if (p) {
                        //Temp Line Storage
                        pBuf = buf;
                        while (p) {
                            printf("Prepending Occurrence number %d of '%s' on line %d of file '%s' with '%s'\n", count, prefix, lineCount, de->d_name, find);
                            //Length of string upto prefix parameter which is found
                            int n = p - pBuf;
                            //Prints everything up to prefix occurrence
                            fprintf(tempfile, "%.*s", n, pBuf);
                            //Prints find
                            fprintf(tempfile, "%s", find);
                            //Prints prefix after find, so find is prepended to prefix
                            fprintf(tempfile, "%s", prefix);
                            //puts the pointer to after the current prefix parameter
                            pBuf = p + strlen(prefix);
                            //Searches for next occurrence of prefix
                            p = strstr(pBuf, prefix);
                            count++;
                        }
                        fprintf(tempfile, "%s", pBuf);
                    } else {
                        //If no prefix's are found on the current line, it prints the entire line as is.
                        fprintf(tempfile, "%s", buf);
                    }
                }
            }

            if (count == 1) {
                printf("Found no occurences of '%s' or '%s' in the file '%s' file remains unchanged.\n", find, prefix, de->d_name);
            }

            fclose(file);
            fclose(tempfile);
            remove(de->d_name);
            rename("tempfile", de->d_name);
        }
    }
    closedir(dr);
}
