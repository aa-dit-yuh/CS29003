#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/*-----------------Global declarations-----------------*/
static unsigned long flop_counter;
static unsigned long flop_counter_normal;
static unsigned long flop_counter_strassen;
static unsigned long flop_counter_hybrid[10];

struct Matrix;
union SubMatrix;

/*------------------The matrix struct------------------*/
struct Matrix{
    
    /* Stores the 2-exponent of the dimension of the matrix
     * The dimension of matrix = 2^n
     */
    int n;

    /* Pointer to the four SubMatrices
     * _                      _
     * | submat[0]  submat[1] |
     * | submat[2]  submat[3] | 
     * -                      -
   */
    union SubMatrix *submat[4];

};

/*----------------The sub-matrix struct----------------*/
union SubMatrix{

  /* Stores a floating point number if submatrix is 1*1 matrix
   * Else, stores the pointer to another Matrix struct
   */
  double element;
  struct Matrix *matrixObject;
};

/*----------------------Functions----------------------*/

/* Function name: initializeMatrixZero
 * Input parameters: 
 *        1. struct Matrix *matrixObject: pointer to a struct Matrix, 
 *        2. int n: 2-exponent of the dimension of the initialized Matrix, dimension = 2^n,
 *        3. int recursive: flag that signifies if all submatrices must be initialized recursive,
 * Purpose: Dynamically allocates a matrix of dimension 2^n. The matrix elements of a 1*1 matrix are zero initialized
 */
void initializeMatrixZero(struct Matrix *matrixObject, int n, int recursive)
{
  matrixObject->n = n;

  if(n==1){
    int i;
    for(i=0;i<4;i++){
      matrixObject->submat[i] = (union SubMatrix *) malloc(sizeof(union SubMatrix));
      matrixObject->submat[i]->element = 0;
    }
  }
  else{
    int i;
    for(i=0;i<4;i++){
      matrixObject->submat[i] = (union SubMatrix *) malloc(sizeof(union SubMatrix));
      matrixObject->submat[i]->matrixObject = (struct Matrix *) malloc(sizeof(struct Matrix));
      if(recursive){
        initializeMatrixZero(matrixObject->submat[i]->matrixObject, n-1, 1);
      }
    }
  }
}

/* Function name: initializeMatrixRandom
 * Input parameters: 
 *        1. struct Matrix *matrixObject: pointer to a struct Matrix, 
 *        2. int n: 2-exponent of the dimension of the initialized Matrix, dimension = 2^n,
 *        3. int recursive: flag that signifies if all submatrices must be initialized recursive.
 * Return: (none)
 * Purpose: Recursively dynamically allocates a matrix of dimension 2^n. The matrix elements of 1*1 matrix are randomly filled using drand48()
 */
void initializeMatrixRandom(struct Matrix *matrixObject, int n, int recursive)
{
  matrixObject->n = n;

  if(n==1){
    int i;
    for(i=0;i<4;i++){
      matrixObject->submat[i] = (union SubMatrix *) malloc(sizeof(union SubMatrix));
      matrixObject->submat[i]->element = drand48();
    }
  }
  else{
    int i;
    for(i=0;i<4;i++){
      matrixObject->submat[i] = (union SubMatrix *) malloc(sizeof(union SubMatrix));
      matrixObject->submat[i]->matrixObject = (struct Matrix *) malloc(sizeof(struct Matrix));
      if(recursive){
        initializeMatrixRandom(matrixObject->submat[i]->matrixObject, n-1, 1);
      }
    }
  }
}

/* Function name: deallocateMatrix
 * Input parameters: 
 *        1. struct Matrix *matrixObject: pointer to a struct Matrix, 
 * Return: (none)
 * Purpose: Deallocated the matrix recursively
 */
void deallocateMatrix(struct Matrix *matrixObject)
{
  if(matrixObject->n==1){
    int i;
    for(i=0;i<4;i++){
      free(matrixObject->submat[i]);
    }
  }
  else{
    int i;
    for(i=0;i<4;i++){
      deallocateMatrix(matrixObject->submat[i]->matrixObject);
      free(matrixObject->submat[i]);
    }
  }
}

/* Function name: getMatrixElement
 * Input parameters: 
 *        1. struct Matrix *matrixObject: pointer to a struct Matrix, 
 *        2. int row: row index,
 *        3. int column: column index,
 * Return: pointer to a variable of type double
 * Purpose: Returns the pointer to the element at particular row and column
 */
double* getMatrixElement(struct Matrix *matrixObject, int row, int column)
{
  if(matrixObject->n == 1){
    if(row >= 2 || column >= 2){
      return NULL;
    }
    else{
      return &matrixObject->submat[(row<<1)|column]->element;
    }
  }
  else if(row < pow(2,matrixObject->n-1)){
    if(column < pow(2,matrixObject->n-1)){
      return getMatrixElement(matrixObject->submat[0]->matrixObject, row, column);
    }
    else{
      return getMatrixElement(matrixObject->submat[1]->matrixObject, row, column - pow(2,matrixObject->n-1));
    }
  }
  else{
    if(column < pow(2,matrixObject->n-1)){
      return getMatrixElement(matrixObject->submat[2]->matrixObject, row - pow(2,matrixObject->n-1), column);
    }
    else{
      return getMatrixElement(matrixObject->submat[3]->matrixObject, row - pow(2,matrixObject->n-1), column - pow(2,matrixObject->n-1));
    }
  }
}

/* Function name: printMatrix
 * Input parameters: 
 *        1. struct Matrix *matrixObject: pointer to a struct Matrix, 
 * Return: void
 * Purpose: Prints all elements of a matrix
 */
void printMatrix(struct Matrix *matrixObject)
{
  int i,j;
  for(i=0;i<pow(2,matrixObject->n);i++){
    printf("|");
    for (j=0; j<pow(2,matrixObject->n); j++){
      printf(" %f ", *getMatrixElement(matrixObject, i, j));
      fflush(stdout);
    }
    printf("|\n");
  }
  printf("\n");
}

/* Function name: normalMatrixMultiply
 * Input parameters: 
 *        1. struct Matrix *matrixInput1: pointer to a struct Matrix, 
 *        2. struct Matrix *matrixInput2: pointer to a struct Matrix, 
 * Return: pointer to a struct Matrix
 * Purpose: Multiplies two Matrices using normal method [0(n^3)] and returns the pointer to the resultant struct Matrix
 */
struct Matrix* normalMatrixMultiply(struct Matrix *matrixInput1, struct Matrix *matrixInput2)
{
  int i,j,k;
  double sum;
  struct Matrix * matrixOutput;

  if(matrixInput1->n != matrixInput2->n)
        return NULL;
 
  matrixOutput = (struct Matrix *) malloc(sizeof(struct Matrix));
  initializeMatrixZero(matrixOutput, matrixInput1->n, 1);
    
  for(i=0; i<pow(2,matrixInput1->n); i++){
    for(j=0; j<pow(2,matrixInput1->n); j++){
      sum = ((*getMatrixElement(matrixInput1, i, 0))*(*getMatrixElement(matrixInput2, 0, j)));
      flop_counter++;

      for(k=1; k<pow(2,matrixInput1->n); k++){
        sum += ((*getMatrixElement(matrixInput1, i, k))*(*getMatrixElement(matrixInput2, k, j)));
        flop_counter += 2;
      }
      *getMatrixElement(matrixOutput, i, j) = sum;
    }
  }
  return matrixOutput;
}

/* Function name: normalMatrixAddSub
 * Input parameters: 
 *        1. struct Matrix *matrixInput1: pointer to a struct Matrix, 
 *        2. struct Matrix *matrixInput2: pointer to a struct Matrix, 
 *        3. double multiplier
 * Return: pointer to a struct Matrix
 * Purpose: Adds two Matrices [matrixInput1 + (multiplier)*matrixInput2] and returns the pointer to the resultant struct Matrix
 */
struct Matrix* normalMatrixAddSub(struct Matrix *matrixInput1, struct Matrix *matrixInput2, double multiplier)
{
  int i,j;
  struct Matrix * matrixOutput;

  if(matrixInput1->n != matrixInput2->n){
    return NULL;
  }

  matrixOutput = (struct Matrix *) malloc(sizeof(struct Matrix));
  initializeMatrixZero(matrixOutput, matrixInput1->n, 1);
    
  for(i=0; i<pow(2,matrixInput1->n); i++){
    for(j=0; j<pow(2,matrixInput1->n); j++){
      *getMatrixElement(matrixOutput, i, j) = ((*getMatrixElement(matrixInput1, i, j)) + (multiplier)*(*getMatrixElement(matrixInput2, i, j)));
      flop_counter++;
    }
  }
  return matrixOutput;
}

/* Function name: strassenMatrixMultiply
 * Input parameters: 
 *        1. struct Matrix *matrixInput1: pointer to a struct Matrix, 
 *        2. struct Matrix *matrixInput2: pointer to a struct Matrix, 
 * Return: pointer to a struct Matrix
 * Purpose: Multiplies two Matrices using strassen Algorithm [0(n^log7)] and returns the pointer to the resultant struct Matrix
 */
struct Matrix* strassenMatrixMultiply(struct Matrix *matrixInput1, struct Matrix *matrixInput2)
{
  int i;
  struct Matrix *strassen[7], *matrixOutput;

  if(matrixInput1->n != matrixInput2->n){
    return NULL;
  }

  matrixOutput = (struct Matrix *) malloc(sizeof(struct Matrix));
  initializeMatrixZero(matrixOutput, matrixInput1->n, 0);
  
  if(matrixInput1->n==1){
    double P[7];
    P[0] = (matrixInput1->submat[0]->element + matrixInput1->submat[3]->element)*(matrixInput2->submat[0]->element + matrixInput2->submat[3]->element);
    flop_counter += 3;
    P[1] = (matrixInput1->submat[2]->element + matrixInput1->submat[3]->element)*(matrixInput2->submat[0]->element);
    flop_counter += 2;
    P[2] = (matrixInput1->submat[0]->element)*(matrixInput2->submat[1]->element - matrixInput2->submat[3]->element);
    flop_counter += 2;
    P[3] = (matrixInput1->submat[3]->element)*(matrixInput2->submat[2]->element - matrixInput2->submat[0]->element);
    flop_counter += 2;
    P[4] = (matrixInput1->submat[0]->element + matrixInput1->submat[1]->element)*(matrixInput2->submat[3]->element);
    flop_counter +=2;
    P[5] = (matrixInput1->submat[2]->element - matrixInput1->submat[0]->element)*(matrixInput2->submat[0]->element + matrixInput2->submat[1]->element);
    flop_counter += 3;
    P[6] = (matrixInput1->submat[1]->element - matrixInput1->submat[3]->element)*(matrixInput2->submat[2]->element + matrixInput2->submat[3]->element);
    flop_counter += 3;

    matrixOutput->submat[0]->element = P[0] + P[3] - P[4] + P[6];
    matrixOutput->submat[1]->element = P[2] + P[4];
    matrixOutput->submat[2]->element = P[1] + P[3];
    matrixOutput->submat[3]->element = P[0] + P[2] - P[1] + P[5];
    flop_counter += 8;

    return matrixOutput;
  }

  for(i=0; i<7; i++){
    strassen[i] = (struct Matrix *) malloc(sizeof(struct Matrix));
    initializeMatrixZero(strassen[i], matrixInput1->n, 0);
  }

  strassen[0] = strassenMatrixMultiply(normalMatrixAddSub(matrixInput1->submat[0]->matrixObject, matrixInput1->submat[3]->matrixObject, 1), normalMatrixAddSub(matrixInput2->submat[0]->matrixObject, matrixInput2->submat[3]->matrixObject, 1));
  strassen[1] = strassenMatrixMultiply(normalMatrixAddSub(matrixInput1->submat[2]->matrixObject, matrixInput1->submat[3]->matrixObject, 1), matrixInput2->submat[0]->matrixObject);
  strassen[2] = strassenMatrixMultiply(matrixInput1->submat[0]->matrixObject, normalMatrixAddSub(matrixInput2->submat[1]->matrixObject, matrixInput2->submat[3]->matrixObject, -1));
  strassen[3] = strassenMatrixMultiply(matrixInput1->submat[3]->matrixObject, normalMatrixAddSub(matrixInput2->submat[2]->matrixObject, matrixInput2->submat[0]->matrixObject, -1));
  strassen[4] = strassenMatrixMultiply(normalMatrixAddSub(matrixInput1->submat[0]->matrixObject, matrixInput1->submat[1]->matrixObject, 1), matrixInput2->submat[3]->matrixObject);
  strassen[5] = strassenMatrixMultiply(normalMatrixAddSub(matrixInput1->submat[2]->matrixObject, matrixInput1->submat[0]->matrixObject, -1), normalMatrixAddSub(matrixInput2->submat[0]->matrixObject, matrixInput2->submat[1]->matrixObject, 1));
  strassen[6] = strassenMatrixMultiply(normalMatrixAddSub(matrixInput1->submat[1]->matrixObject, matrixInput1->submat[3]->matrixObject, -1), normalMatrixAddSub(matrixInput2->submat[2]->matrixObject, matrixInput2->submat[3]->matrixObject, 1));

  matrixOutput->submat[0]->matrixObject = normalMatrixAddSub(strassen[0], normalMatrixAddSub(strassen[3], normalMatrixAddSub(strassen[4], strassen[6], -1), -1), 1);
  matrixOutput->submat[1]->matrixObject = normalMatrixAddSub(strassen[2], strassen[4], 1);
  matrixOutput->submat[2]->matrixObject = normalMatrixAddSub(strassen[1], strassen[3], 1);
  matrixOutput->submat[3]->matrixObject = normalMatrixAddSub(strassen[0], normalMatrixAddSub(strassen[1], normalMatrixAddSub(strassen[2], strassen[5], 1), -1), -1);

  return matrixOutput;  
}

/* Function name: hybridMatrixMultiply
 * Input parameters: 
 *        1. struct Matrix *matrixInput1: pointer to a struct Matrix, 
 *        2. struct Matrix *matrixInput2: pointer to a struct Matrix,
 *        3. int base_case: dimension below which normal matrix multiplication occurs over Strassen multiplication 
 * Return: pointer to a struct Matrix
 * Purpose: Multiplies two Matrices using hybrid version of strassen Algorithm and returns the pointer to the resultant struct Matrix
 */
struct Matrix* hybridMatrixMultiply(struct Matrix *matrixInput1, struct Matrix *matrixInput2, int base_case)
{
  int i;
  struct Matrix *strassen[7], *matrixOutput;

  if(matrixInput1->n != matrixInput2->n){
    return NULL;
  }

  matrixOutput = (struct Matrix *) malloc(sizeof(struct Matrix));
  initializeMatrixZero(matrixOutput, matrixInput1->n, 0);
  
  if(matrixInput1->n <= base_case){
    return normalMatrixMultiply(matrixInput1, matrixInput2);
  }

  for(i=0; i<7; i++){
    strassen[i] = (struct Matrix *) malloc(sizeof(struct Matrix));
    initializeMatrixZero(strassen[i], matrixInput1->n, 0);
  }

  strassen[0] = hybridMatrixMultiply(normalMatrixAddSub(matrixInput1->submat[0]->matrixObject, matrixInput1->submat[3]->matrixObject, 1), normalMatrixAddSub(matrixInput2->submat[0]->matrixObject, matrixInput2->submat[3]->matrixObject, 1), base_case);
  strassen[1] = hybridMatrixMultiply(normalMatrixAddSub(matrixInput1->submat[2]->matrixObject, matrixInput1->submat[3]->matrixObject, 1), matrixInput2->submat[0]->matrixObject, base_case);
  strassen[2] = hybridMatrixMultiply(matrixInput1->submat[0]->matrixObject, normalMatrixAddSub(matrixInput2->submat[1]->matrixObject, matrixInput2->submat[3]->matrixObject, -1), base_case);
  strassen[3] = hybridMatrixMultiply(matrixInput1->submat[3]->matrixObject, normalMatrixAddSub(matrixInput2->submat[2]->matrixObject, matrixInput2->submat[0]->matrixObject, -1), base_case);
  strassen[4] = hybridMatrixMultiply(normalMatrixAddSub(matrixInput1->submat[0]->matrixObject, matrixInput1->submat[1]->matrixObject, 1), matrixInput2->submat[3]->matrixObject, base_case);
  strassen[5] = hybridMatrixMultiply(normalMatrixAddSub(matrixInput1->submat[2]->matrixObject, matrixInput1->submat[0]->matrixObject, -1), normalMatrixAddSub(matrixInput2->submat[0]->matrixObject, matrixInput2->submat[1]->matrixObject, 1), base_case);
  strassen[6] = hybridMatrixMultiply(normalMatrixAddSub(matrixInput1->submat[1]->matrixObject, matrixInput1->submat[3]->matrixObject, -1), normalMatrixAddSub(matrixInput2->submat[2]->matrixObject, matrixInput2->submat[3]->matrixObject, 1), base_case);

  matrixOutput->submat[0]->matrixObject = normalMatrixAddSub(strassen[0], normalMatrixAddSub(strassen[3], normalMatrixAddSub(strassen[4], strassen[6], -1), -1), 1);
  matrixOutput->submat[1]->matrixObject = normalMatrixAddSub(strassen[2], strassen[4], 1);
  matrixOutput->submat[2]->matrixObject = normalMatrixAddSub(strassen[1], strassen[3], 1);
  matrixOutput->submat[3]->matrixObject = normalMatrixAddSub(strassen[0], normalMatrixAddSub(strassen[1], normalMatrixAddSub(strassen[2], strassen[5], 1), -1), -1);

  return matrixOutput;  

}

/*--------------------Main function--------------------*/
int main()
{
  int dimension, base_case_size;
  struct Matrix *matrixObject1, *matrixObject2, *normalMatrixMultiplied, *strassenMatrixMultiplied, *hybridMatrixMultiplied;

  printf("Enter the 2-exponent of the dimension of the two matrices to be multiplied:\n"
        "For eg if the dimension is 64=2^6, enter 6\n");
  scanf("%d",&dimension);

  matrixObject1 = (struct Matrix *) malloc(sizeof(struct Matrix));
  matrixObject2 = (struct Matrix *) malloc(sizeof(struct Matrix));
  
  initializeMatrixRandom(matrixObject1, dimension, 1);
  initializeMatrixRandom(matrixObject2, dimension, 1);

  if(dimension<=3){
    printf("Printing the first matrix:\n");
    printMatrix(matrixObject1);

    printf("Printing the second matrix:\n");
    printMatrix(matrixObject2);
  }

  printf("\n\nMultiplying the two matrices with normal matrix multiplication\n");
  flop_counter = 0;
  normalMatrixMultiplied = normalMatrixMultiply(matrixObject1, matrixObject2);
  flop_counter_normal = flop_counter;
  printf("Flops: %lu\n", flop_counter_normal);

  if(dimension<=3){
    printf("Printing the multiplied matrix:\n");
    printMatrix(normalMatrixMultiplied);
  }

  printf("\n\nMultiplying the two matrices with Strassen algorithm only\n");
  flop_counter = 0;
  strassenMatrixMultiplied = strassenMatrixMultiply(matrixObject1, matrixObject2);
  flop_counter_strassen = flop_counter;
  printf("Flops: %lu\n", flop_counter_strassen);

  if(dimension<=3){
    printf("Printing the multiplied matrix:\n");
    printMatrix(strassenMatrixMultiplied);
  }

  for(base_case_size=1; base_case_size<=(6<dimension?6:dimension); base_case_size++){
    printf("\n\nMultiplying the two matrices with hybrid-Strassen algorithm base case=2^%d level\n",base_case_size);
    flop_counter = 0;
    hybridMatrixMultiplied = hybridMatrixMultiply(matrixObject1, matrixObject2, base_case_size);
    flop_counter_hybrid[base_case_size] = flop_counter;
    printf("Flops: %lu\n", flop_counter);

    if(flop_counter_hybrid[base_case_size] < flop_counter_normal){
      printf("Savings obtained over normal matrix multiplication = %lu\n", flop_counter_normal - flop_counter_hybrid[base_case_size]);
    }
    if(flop_counter_hybrid[base_case_size] < flop_counter_strassen){
      printf("Savings obtained over purely Strassen multiplication = %lu\n", flop_counter_strassen - flop_counter_hybrid[base_case_size]);
    }
    else{
      printf("No savings obtained in this case\n");
    }
    if(dimension<=3){
      printf("Printing the multiplied matrix:\n");
      printMatrix(hybridMatrixMultiplied);
    }
  }
  deallocateMatrix(matrixObject1);
  deallocateMatrix(matrixObject2);
  deallocateMatrix(normalMatrixMultiplied);
  deallocateMatrix(strassenMatrixMultiplied);
  deallocateMatrix(hybridMatrixMultiplied);
  return 0;
}
