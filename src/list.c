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

void addJsonStudent(Node** head, const Student* student) 
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = *student;
    newNode->next = NULL;

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
}

void loadStudentsFromFile(Node** head) 
{
    FILE* file = fopen("data.json", "r");
    if (file == NULL) 
    {
        printf("Error opening file.\n");
        return;
    }

    char buffer[1024];
    fread(buffer, sizeof(buffer), 1, file);
    fclose(file);

    struct json_object* root = json_tokener_parse(buffer);
    int arrayLength = json_object_array_length(root);

    for (int i = 0; i < arrayLength; i++) 
    {
        struct json_object* studentObj = json_object_array_get_idx(root, i);

        struct json_object* lastNameObj;
        struct json_object* firstNameObj;
        struct json_object* middleNameObj;
        struct json_object* birthYearObj;
        struct json_object* birthMonthObj;
        struct json_object* birthDayObj;
        struct json_object* groupObj;
        struct json_object* genderObj;

        json_object_object_get_ex(studentObj, "last_name", &lastNameObj);
        json_object_object_get_ex(studentObj, "first_name", &firstNameObj);
        json_object_object_get_ex(studentObj, "middle_name", &middleNameObj);
        json_object_object_get_ex(studentObj, "birth_year", &birthYearObj);
        json_object_object_get_ex(studentObj, "birth_month", &birthMonthObj);
        json_object_object_get_ex(studentObj, "birth_day", &birthDayObj);
        json_object_object_get_ex(studentObj, "group", &groupObj);
        json_object_object_get_ex(studentObj, "gender", &genderObj);


        Student student;
        strncpy(student.last_name, json_object_get_string(lastNameObj), sizeof(student.last_name));
        strncpy(student.first_name, json_object_get_string(firstNameObj), sizeof(student.first_name));
        strncpy(student.middle_name, json_object_get_string(middleNameObj), sizeof(student.middle_name));
        student.birth_date.birth_year = json_object_get_int(birthYearObj);
        student.birth_date.birth_month = json_object_get_int(birthMonthObj);
        student.birth_date.birth_day = json_object_get_int(birthDayObj);
        strncpy(student.group, json_object_get_string(groupObj), sizeof(student.group));
        student.gender = json_object_get_int(genderObj);

        addJsonStudent(head, &student);
    }

    json_object_put(root);
}

void saveStudentsToFile(const Node* head) 
{
    struct json_object* root = json_object_new_array();
    const Node* current = head;

    while (current != NULL) 
    {
        struct json_object* studentObj = json_object_new_object();

        json_object_object_add(studentObj, "last_name", json_object_new_string(current->data.last_name));
        json_object_object_add(studentObj, "first_name", json_object_new_string(current->data.first_name));
        json_object_object_add(studentObj, "middle_name", json_object_new_string(current->data.middle_name));
        json_object_object_add(studentObj, "birth_year", json_object_new_int(current->data.birth_date.birth_year));
        json_object_object_add(studentObj, "birth_month", json_object_new_int(current->data.birth_date.birth_month));
        json_object_object_add(studentObj, "birth_day", json_object_new_int(current->data.birth_date.birth_day));
        json_object_object_add(studentObj, "group", json_object_new_string(current->data.group));
        json_object_object_add(studentObj, "gender", json_object_new_int(current->data.gender));

        json_object_array_add(root, studentObj);

        current = current->next;
    }

    FILE* file = fopen("data.json", "w");
    if (file == NULL) 
    {
        printf("Error opening file.\n");
        return;
    }

    fprintf(file, "%s\n", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));
    fclose(file);

    json_object_put(root);
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