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
    FILE* file = fopen("students.json", "r");
    if (file == NULL) {
        printf("Ошибка при открытии файла.\n");
        return;
    }

    // Чтение данных из файла в строку
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = (char*)malloc(file_size + 1);
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);

    // Парсинг JSON-строки и добавление данных в список
    json_object* root = json_tokener_parse(buffer);
    if (root == NULL) {
        printf("Ошибка при парсинге JSON.\n");
        free(buffer);
        return;
    }

    int length = json_object_array_length(root);
    for (int i = 0; i < length; i++) {
        json_object* student_obj = json_object_array_get_idx(root, i);
        if (student_obj != NULL && json_object_get_type(student_obj) == json_type_object) {
            json_object* last_name_obj = json_object_object_get(student_obj, "last_name");
            json_object* first_name_obj = json_object_object_get(student_obj, "first_name");
            json_object* middle_name_obj = json_object_object_get(student_obj, "middle_name");
            json_object* birth_date_obj = json_object_object_get(student_obj, "birth_date");
            json_object* group_obj = json_object_object_get(student_obj, "group");
            json_object* gender_obj = json_object_object_get(student_obj, "gender");

            if (last_name_obj != NULL && first_name_obj != NULL && middle_name_obj != NULL &&
                birth_date_obj != NULL && group_obj != NULL && gender_obj != NULL) {
                const char* last_name = json_object_get_string(last_name_obj);
                const char* first_name = json_object_get_string(first_name_obj);
                const char* middle_name = json_object_get_string(middle_name_obj);

                json_object* birth_date_year_obj = json_object_object_get(birth_date_obj, "birth_year");
                json_object* birth_date_month_obj = json_object_object_get(birth_date_obj, "birth_month");
                json_object* birth_date_day_obj = json_object_object_get(birth_date_obj, "birth_day");

                if (birth_date_year_obj != NULL && birth_date_month_obj != NULL && birth_date_day_obj != NULL) {
                    int birth_date_year = json_object_get_int(birth_date_year_obj);
                    int birth_date_month = json_object_get_int(birth_date_month_obj);
                    int birth_date_day = json_object_get_int(birth_date_day_obj);

                    const char* group = json_object_get_string(group_obj);
                    Gender gender = json_object_get_int(gender_obj) == 0 ? MALE : FEMALE;

                    Student student;
                    strcpy(student.last_name, last_name);
                    strcpy(student.first_name, first_name);
                    strcpy(student.middle_name, middle_name);
                    student.birth_date.birth_year = birth_date_year;
                    student.birth_date.birth_month = birth_date_month;
                    student.birth_date.birth_day = birth_date_day;
                    strcpy(student.group, group);
                    student.gender = gender;

                    addStudent(head);
                }
            }
        }
    }

    free(buffer);
    json_object_put(root);
}

void saveStudentsToFile(const Node* head) {
    FILE* file = fopen("data.json", "w");
    if (file == NULL) {
        printf("Ошибка при открытии файла.\n");
        return;
    }

    json_object* root = json_object_new_array();

    const Node* current = head;
    while (current != NULL) {
        const Student* student = &(current->data);
        json_object* student_obj = json_object_new_object();

        json_object_object_add(student_obj, "last_name", json_object_new_string(student->last_name));
        json_object_object_add(student_obj, "first_name", json_object_new_string(student->first_name));
        json_object_object_add(student_obj, "middle_name", json_object_new_string(student->middle_name));

        json_object* birth_date_obj = json_object_new_object();
        json_object_object_add(birth_date_obj, "year", json_object_new_int(student->birth_date.birth_year));
        json_object_object_add(birth_date_obj, "month", json_object_new_int(student->birth_date.birth_month));
        json_object_object_add(birth_date_obj, "day", json_object_new_int(student->birth_date.birth_day));
        json_object_object_add(student_obj, "birth_date", birth_date_obj);

        json_object_object_add(student_obj, "group", json_object_new_string(student->group));
        json_object_object_add(student_obj, "gender", json_object_new_int(student->gender));

        json_object_array_add(root, student_obj);

        current = current->next;
    }

    const char* json_str = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY);
    fputs(json_str, file);

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