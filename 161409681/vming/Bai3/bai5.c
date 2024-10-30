#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>

#define BUFFER_SIZE 1000  // Kích thước của buffer để lưu chuỗi Collatz

// Hàm kiểm tra số nguyên dương
int is_positive_integer(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') return 0;
    }
    return 1;
}

// Hàm Collatz, lưu chuỗi vào buffer
void collatz_sequence(int n, int *buffer) {
    int index = 0;
    while (n != 1) {
        buffer[index++] = n;
        if (n % 2 == 0) {
            n /= 2;
        } else {
            n = 3 * n + 1;
        }
    }
    buffer[index++] = 1; // Thêm số 1 vào chuỗi
    buffer[index] = -1;  // Đánh dấu kết thúc chuỗi bằng -1
}

int main(int argc, char *argv[]) {
    // Kiểm tra đầu vào
    if (argc != 2 || !is_positive_integer(argv[1])) {
        fprintf(stderr, "Khong hop le %s\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Nhap so nguyen duong.\n");
        return 1;
    }

    // Tạo bộ nhớ chia sẻ
    int *buffer = mmap(NULL, BUFFER_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Tạo tiến trình con để tính toán chuỗi Collatz
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Tiến trình con
        collatz_sequence(n, buffer);
        return 0;  // Kết thúc tiến trình con sau khi hoàn thành tính toán
    } else {
        // Tiến trình cha
        wait(NULL);  // Chờ tiến trình con kết thúc

        // In kết quả từ buffer
        printf("Chuoi so la: ");
        for (int i = 0; buffer[i] != -1; i++) {
            printf("%d ", buffer[i]);
        }
        printf("\n");

        // Giải phóng bộ nhớ chia sẻ
        munmap(buffer, BUFFER_SIZE * sizeof(int));
    }

    return 0;
}
