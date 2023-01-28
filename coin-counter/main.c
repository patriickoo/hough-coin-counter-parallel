#include "hcc_lib.h"

int main() {
    clock_t start = clock();

    MPI_Status status;
    int myrank, size;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    struct matrix mat;
    struct matrix accumulator;
    init();

    if (myrank == 0) {
        // READ IMAGE
        FILE *f;
        f = fopen("files/matrix.txt", "r");
        if (f != NULL) {
            printf("File found: ");
        }

        read_image(f, &mat);
        fclose(f);
        printf("%dx%d\nIncrementing accumulator...\n", mat.cols, mat.rows);

        // START HOUGH TANSFORM
        accumulator.rows = mat.rows;
        accumulator.cols = mat.cols;
        accumulator.faces = sqrt(pow(mat.rows, 2) + pow(mat.cols, 2)) / 6;
        long long unsigned int data_length = accumulator.rows * accumulator.cols * accumulator.faces;
        printf("data length: %llu\n", data_length);
        accumulator.data = malloc(sizeof *accumulator.data * data_length);
        printf("accumulator: x(%d) y(%d) f(%d) sizeofdata(%lu)\n", accumulator.cols, accumulator.rows,
               accumulator.faces, sizeof(accumulator.data));
    }

    printf("Broadcasting... %d\n", myrank);
    MPI_Bcast(&mat.rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&mat.cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf(" received myrank %d size %d %d\n", myrank, mat.rows, mat.cols);

    if (myrank != 0) {
        mat.data = (int *) malloc(sizeof(int) * mat.rows * mat.cols);
        accumulator.rows = mat.rows;
        accumulator.cols = mat.cols;
        accumulator.faces = sqrt(pow(mat.rows, 2) + pow(mat.cols, 2)) / 6;
    }

    MPI_Bcast(mat.data, mat.rows * mat.cols, MPI_INT, 0, MPI_COMM_WORLD);
    printf("[%d] Received.\n", myrank);

    MPI_Barrier(MPI_COMM_WORLD);

    printf("[%d] Barrier surpassed!\n", myrank);

    increment_accumulator(&mat, &accumulator, size, myrank);

    printf("[%d] Accumulator incremented.\n", myrank);

    if (myrank == 0) {
        // FIND MAXIMUM
        int peak = find_maximum(&accumulator);

        printf("Peak is %d\nWriting circles...\n", peak);

        // SAVE CIRCLES COORDINATES AND PRINT ON FILE
        struct centers_coords *coords;
        coords = (struct centers_coords *) malloc(sizeof(int) * 3 * 1000);
        int size2 = write_circles(coords, &accumulator, (int) ((float) peak * THRESHOLD_PERCENTAGE / 100));
        printf("Circles written.\n");

        // COUNT COINS
        printf("Subtotal is %f\n", count_coins(coords, size2));

        free_matrix(&mat);
    }

    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("TIME SPENT %lf\n", time_spent);

    MPI_Finalize();

    return 0;
}
