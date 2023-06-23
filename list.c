#include "list.h"
#include "include/json-c/json.h"
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
    FILE* file = fopen("data.json", "r");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(sizeof(char) * file_size);
    fread(buffer, sizeof(char), file_size, file);
    fclose(file);

    struct json_object* json = json_tokener_parse(buffer);
    free(buffer);

    int studentCount = json_object_array_length(json);
    for (int i = 0; i < studentCount; i++)
    {
        struct json_object* studentObj = json_object_array_get_idx(json, i);

        struct Student* student = (struct Student*)malloc(sizeof(struct Student));
        student->next = NULL;

        json_object_object_foreach(studentObj, key, val)
        {
            if (strcmp(key, "Name") == 0)
            {
                strcpy(student->firstName, json_object_get_string(val));
            }
            else if (strcmp(key, "Surname") == 0)
            {
                strcpy(student->lastName, json_object_get_string(val));
            }
            else if (strcmp(key, "Middle name") == 0)
            {
                strcpy(student->middleName, json_object_get_string(val));
            }
            else if (strcmp(key, "Date of Birth") == 0)
            {
                struct json_object* dobObj = val;
                student->birthDate.day = json_object_get_int(json_object_array_get_idx(dobObj, 0));
                student->birthDate.month = json_object_get_int(json_object_array_get_idx(dobObj, 1));
                student->birthDate.year = json_object_get_int(json_object_array_get_idx(dobObj, 2));
            }
            else if (strcmp(key, "Group") == 0)
            {
                strcpy(student->group, json_object_get_string(val));
            }
            else if (strcmp(key, "Gender") == 0)
            {
                const char* genderStr = json_object_get_string(val);
                if (strcmp(genderStr, "Male") == 0)
                {
                    student->gender = MALE;
                }
                else
                {
                    student->gender = FEMALE;
                }
            }
        }

        if (head == NULL)
        {
            head = student;
        }
        else
        {
            struct Student* current = head;
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = student;
        }
    }
    json_object_put(json);

    printf("Student data successfully loaded from 'data.json' file.\n");
}

void saveStudentsToFile(const Node* head) {
    struct json_object* json = json_object_new_array();

    struct Student* current = head;
    while (current != NULL)

    {
        struct json_object* studentObj = json_object_new_object();

        json_object_object_add(studentObj, "Name", json_object_new_string(current->firstName));
        json_object_object_add(studentObj, "Surname", json_object_new_string(current->lastName));
        json_object_object_add(studentObj, "Middle name", json_object_new_string(current->middleName));

        struct json_object* dobObj = json_object_new_array();
        json_object_array_add(dobObj, json_object_new_int(current->birthDate.day));
        json_object_array_add(dobObj, json_object_new_int(current->birthDate.month));
        json_object_array_add(dobObj, json_object_new_int(current->birthDate.year));
        json_object_object_add(studentObj, "Date of Birth", dobObj);

        json_object_object_add(studentObj, "Group", json_object_new_string(current->group));

        const char* genderStr = (current->gender == MALE) ? "Male" : "Female";
        json_object_object_add(studentObj, "Gender", json_object_new_string(genderStr));

        json_object_array_add(json, studentObj);

        current = current->next;
    }

    FILE* file = fopen("data.json", "w");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    fprintf(file, "%s", json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY));
    fclose(file);

    json_object_put(json);

    printf("Data successfully saved in file 'date.json'.\n");
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