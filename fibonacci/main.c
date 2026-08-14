#include <stdio.h>

int fibonacci(int n) {
    if (n <= 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }

    int prev = 0;
    int curr = 1;

    for (int i = 2; i <= n; i++) {
        int next = prev + curr;
        prev = curr;
        curr = next;
    }

    return curr;
}

int main() {
    int count = 0;

    printf("How many Fibonacci numbers? ");
    scanf("%d", &count);

    for (int i = 0; i < count; i++) {
        printf("%d ", fibonacci(i));
    }
    printf("\n");

    return 0;
}
