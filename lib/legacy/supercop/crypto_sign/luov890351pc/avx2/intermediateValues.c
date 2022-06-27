#include "intermediateValues.h"

void printVinegarValues(FELT *vinegar){
	#ifdef KAT
		int i;
		printf("vinegar values = ");
		for(i=0 ; i<VINEGAR_VARS ; i++){
			printf("%02X", vinegar[i]);
		}
		printf("\n");
	#endif
}


void printAugmentedMatrix(Matrix A){
	#ifdef KAT
		int i,j,k;
		printf("Augmented matrix = ");
		for(i=0 ; i<OIL_VARS ; i++){
			for(j=0 ; j<=OIL_VARS ; j++){
				printf("%02X", A.array[i][j]);
			}
			printf("\n");
		}
	#endif
}

void reportSolutionFound(int solution_found){
	#ifdef KAT
		if(solution_found == 0){
			printf("solution not found, retry\n");
		}
		else{
			printf("solution found\n");
		}
	#endif
}

void printPrivateSolution(FELT *sig){
	#ifdef KAT
		int i,j;
		printf("private solution = ");
		for(i=0 ; i<VINEGAR_VARS + OIL_VARS  ; i++){
			printf("%02X", sig[i+1]);
		}
		printf("\n");
	#endif
}

void printEvaluation(FELT *eval){
	#ifdef KAT
		int i,j;
		printf("evaluation = ");
		for(i=0 ; i< OIL_VARS  ; i++){
			printf("%02X", eval[i]);
		}
		printf("\n");
	#endif
}
