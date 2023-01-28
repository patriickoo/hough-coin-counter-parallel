#include "hcc_lib.h"

int main() {
    MPI_Status status;
    int myrank, size;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    struct matrix mat;
    mat.cols = 0;
    mat.rows = 0;
    mat.faces = 1;
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

    }

    mat.faces = 1;
    MPI_Bcast(&mat.cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&mat.rows, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (myrank != 0) {
        printf("[%d] Received cols (%d), rows (%d)\n", myrank, mat.cols, mat.rows);
        mat.data = malloc(sizeof *mat.data * mat.cols * mat.rows);
    }

    MPI_Bcast(mat.data, mat.cols * mat.rows, MPI_INT, 0, MPI_COMM_WORLD);


    // START HOUGH TANSFORM
    accumulator.rows = mat.rows;
    accumulator.cols = mat.cols;
    accumulator.faces = sqrt(pow(mat.rows, 2) + pow(mat.cols, 2)) / 6;
    long long unsigned int data_length = accumulator.rows * accumulator.cols * accumulator.faces;
    printf("[%d] data length: %llu\n", myrank, data_length);
    accumulator.data = malloc(sizeof *accumulator.data * data_length);
    printf("[%d] accumulator: x(%d) y(%d) f(%d)\n", myrank, accumulator.cols, accumulator.rows, accumulator.faces);

    increment_accumulator(&mat, &accumulator, size, myrank);

    if (myrank == 0) {
        printf("Accumulator incremented.\n");

        // FIND MAXIMUM
        int peak = find_maximum(&accumulator);

        printf("Peak is %d\nWriting circles...\n", peak);

        // SAVE CIRCLES COORDINATES AND PRINT ON FILE
        struct centers_coords *coords;
        coords = (struct centers_coords *) malloc(sizeof(int) * 3 * 1000);
        int size2 = write_circles(coords, &accumulator, (int) ((float) peak * 80 / 100));
        printf("Circles written.\n");

        // COUNT COINS
        printf("Subtotal is %f\n", count_coins(coords, size2));

        //fclose(print_mat);
        free_matrix(&mat);
    }

    MPI_Finalize();

    return 0;
}
