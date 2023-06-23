#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void addStudent(Node** head) 
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) 
    {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->next = NULL;

    printf("Enter student details:\n");

    printf("Last Name: ");
    scanf("%s", newNode->data.last_name);

    printf("First Name: ");
    scanf("%s", newNode->data.first_name);

    printf("Middle Name: ");
    scanf("%s", newNode->data.middle_name);

    printf("Birth Date (YYYY-MM-DD): ");
    while (scanf("%d-%d-%d", &newNode->data.birth_date.birth_year, &newNode->data.birth_date.birth_month, &newNode->data.birth_date.birth_day) != 3) 
    {
        printf("Invalid input. Please enter the birth date in the format (YYYY-MM-DD): ");
        while (getchar() != '\n');
    }

    printf("Group: ");
    scanf("%s", newNode->data.group);

    int genderChoice;
    do {
        printf("Gender (0 - Male, 1 - Female): ");
        while (scanf("%d", &genderChoice) != 1) 
        {
            printf("Invalid input. Please enter either 0 (Male) or 1 (Female): ");
            while (getchar() != '\n');
        }
    } while (genderChoice != 0 && genderChoice != 1);

    newNode->data.gender = (genderChoice == 0) ? MALE : FEMALE;

    if (*head == NULL) 
    {
        *head = newNode;
    } 
    else 
    {
        Node* current = *head;
        while (current->next != NULL) 
        {
            current = current->next;
        }
        current->next = newNode;
    }

    printf("Student added successfully.\n");
}

void removeStudent(Node** head) {
    if (*head == NULL) {
        printf("The student database is empty.\n");
        return;
    }

    char firstName[50];
    char lastName[50];

    printf("\nRemoving a student:\n");
    printf("Enter first name: ");
    scanf("%s", firstName);
    getchar();

    printf("Enter last name: ");
    fgets(lastName, sizeof(lastName), stdin);
    lastName[strcspn(lastName, "\n")] = '\0';

    Node* current = *head;
    Node* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->data.first_name, firstName) == 0 && strcmp(current->data.last_name, lastName) == 0) {
            if (prev == NULL) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            printf("The student has been successfully deleted.\n");
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("Student not found.\n");
}

void printStudent(const Student* student) 
{
    printf("Last Name: %s\n", student->last_name);
    printf("First Name: %s\n", student->first_name);
    printf("Middle Name: %s\n", student->middle_name);
    printf("Birth Date: %d-%02d-%02d\n", student->birth_date.birth_year, student->birth_date.birth_month, student->birth_date.birth_day);
    printf("Group: %s\n", student->group);
    printf("Gender: %s\n", student->gender == MALE ? "Male" : "Female");
}

void printStudentList(const Node* head) 
{
    if (head == NULL) 
    {
        printf("Student list is empty.\n");
        return;
    }
    
    const Node* current = head;
    while (current != NULL) {
        printStudent(&current->data);
        printf("---------------------\n");
        current = current->next;
    }
}

void loadStudentsFromFile(Node** head) 
{
    FILE* file = fopen("data.bin", "rb");
    if (file == NULL) 
    {
        printf("File not found. Starting with an empty student list.\n");
        return;
    }

    Node* current = NULL;
    while (1) 
    {
        Student student;
        size_t elements_read = fread(&student, sizeof(Student), 1, file);
        if (elements_read != 1) 
        {
            break;
        }

        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->data = student;
        newNode->next = NULL;

        if (*head == NULL) 
        {
            *head = newNode;
            current = newNode;
        } 
        else 
        {
            current->next = newNode;
            current = newNode;
        }
    }

    fclose(file);
    printf("Student list loaded from file.\n");
}

void saveStudentsToFile(const Node* head) {
    FILE* file = fopen("data.bin", "wb");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file for writing.\n");
        return;
    }

    const Node* current = head;
    int listSize = 0;

    // Подсчитываем количество студентов в списке
    while (current != NULL) {
        listSize++;
        current = current->next;
    }

    // Записываем размер списка студентов
    fwrite(&listSize, sizeof(int), 1, file);

    // Записываем каждого студента в файл
    current = head;
    while (current != NULL) {
        size_t elements_written = fwrite(&(current->data), sizeof(Student), 1, file);
        if (elements_written != 1) {
            fprintf(stderr, "Error writing student data to file.\n");
            fclose(file); // Закрываем файл перед выходом
            return;
        }
        current = current->next;
    }

    fclose(file);
}

void freeStudentList(Node** head) 
{
    Node* current = *head;
    while (current != NULL) 
    {
        Node* temp = current;
        current = current->next;
        free(temp);
    }

    *head = NULL;
}