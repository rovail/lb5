#ifndef LIST_H
#define LIST_H

typedef enum {
    MALE,
    FEMALE
} Gender;

typedef struct {
    int birth_year;
    int birth_month;
    int birth_day;
} BirthDate;

typedef struct {
    char last_name[50];
    char first_name[50];
    char middle_name[50];
    BirthDate birth_date;
    char group[10];
    Gender gender;
} Student;

typedef struct Node {
    Student data;
    struct Node* next;
} Node;

void loadStudentsFromFile(Node** head);
void saveStudentsToFile(const Node* head);
void addStudent(Node** head);
void removeStudent(Node** head);
void freeStudentList(Node** head);
void printStudent(const Student* student);
void printStudentList(const Node* head);

#endif