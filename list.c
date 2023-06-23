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

void loadStudentsFromFile(Node** head) {
    FILE* file = fopen("students.json", "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return;
    }

    if (fread(buffer, 1, file_size, file) != file_size) {
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

    cJSON* studentArray = cJSON_GetObjectItem(root, "students");
    if (studentArray == NULL || cJSON_GetArraySize(studentArray) == 0) {
        printf("No student data found in the file.\n");
        cJSON_Delete(root);
        return;
    }

    cJSON* studentObj = NULL;
    cJSON_ArrayForEach(studentObj, studentArray) {
        cJSON* lastNameItem = cJSON_GetObjectItem(studentObj, "last_name");
        cJSON* firstNameItem = cJSON_GetObjectItem(studentObj, "first_name");
        cJSON* middleNameItem = cJSON_GetObjectItem(studentObj, "middle_name");
        cJSON* birthYearItem = cJSON_GetObjectItem(studentObj, "birth_year");
        cJSON* birthMonthItem = cJSON_GetObjectItem(studentObj, "birth_month");
        cJSON* birthDayItem = cJSON_GetObjectItem(studentObj, "birth_day");
        cJSON* groupItem = cJSON_GetObjectItem(studentObj, "group");
        cJSON* genderItem = cJSON_GetObjectItem(studentObj, "gender");

        if (lastNameItem == NULL || firstNameItem == NULL || middleNameItem == NULL ||
            birthYearItem == NULL || birthMonthItem == NULL || birthDayItem == NULL ||
            groupItem == NULL || genderItem == NULL) {
            printf("Invalid student data.\n");
            continue;
        }

        Student newStudent;
        strncpy(newStudent.last_name, lastNameItem->valuestring, sizeof(newStudent.last_name) - 1);
        newStudent.last_name[sizeof(newStudent.last_name) - 1] = '\0';
        strncpy(newStudent.first_name, firstNameItem->valuestring, sizeof(newStudent.first_name) - 1);
        newStudent.first_name[sizeof(newStudent.first_name) - 1] = '\0';
        strncpy(newStudent.middle_name, middleNameItem->valuestring, sizeof(newStudent.middle_name) - 1);
        newStudent.middle_name[sizeof(newStudent.middle_name) - 1] = '\0';
        newStudent.birth_date.birth_year = birthYearItem->valueint;
        newStudent.birth_date.birth_month = birthMonthItem->valueint;
        newStudent.birth_date.birth_day = birthDayItem->valueint;
        strncpy(newStudent.group, groupItem->valuestring, sizeof(newStudent.group) - 1);
        newStudent.group[sizeof(newStudent.group) - 1] = '\0';
        newStudent.gender = (Gender)genderItem->valueint;

        Node* newNode = (Node*)malloc(sizeof(Node));
        if (newNode == NULL) {
            printf("Memory allocation failed.\n");
            continue;
        }

        newNode->data = newStudent;
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
    FILE* file = fopen("students.json", "w");
    if (file == NULL) {
        printf("Failed to create file.\n");
        return;
    }

    cJSON* root = cJSON_CreateObject();
    cJSON* studentArray = cJSON_CreateArray();

    const Node* current = head;
    while (current != NULL) {
        cJSON* studentObj = cJSON_CreateObject();
        cJSON_AddItemToArray(studentArray, studentObj);

        cJSON_AddStringToObject(studentObj, "last_name", current->data.last_name);
        cJSON_AddStringToObject(studentObj, "first_name", current->data.first_name);
        cJSON_AddStringToObject(studentObj, "middle_name", current->data.middle_name);
        cJSON_AddNumberToObject(studentObj, "birth_year", current->data.birth_date.birth_year);
        cJSON_AddNumberToObject(studentObj, "birth_month", current->data.birth_date.birth_month);
        cJSON_AddNumberToObject(studentObj, "birth_day", current->data.birth_date.birth_day);
        cJSON_AddStringToObject(studentObj, "group", current->data.group);
        cJSON_AddNumberToObject(studentObj, "gender", current->data.gender);

        current = current->next;
    }

    cJSON_AddItemToObject(root, "students", studentArray);

    char* jsonStr = cJSON_Print(root);
    fprintf(file, "%s", jsonStr);
    fclose(file);

    cJSON_Delete(root);
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