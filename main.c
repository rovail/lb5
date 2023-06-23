#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main() 
{
    Node* studentList = NULL;
    int choice;

    loadStudentsFromFile(&studentList);

    while (1) {
        printf("Menu:\n");
        printf("1. Print student list\n");
        printf("2. Add new student\n");
        printf("3. Remove existing student\n");
        printf("4. Save student list to file\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice < 1 || choice > 5) {
            printf("Invalid choice. Please try again.\n");
            continue;
        }

        switch (choice) {
            case 1:
                printf("\nStudent List:\n");
                printStudentList(studentList);
                break;
            case 2:
                addStudent(&studentList);
                break;
            case 3:
                removeStudent(&studentList);
                break;
            case 4:
                saveStudentsToFile(studentList);
                printf("Student list saved to file.\n");
                break;
            case 5:
                saveStudentsToFile(studentList);
                freeStudentList(&studentList);
                printf("Exiting program.\n");
                exit(0);
        }
        printf("\n");
    }

    return 0;
}