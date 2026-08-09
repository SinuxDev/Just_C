#include <stdio.h>

int main() {
   
   int age = 0;
   float gpa = 0.0f;
   char grade = '\0';
   char name[30] = "";

   printf("Enter your age : ");
   scanf("%d", &age);

   printf("%d", age);

   return 0;
}