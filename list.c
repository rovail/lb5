#include "list.h"
#include "cJSON.h"
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

void removeStudent(Node** head) 
{
    if (*head == NULL) 
    {
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
    while (current != NULL) 
    {
        if (strcmp(current->data.first_name, firstName) == 0 && strcmp(current->data.last_name, lastName) == 0) 
        {
            if (prev == NULL) 
            {
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
    //printf("Middle Name: %s\n", student->middle_name);
    //printf("Birth Date: %d-%02d-%02d\n", student->birth_date.birth_year, student->birth_date.birth_month, student->birth_date.birth_day);
    printf("Group: %s\n", student->group);
    //printf("Gender: %s\n", student->gender == MALE ? "Male" : "Female");
}

void printStudentList(const Node* head) 
{
    if (head == NULL) 
    {
        printf("Student list is empty.\n");
        return;
    }
    
    const Node* current = head;
    while (current != NULL) 
    {
        printStudent(&current->data);
        printf("---------------------\n");
        current = current->next;
    }
}

void loadStudentsFromFile(Node** head) 
{
    cJSON* studentsArray = NULL;

    FILE* file = fopen("students.json", "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) 
    {
        printf("Memory allocation failed.\n");
        fclose(file);
        return;
    }

    if (fread(buffer, 1, file_size, file) != file_size) 
    {
        printf("Failed to read file.\n");
        fclose(file);
        free(buffer);
        return;
    }
    buffer[file_size] = '\0';
    fclose(file);

    cJSON* root = cJSON_Parse(buffer);
    free(buffer);

    if (root == NULL) {
        printf("Invalid JSON data.\n");
        return;
    }

    studentsArray = cJSON_GetObjectItemCaseSensitive(root, "students");
    if (!cJSON_IsArray(studentsArray)) {
        cJSON_Delete(root);
        printf("Invalid JSON data.\n");
        return;
    }

    cJSON* studentObject = NULL;
    cJSON_ArrayForEach(studentObject, studentsArray) {
        Student student;
        cJSON* item = NULL;

        item = cJSON_GetObjectItemCaseSensitive(studentObject, "last_name");
        if (!cJSON_IsString(item) || item->valuestring == NULL) {
            continue;
        }
        strncpy(student.last_name, item->valuestring, sizeof(student.last_name));

        item = cJSON_GetObjectItemCaseSensitive(studentObject, "first_name");
        if (!cJSON_IsString(item) || item->valuestring == NULL) {
            continue;
        }
        strncpy(student.first_name, item->valuestring, sizeof(student.first_name));

        item = cJSON_GetObjectItemCaseSensitive(studentObject, "middle_name");
        if (!cJSON_IsString(item) || item->valuestring == NULL) {
            continue;
        }
        strncpy(student.middle_name, item->valuestring, sizeof(student.middle_name));

        item = cJSON_GetObjectItemCaseSensitive(studentObject, "birth_date");
        if (!cJSON_IsObject(item)) {
            continue;
        }
        cJSON* birthDateObject = item;

        item = cJSON_GetObjectItemCaseSensitive(birthDateObject, "birth_year");
        if (!cJSON_IsNumber(item)) {
            continue;
        }
        student.birth_date.birth_year = item->valueint;

        item = cJSON_GetObjectItemCaseSensitive(birthDateObject, "birth_month");
        if (!cJSON_IsNumber(item)) {
            continue;
        }
        student.birth_date.birth_month = item->valueint;

        item = cJSON_GetObjectItemCaseSensitive(birthDateObject, "birth_day");
        if (!cJSON_IsNumber(item)) {
            continue;
        }
        student.birth_date.birth_day = item->valueint;

        item = cJSON_GetObjectItemCaseSensitive(studentObject, "group");
        if (!cJSON_IsString(item) || item->valuestring == NULL) {
            continue;
        }
        strncpy(student.group, item->valuestring, sizeof(student.group));

        item = cJSON_GetObjectItemCaseSensitive(studentObject, "gender");
        if (!cJSON_IsNumber(item)) {
            continue;
        }
        student.gender = (item->valueint == 0) ? MALE : FEMALE;

        Node* newNode = (Node*)malloc(sizeof(Node));
        if (newNode == NULL) {
            printf("Memory allocation failed.\n");
            continue;
        }
        newNode->data = student;
        newNode->next = NULL;

        if (*head == NULL) {
            *head = newNode;
        } else {
            Node* current = *head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    cJSON_Delete(root);
    printf("Students loaded from file successfully.\n");
}

void saveStudentsToFile(const Node* head) {
    cJSON* root = cJSON_CreateObject();
    cJSON* studentsArray = cJSON_CreateArray();

    const Node* current = head;
    while (current != NULL) {
        const Student* student = &(current->data);

        cJSON* studentObject = cJSON_CreateObject();
        cJSON_AddItemToArray(studentsArray, studentObject);

        cJSON_AddStringToObject(studentObject, "last_name", student->last_name);
        cJSON_AddStringToObject(studentObject, "first_name", student->first_name);
        cJSON_AddStringToObject(studentObject, "middle_name", student->middle_name);

        cJSON* birthDateObject = cJSON_CreateObject();
        cJSON_AddItemToObject(studentObject, "birth_date", birthDateObject);
        cJSON_AddNumberToObject(birthDateObject, "birth_year", student->birth_date.birth_year);
        cJSON_AddNumberToObject(birthDateObject, "birth_month", student->birth_date.birth_month);
        cJSON_AddNumberToObject(birthDateObject, "birth_day", student->birth_date.birth_day);

        cJSON_AddStringToObject(studentObject, "group", student->group);
        cJSON_AddNumberToObject(studentObject, "gender", student->gender);

        current = current->next;
    }

    cJSON_AddItemToObject(root, "students", studentsArray);

    char* jsonString = cJSON_Print(root);
    cJSON_Delete(root);

    FILE* file = fopen("students.json", "w");
    if (file == NULL) {
        printf("Failed to create file.\n");
        free(jsonString);
        return;
    }

    fputs(jsonString, file);
    fclose(file);
    free(jsonString);

    printf("Student list saved to file successfully.\n");
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