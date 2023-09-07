#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define RESET "\x1B[0m"
#define BOLD "\x1B[1m"
#define MAX_TITLE_LENGTH 100

FILE *fp1, *fp2, *fp;

void tableOfContents(void);
void createNewPage(void);
void appendPage(void);
void deletePage(void);
void searchJournal(void);
void viewPage(void);
void duplicatePage(void);

int main(void) {
  int choice;
  int running;

  running = 1;

  struct stat st = {0};

  if (stat("./entries", &st) == -1) {
    mkdir("./entries", 0700);
  }

  printf("\n%sWelcome to Your Journal%s\n", BOLD, RESET);

  while (running) {
    printf("\n___________________________________________________\n");
    printf("\n%sMain Page%s\n", BOLD, RESET);
    printf("___________________________________________________\n\n");
    printf("1. Table of Contents\n");
    printf("2. Create New / Overwrite Page\n");
    printf("3. Append to Page\n");
    printf("4. Delete Page\n");
    printf("5. Search for Word/Phrase\n");
    printf("6. View Page\n");
    printf("7. Duplicate Page\n");
    printf("8. Exit\n");
    printf("\nInput Option Number: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      tableOfContents();
      continue;
    case 2:
      createNewPage();
      continue;
    case 3:
      appendPage();
      continue;
    case 4:
      deletePage();
      continue;
    case 5:
      searchJournal();
      continue;
    case 6:
      viewPage();
      continue;
    case 7:
      duplicatePage();
      continue;
    case 8:
      running = 0;
    }

    return 0;
  }
}

void createNewPage() {
  char c;
  int page_num;
  char buf[0x100];
  char title[0x100];
  char contents[0x100];

  printf("\nEnter New Page Number: ");
  scanf("%d", &page_num);

  printf("\n___________________________________________________\n");
  printf("\n%sWrite Page %d%s\n", BOLD, page_num, RESET);
  printf("___________________________________________________\n");

  snprintf(buf, sizeof(buf), "./entries/%d.txt", page_num);
  FILE *fp1 = fopen(buf, "w");

  printf("\nEnter Title: ");
  scanf("%s", title);

  fprintf(fp1, "%d", page_num);
  fprintf(fp1, ": ");
  fprintf(fp1, "%s", title);
  fprintf(fp1, "\n");

  printf("\nEnter the text and press '/' to save\n\n");
  while (1) {
    c = getchar();
    fputc(c, fp1);

    if (c == '/') {
      fclose(fp1);
      break;
    }
  }

  tableOfContents();
}

void appendPage() {
  int page_num;
  char buf1[0x100];
  char c;
  char repl;

  printf("\nEnter page number: ");
  scanf("%d", &page_num);

  printf("\n___________________________________________________\n");
  printf("\n%sAppend to Page %d%s\n", BOLD, page_num, RESET);
  printf("___________________________________________________\n\n");

  snprintf(buf1, sizeof(buf1), "./entries/%d.txt", page_num);
  FILE *fp1 = fopen(buf1, "r+");
  FILE *fp2 = fopen(buf1, "a");

  if (fp1 == NULL) {
    printf("\nFile does not exist");
    fclose(fp1);
    goto end3;
  }

  repl = ' ';
  c = 0;
  while (c != '/') {
    c = getc(fp1);
    if (c == '/') {
      fseek(fp1, -1, SEEK_CUR);
      fputc(repl, fp1);
    } else {
      printf("%c", c);
    }
  }

  fclose(fp1);

  printf("\n\nType the text and press '/' to save\n\n");

  while (1) {
    c = getchar();
    fputc(c, fp2);

    if (c == '/') {
      break;
    }
  }

end3:
  fclose(fp2);
}

void deletePage() {
  int page_num;
  char buf[0x100];

  printf("\n___________________________________________________\n");
  printf("\n%sDelete Page%s\n", BOLD, RESET);
  printf("___________________________________________________\n");

  printf("\nEnter Page Number to Delete: ");
  scanf("%d", &page_num);

  snprintf(buf, sizeof(buf), "./entries/%d.txt", page_num);

  if (remove(buf) == 0)
    printf("\nDeleted successfully\n");
  else
    printf("\nFile does not exist\n");
}

void searchJournal() {
  DIR *dir;
  struct dirent *ent;
  char folder_path[] = "path/to/folder"; // Replace with your folder path
  char word[MAX_WORD_LENGTH];
  int word_index = 0;
  int c;

  printf("Enter the word to search for: ");

  // Read the word character by character
  while ((c = getchar()) != '\n' && word_index < MAX_WORD_LENGTH - 1) {
    word[word_index++] = c;
  }
  word[word_index] = '\0'; // Null-terminate the word

  if ((dir = opendir(folder_path)) != NULL) {
    // Iterate through all files in the folder
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_type == DT_REG && strstr(ent->d_name, ".txt") != NULL) {
        // Open file
        FILE *file;
        char file_path[100];
        sprintf(file_path, "%s/%s", folder_path, ent->d_name);
        file = fopen(file_path, "r");

        if (file != NULL) {
          char line[256];
          int line_number = 1;
          int found = 0;

          // Search word in each line of the file
          while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, word) != NULL) {
              found = 1;
              break;
            }
            line_number++;
          }

          // Print file name if word is found
          if (found) {
            printf("Word found in file: %s\n", ent->d_name);
            printf("Line number: %d\n", line_number);
            printf("----------------------------\n");
          }

          // Close file
          fclose(file);
        }
      }
    }
    closedir(dir);
  } else {
    // Error opening directory
    perror("Error opening directory");
  }
}

void viewPage() {
  int page_num;
  char buf1[0x100];
  char c;

  printf("\nEnter Page Number: ");
  scanf("%d", &page_num);

  printf("\n___________________________________________________\n");
  printf("\n%sView Page %d%s\n", BOLD, page_num, RESET);
  printf("___________________________________________________\n\n");

  snprintf(buf1, sizeof(buf1), "./entries/%d.txt", page_num);
  FILE *fp1 = fopen(buf1, "r");

  if (fp1 == NULL) {
    printf("\nPage does not exist");
    goto end1;
  }

  c = 0;
  while (c != '/') {
    c = getc(fp1);
    if (c != '/') {
      printf("%c", c);
    }
  }

end1:
  fclose(fp1);
}

void duplicatePage() {
  int page_num;
  int dup_page_num;
  char buf1[0x100];
  char buf2[0x100];
  char c;
  int title_char;

  printf("\n___________________________________________________\n");
  printf("\n%sDuplicate Page%s\n", BOLD, RESET);
  printf("___________________________________________________\n\n");

  printf("\nEnter Original Page Number: ");
  scanf("%d", &page_num);

  printf("\nEnter Duplicate Page Number: ");
  scanf("%d", &dup_page_num);

  snprintf(buf1, sizeof(buf1), "./entries/%d.txt", page_num);
  FILE *fp1 = fopen(buf1, "r");

  snprintf(buf2, sizeof(buf2), "./entries/%d.txt", dup_page_num);
  FILE *fp2 = fopen(buf2, "w");

  if (fp1 == NULL) {
    printf("\nPage does not exist");
    goto end1;
  }

  c = 0;
  while (c != '/') {
    c = getc(fp1);
    fputc(c, fp2);
  }

  fseek(fp2, 0, SEEK_SET);
  title_char = dup_page_num + '0';
  fputc(title_char, fp2);

end1:
  fclose(fp1);
  fclose(fp2);
}

void tableOfContents() {
  DIR *dir;
  struct dirent *ent;
  char folder_path[] = "./entries"; // Replace with your folder path
  char **file_paths = NULL;
  char **titles = NULL;
  int file_count = 0;

  if ((dir = opendir(folder_path)) != NULL) {
    // Iterate through all files in the folder
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_type == DT_REG && strstr(ent->d_name, ".txt") != NULL) {
        // Open file
        FILE *file;
        char file_path[MAX_TITLE_LENGTH];
        sprintf(file_path, "%s/%s", folder_path, ent->d_name);
        file = fopen(file_path, "r");

        if (file != NULL) {
          // Read first line as title
          char title[MAX_TITLE_LENGTH];
          fgets(title, MAX_TITLE_LENGTH, file);
          title[strlen(title) - 1] = '\0'; // Remove newline character

          // Allocate memory and store file path
          file_paths = realloc(file_paths, (file_count + 1) * sizeof(char *));
          file_paths[file_count] = malloc(strlen(file_path) + 1);
          strcpy(file_paths[file_count], file_path);

          // Allocate memory and store title
          titles = realloc(titles, (file_count + 1) * sizeof(char *));
          titles[file_count] = malloc(strlen(title) + 1);
          strcpy(titles[file_count], title);

          // Increment file count
          file_count++;

          // Close file
          fclose(file);
        }
      }
    }
    closedir(dir);

    // Print table of contents
    printf("\n___________________________________________________\n");
    printf("\n%sTable of Contents%s\n", BOLD, RESET);
    printf("___________________________________________________\n\n");

    printf("Page: Title\n\n");

    for (int i = 0; i < file_count; i++) {
      printf("%s\n", titles[i]);
    }

    // Free dynamically allocated memory
    for (int i = 0; i < file_count; i++) {
      free(file_paths[i]);
      free(titles[i]);
    }
    free(file_paths);
    free(titles);
  } else {
    // Error opening directory
    perror("Error opening directory");
  }
}
