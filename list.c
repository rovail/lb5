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

void addJsonStudent(Node** head, Node* newNode) {
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

    cJSON* students = cJSON_GetObjectItem(root, "students");
    if (students == NULL || !cJSON_IsArray(students)) {
        printf("Invalid student data.\n");
        cJSON_Delete(root);
        return;
    }

    cJSON* student;
    cJSON_ArrayForEach(student, students) {
        cJSON* last_name = cJSON_GetObjectItem(student, "last_name");
        cJSON* first_name = cJSON_GetObjectItem(student, "first_name");
        cJSON* middle_name = cJSON_GetObjectItem(student, "middle_name");
        cJSON* birth_year = cJSON_GetObjectItem(student, "birth_year");
        cJSON* birth_month = cJSON_GetObjectItem(student, "birth_month");
        cJSON* birth_day = cJSON_GetObjectItem(student, "birth_day");
        cJSON* group = cJSON_GetObjectItem(student, "group");
        cJSON* gender = cJSON_GetObjectItem(student, "gender");

        if (!cJSON_IsString(last_name) || !cJSON_IsString(first_name) || !cJSON_IsString(middle_name) ||
            !cJSON_IsNumber(birth_year) || !cJSON_IsNumber(birth_month) || !cJSON_IsNumber(birth_day) ||
            !cJSON_IsString(group) || !cJSON_IsNumber(gender)) {
            printf("Invalid student data.\n");
            cJSON_Delete(root);
            return;
        }

        Student newStudent;
        strcpy(newStudent.last_name, last_name->valuestring);
        strcpy(newStudent.first_name, first_name->valuestring);
        strcpy(newStudent.middle_name, middle_name->valuestring);
        newStudent.birth_date.birth_year = birth_year->valueint;
        newStudent.birth_date.birth_month = birth_month->valueint;
        newStudent.birth_date.birth_day = birth_day->valueint;
        strcpy(newStudent.group, group->valuestring);
        newStudent.gender = (Gender)gender->valueint;

        Node* newNode = (Node*)malloc(sizeof(Node));
        if (newNode == NULL) {
            printf("Memory allocation failed.\n");
            cJSON_Delete(root);
            return;
        }
        newNode->data = newStudent;
        newNode->next = NULL;

        addJsonStudent(head, newNode);
    }

    printf("Students loaded from file successfully.\n");

    cJSON_Delete(root);
}

void saveStudentsToFile(const Node* head) {
    FILE* file = fopen("students.json", "w");
    if (file == NULL) {
        printf("Failed to create file.\n");
        return;
    }

    cJSON* root = cJSON_CreateObject();
    cJSON* studentsArray = cJSON_CreateArray();

    const Node* current = head;
    while (current != NULL) {
        cJSON* studentObject = cJSON_CreateObject();
        cJSON_AddItemToObject(studentObject, "last_name", cJSON_CreateString(current->data.last_name));
        cJSON_AddItemToObject(studentObject, "first_name", cJSON_CreateString(current->data.first_name));
        cJSON_AddItemToObject(studentObject, "middle_name", cJSON_CreateString(current->data.middle_name));
        cJSON_AddItemToObject(studentObject, "birth_year", cJSON_CreateNumber(current->data.birth_date.birth_year));
        cJSON_AddItemToObject(studentObject, "birth_month", cJSON_CreateNumber(current->data.birth_date.birth_month));
        cJSON_AddItemToObject(studentObject, "birth_day", cJSON_CreateNumber(current->data.birth_date.birth_day));
        cJSON_AddItemToObject(studentObject, "group", cJSON_CreateString(current->data.group));
        cJSON_AddItemToObject(studentObject, "gender", cJSON_CreateNumber(current->data.gender));

        cJSON_AddItemToArray(studentsArray, studentObject);

        current = current->next;
    }

    cJSON_AddItemToObject(root, "students", studentsArray);

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