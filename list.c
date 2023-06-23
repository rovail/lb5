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

void addJsonStudent(Node** head, Node* newNode)
{
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
    FILE* file = fopen("students.json", "r");
    if (file == NULL)
    {
        printf("Failed to open file.\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* jsonString = (char*)malloc(fileSize + 1);
    if (jsonString == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(file);
        return;
    }

    size_t bytesRead = fread(jsonString, 1, fileSize, file);
    fclose(file);

    if (bytesRead != (size_t)fileSize)
    {
        printf("Failed to read file.\n");
        free(jsonString);
        return;
    }

    jsonString[fileSize] = '\0';

    cJSON* root = cJSON_Parse(jsonString);
    if (root == NULL || !cJSON_IsArray(root))
    {
        printf("Invalid JSON format.\n");
        free(jsonString);
        cJSON_Delete(root);
        return;
    }

    cJSON* student;
    cJSON_ArrayForEach(student, root)
    {
        cJSON* lastName = cJSON_GetObjectItemCaseSensitive(student, "last_name");
        cJSON* firstName = cJSON_GetObjectItemCaseSensitive(student, "first_name");
        cJSON* middleName = cJSON_GetObjectItemCaseSensitive(student, "middle_name");
        cJSON* birthDate = cJSON_GetObjectItemCaseSensitive(student, "birth_date");
        cJSON* group = cJSON_GetObjectItemCaseSensitive(student, "group");
        cJSON* gender = cJSON_GetObjectItemCaseSensitive(student, "gender");

        if (!cJSON_IsString(lastName) || !cJSON_IsString(firstName) || !cJSON_IsString(middleName) ||
            !cJSON_IsObject(birthDate) || !cJSON_IsString(group) || !cJSON_IsNumber(gender))
        {
            printf("Invalid student data.\n");
            continue;
        }

        BirthDate birth;
        cJSON* birthYear = cJSON_GetObjectItemCaseSensitive(birthDate, "birth_year");
        cJSON* birthMonth = cJSON_GetObjectItemCaseSensitive(birthDate, "birth_month");
        cJSON* birthDay = cJSON_GetObjectItemCaseSensitive(birthDate, "birth_day");

        if (!cJSON_IsNumber(birthYear) || !cJSON_IsNumber(birthMonth) || !cJSON_IsNumber(birthDay))
        {
            printf("Invalid birth date.\n");
            continue;
        }

        birth.birth_year = birthYear->valueint;
        birth.birth_month = birthMonth->valueint;
        birth.birth_day = birthDay->valueint;

        Student newStudent;
        strcpy(newStudent.last_name, lastName->valuestring);
        strcpy(newStudent.first_name, firstName->valuestring);
        strcpy(newStudent.middle_name, middleName->valuestring);
        newStudent.birth_date = birth;
        strcpy(newStudent.group, group->valuestring);

        int genderValue = gender->valueint;
        if (genderValue == 0)
            newStudent.gender = MALE;
        else if (genderValue == 1)
            newStudent.gender = FEMALE;
        else
        {
            printf("Invalid gender value.\n");
            continue;
        }

        addJsonStudent(head, &newStudent);
    }

    printf("Students loaded from file successfully.\n");

    free(jsonString);
    cJSON_Delete(root);
}

void saveStudentsToFile(const Node* head)
{
    cJSON* root = cJSON_CreateArray(); // Create the root JSON array

    // Iterate over the student list and create a JSON object for each student
    const Node* current = head;
    while (current != NULL)
    {
        cJSON* studentObject = cJSON_CreateObject(); // Create a JSON object for the student

        // Fill the JSON object fields with data from the Student structure
        cJSON_AddStringToObject(studentObject, "last_name", current->data.last_name);
        cJSON_AddStringToObject(studentObject, "first_name", current->data.first_name);
        cJSON_AddStringToObject(studentObject, "middle_name", current->data.middle_name);

        cJSON* birthDateObject = cJSON_CreateObject(); // Create a JSON object for the birth date
        cJSON_AddNumberToObject(birthDateObject, "birth_year", current->data.birth_date.birth_year);
        cJSON_AddNumberToObject(birthDateObject, "birth_month", current->data.birth_date.birth_month);
        cJSON_AddNumberToObject(birthDateObject, "birth_day", current->data.birth_date.birth_day);
        cJSON_AddItemToObject(studentObject, "birth_date", birthDateObject);

        cJSON_AddStringToObject(studentObject, "group", current->data.group);
        
        // Set the gender field as a number
        cJSON_AddNumberToObject(studentObject, "gender", (int)current->data.gender);

        cJSON_AddItemToArray(root, studentObject); // Add the student object to the root array

        current = current->next;
    }

    char* jsonString = cJSON_Print(root); // Convert the root JSON array to a string

    cJSON_Delete(root); // Free the memory occupied by the root JSON array

    // Write the JSON string to the "students.json" file
    FILE* file = fopen("students.json", "w");
    if (file != NULL)
    {
        fputs(jsonString, file);
        fclose(file);
        printf("Student list saved to file successfully.\n");
    }
    else
    {
        printf("Failed to open file for writing.\n");
    }

    free(jsonString); // Free the memory occupied by the JSON string
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