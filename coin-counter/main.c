#include "hcc_lib.h"

int main() {
    clock_t start = clock();  // start counting time

    MPI_Status status;
    int myrank, size;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    struct matrix mat;
    struct matrix accumulator;
    init();  // initialise precomputed sine and cosine arrays

    if (myrank == 0) {  // master process initialising
        FILE *f;
        f = fopen("files/matrix.txt", "r");
        if (f != NULL) {
            printf("File found: ");
        }

        read_image(f, &mat);  // read image and convert it into a struct matrix
        fclose(f);
        printf("%dx%d\nIncrementing accumulator...\n", mat.cols, mat.rows);

        /* HOUGH TANSFORM */
        accumulator.rows = mat.rows;
        accumulator.cols = mat.cols;
        accumulator.faces = sqrt(pow(mat.rows, 2) + pow(mat.cols, 2)) / 6;
        long long unsigned int data_length = accumulator.rows * accumulator.cols * accumulator.faces;
        printf("data length: %llu\n", data_length);
        accumulator.data = malloc(sizeof *accumulator.data * data_length);
        printf("accumulator: x(%d) y(%d) f(%d) sizeofdata(%lu)\n", accumulator.cols, accumulator.rows,
               accumulator.faces, sizeof(accumulator.data));
    }

    MPI_Bcast(&mat.rows, 1, MPI_INT, 0, MPI_COMM_WORLD);  // broadcast rows and cols
    MPI_Bcast(&mat.cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (myrank != 0) {  // all processes except master allocate space for edge detected mat and accumulator
        mat.data = (int *) malloc(sizeof(int) * mat.rows * mat.cols);
        accumulator.rows = mat.rows;
        accumulator.cols = mat.cols;
        accumulator.faces = sqrt(pow(mat.rows, 2) + pow(mat.cols, 2)) / 6;
    }

    MPI_Bcast(mat.data, mat.rows * mat.cols, MPI_INT, 0, MPI_COMM_WORLD);  // broadcast edge detected matrix

    MPI_Barrier(MPI_COMM_WORLD);

    increment_accumulator(&mat, &accumulator, size, myrank);  // start parallel accumulator incrementing

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
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (myrank == 0) {
    
        clock_t end = clock();
        double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;

        printf("TIME SPENT %lf\n", time_spent);

    }

    MPI_Finalize();

    return 0;
}
