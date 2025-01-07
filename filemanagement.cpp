#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h> // For struct stat and stat()

// File structure to hold file information
typedef struct {
    char name[256];
    off_t size;
} FileInfo;

// Function to compare files by size in ascending order
int compareAscending(const void *a, const void *b) {
    return ((FileInfo *)a)->size - ((FileInfo *)b)->size;
}

// Function to compare files by size in descending order
int compareDescending(const void *a, const void *b) {
    return ((FileInfo *)b)->size - ((FileInfo *)a)->size;
}

// Function to create a file
void createFile(const char *filename) {
    struct stat buffer;
    // Check if the file already exists
    if (stat(filename, &buffer) == 0) {
        printf("Error: File already exists: %s\n", filename);
        return;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to create file");
        return;
    }

    fprintf(file, "Hello, this is a new file.\n");
    fclose(file);
    printf("File created successfully: %s\n", filename);
}

// Function to read a file
void readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    fclose(file);
}

// Function to delete a file
void deleteFile(const char *filename) {
    if (remove(filename) == 0) {
        printf("File deleted successfully: %s\n", filename);
    } else {
        perror("Failed to delete file");
    }
}

// Function to rename a file
void renameFile(const char *oldName, const char *newName) {
    if (rename(oldName, newName) == 0) {
        printf("File renamed successfully from %s to %s\n", oldName, newName);
    } else {
        perror("Failed to rename file");
    }
}

// Function to copy a file
void copyFile(const char *source, const char *destination) {
    FILE *src = fopen(source, "r");
    if (src == NULL) {
        perror("Failed to open source file");
        return;
    }
    
    FILE *dest = fopen(destination, "w");
    if (dest == NULL) {
        perror("Failed to open destination file");
        fclose(src);
        return;
    }
    
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytesRead, dest);
    }
    
    fclose(src);
    fclose(dest);
    printf("File copied successfully from %s to %s\n", source, destination);
}

// Function to move a file
void moveFile(const char *source, const char *destination) {
    // Copy the file to the new location
    copyFile(source, destination);
    // Delete the original file if the copy was successful
    deleteFile(source);
}

// Function to list files in a directory
void listFiles(const char *directory) {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("Failed to open directory");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
}

// Function to list and sort files in a directory
void sortFiles(const char *directory, int order) {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("Failed to open directory");
        return;
    }

    struct dirent *entry;
    struct stat fileStat;
    FileInfo files[100];
    int count = 0;

    // Read files in the directory
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Only regular files
            char fullPath[512];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, entry->d_name);
            if (stat(fullPath, &fileStat) == 0) {
                strncpy(files[count].name, entry->d_name, sizeof(files[count].name));
                files[count].size = fileStat.st_size;
                count++;
            }
        }
    }
    closedir(dir);

    // Sort files based on size
    if (order == 1) {
        qsort(files, count, sizeof(FileInfo), compareAscending);
    } else if (order == 2) {
        qsort(files, count, sizeof(FileInfo), compareDescending);
    }

    // Print sorted files
    printf("Sorted Files:\n");
    for (int i = 0; i < count; i++) {
        printf("%s - %ld bytes\n", files[i].name, files[i].size);
    }
}

// Main function with updated menu
int main() {
    int choice;
    char filename[100], newname[100], destination[100];
    
    while (1) {
        printf("\nFile Management System\n");
        printf("1. Create File\n");
        printf("2. Read File\n");
        printf("3. Delete File\n");
        printf("4. Rename File\n");
        printf("5. Copy File\n");
        printf("6. Move File\n");
        printf("7. List Files in Directory\n");
        printf("8. Sort Files by Size\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice. Please enter a number between 1 and 9.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                printf("Enter filename to create: ");
                scanf("%s", filename);
                createFile(filename);
                break;
            case 2:
                printf("Enter filename to read: ");
                scanf("%s", filename);
                readFile(filename);
                break;
            case 3:
                printf("Enter filename to delete: ");
                scanf("%s", filename);
                deleteFile(filename);
                break;
            case 4:
                printf("Enter current filename: ");
                scanf("%s", filename);
                printf("Enter new filename: ");
                scanf("%s", newname);
                renameFile(filename, newname);
                break;
            case 5:
                printf("Enter source filename to copy: ");
                scanf("%s", filename);
                printf("Enter destination filename: ");
                scanf("%s", destination);
                copyFile(filename, destination);
                break;
            case 6:
                printf("Enter source filename to move: ");
                scanf("%s", filename);
                printf("Enter destination filename: ");
                scanf("%s", destination);
                moveFile(filename, destination);
                break;
            case 7:
                printf("Listing files in current directory:\n");
                listFiles(".");
                break;
            case 8:
                printf("Sort by:\n");
                printf("1. Ascending Order (Smallest to Largest)\n");
                printf("2. Descending Order (Largest to Smallest)\n");
                printf("Enter your choice: ");
                int sortChoice;
                scanf("%d", &sortChoice);
                if (sortChoice == 1 || sortChoice == 2) {
                    sortFiles(".", sortChoice);
                } else {
                    printf("Invalid choice. Please select 1 or 2.\n");
                }
                break;
            case 9:
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice. Please enter a number between 1 and 9.\n");
        }
    }
    return 0;
}