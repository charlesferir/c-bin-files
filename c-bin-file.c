#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define OUTPUT "matrix.dat"
#define MATRIX_SIZE 10

struct matrix {
    uint32_t size;
    uint32_t* content;
};

int write_matrix(struct matrix* m) {
    int matrix_fd = open(OUTPUT, O_WRONLY | O_CREAT, 0644);
    if(matrix_fd < 0)
        return errno;

    ssize_t ret = write(matrix_fd, &m->size, sizeof(uint32_t));
    if(ret < 0)
        return errno;

    ret = write(matrix_fd, m->content, m->size * m->size * sizeof(uint32_t));
    close(matrix_fd);
    return 0;
}

int read_matrix(struct matrix* m) {
    int matrix_fd = open(OUTPUT, O_RDONLY);
    if(matrix_fd < 0)
        return errno;

    ssize_t ret = read(matrix_fd, &m->size, sizeof(uint32_t));
    if(ret < 0)
        return errno;

    m->content = (uint32_t*) malloc(m->size * m->size * sizeof(uint32_t));
    if(!m->content)
        return errno;

    ret = read(matrix_fd, m->content, m->size * m->size * sizeof(uint32_t));
    close(matrix_fd);
    return 0;
}

void fill_matrix(struct matrix* m) {
    for(int i = 0; i < m->size; i++) {
        for(int j = 0; j < m->size; j++)
            m->content[i * m->size + j] = i * m->size + j;
    }
}

void print_matrix(struct matrix* m) {
    for(int i = 0; i < m->size; i++) {
        for(int j = 0; j < m->size; j++)
            printf("%3u ", m->content[i * m->size + j]);
        printf("\n");
    }
}

int main() {
    struct matrix written_m, read_m;
    written_m.size = MATRIX_SIZE;
    written_m.content = (uint32_t*) malloc(written_m.size * written_m.size * sizeof(uint32_t));
    if(!written_m.content) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return errno;
    }
    
    fill_matrix(&written_m);
    printf("WRITTEN MATRIX:\n");
    print_matrix(&written_m);

    int ret = write_matrix(&written_m);
    if(ret) {
        fprintf(stderr, "error: %s\n", strerror(ret));
        return ret;
    }

    ret = read_matrix(&read_m);
    if(ret) {
        fprintf(stderr, "error: %s\n", strerror(ret));
        return ret;
    }

    printf("READ MATRIX:\n");
    print_matrix(&written_m);

    return 0;
}
