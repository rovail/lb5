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

    cJSON* students_array = cJSON_GetObjectItem(root, "students");
    if (students_array == NULL || !cJSON_IsArray(students_array)) {
        cJSON_Delete(root);
        printf("Invalid students data.\n");
        return;
    }

    int num_students = cJSON_GetArraySize(students_array);
    for (int i = 0; i < num_students; i++) {
        cJSON* student_item = cJSON_GetArrayItem(students_array, i);
        if (student_item != NULL && cJSON_IsObject(student_item)) {
            cJSON* last_name_item = cJSON_GetObjectItem(student_item, "last_name");
            cJSON* first_name_item = cJSON_GetObjectItem(student_item, "first_name");
            cJSON* middle_name_item = cJSON_GetObjectItem(student_item, "middle_name");
            cJSON* birth_date_item = cJSON_GetObjectItem(student_item, "birth_date");
            cJSON* group_item = cJSON_GetObjectItem(student_item, "group");
            cJSON* gender_item = cJSON_GetObjectItem(student_item, "gender");

            if (last_name_item != NULL && cJSON_IsString(last_name_item) &&
                first_name_item != NULL && cJSON_IsString(first_name_item) &&
                middle_name_item != NULL && cJSON_IsString(middle_name_item) &&
                birth_date_item != NULL && cJSON_IsObject(birth_date_item) &&
                group_item != NULL && cJSON_IsString(group_item) &&
                gender_item != NULL && cJSON_IsNumber(gender_item)) {
                // Создание нового узла списка и заполнение данных студента
                Node* newNode = (Node*)malloc(sizeof(Node));
                if (newNode != NULL) {
                    snprintf(newNode->data.last_name, sizeof(newNode->data.last_name), "%s", last_name_item->valuestring);
                    snprintf(newNode->data.first_name, sizeof(newNode->data.first_name), "%s", first_name_item->valuestring);
                    snprintf(newNode->data.middle_name, sizeof(newNode->data.middle_name), "%s", middle_name_item->valuestring);

                    cJSON* year_item = cJSON_GetObjectItem(birth_date_item, "year");
                    cJSON* month_item = cJSON_GetObjectItem(birth_date_item, "month");
                    cJSON* day_item = cJSON_GetObjectItem(birth_date_item, "day");
                    if (year_item != NULL && cJSON_IsNumber(year_item) &&
                        month_item != NULL && cJSON_IsNumber(month_item) &&
                        day_item != NULL && cJSON_IsNumber(day_item)) {
                        newNode->data.birth_date.birth_year = year_item->valueint;
                        newNode->data.birth_date.birth_month = month_item->valueint;
                        newNode->data.birth_date.birth_day = day_item->valueint;
                    }

                    snprintf(newNode->data.group, sizeof(newNode->data.group), "%s", group_item->valuestring);

                    int gender_value = gender_item->valueint;
                    newNode->data.gender = (gender_value == 0) ? MALE : FEMALE;

                    newNode->next = NULL;

                    // Добавление студента в список
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
            }
        }
    }

    cJSON_Delete(root);

    printf("Students loaded from file successfully.\n");
}

void saveStudentsToFile(const Node* head) {
    cJSON* root = cJSON_CreateArray(); // Создаем массив JSON-объектов

    const Node* current = head;
    while (current != NULL) {
        cJSON* studentObject = cJSON_CreateObject(); // Создаем JSON-объект для каждого студента

        // Заполняем JSON-объект данными студента
        cJSON_AddStringToObject(studentObject, "last_name", current->data.last_name);
        cJSON_AddStringToObject(studentObject, "first_name", current->data.first_name);
        cJSON_AddStringToObject(studentObject, "middle_name", current->data.middle_name);
        cJSON_AddNumberToObject(studentObject, "birth_year", current->data.birth_date.birth_year);
        cJSON_AddNumberToObject(studentObject, "birth_month", current->data.birth_date.birth_month);
        cJSON_AddNumberToObject(studentObject, "birth_day", current->data.birth_date.birth_day);
        cJSON_AddStringToObject(studentObject, "group", current->data.group);
        cJSON_AddNumberToObject(studentObject, "gender", current->data.gender);

        cJSON_AddItemToArray(root, studentObject); // Добавляем JSON-объект студента в массив

        current = current->next;
    }

    FILE* file = fopen("students.json", "a"); // Открываем файл в режиме добавления
    if (file == NULL) {
        printf("Failed to open file for appending.\n");
        cJSON_Delete(root); // Освобождаем память, если не удалось открыть файл
        return;
    }

    char* jsonString = cJSON_Print(root); // Преобразуем JSON-данные в строку

    fprintf(file, "%s\n", jsonString); // Записываем строку в файл
    fclose(file);
    cJSON_Delete(root);
    free(jsonString);

    printf("Student list saved to file.\n");
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