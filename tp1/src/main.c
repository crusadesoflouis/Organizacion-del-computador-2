#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "obdd.h"

char *formulasFile  =  "formulas.txt";

void formulas() {


	printf( "OBDD_X1_OR_X2\n");
	obdd_mgr* new_mgr	= obdd_mgr_create();
	obdd* x1_obdd		= obdd_mgr_var(new_mgr, "x1");
	obdd* x2_obdd		= obdd_mgr_var(new_mgr, "x2");
	obdd* x1_or_x2_obdd	= obdd_apply_or(x1_obdd, x2_obdd);
	obdd_print(x1_or_x2_obdd);
	printf( "\n");
	printf( "OBDD_X1_AND_X2\n");
	obdd* x1_and_x2_obdd	= obdd_apply_and(x1_obdd, x2_obdd);
	obdd_print(x1_and_x2_obdd);
	printf( "\n");
	printf( "OBDD_X1_AND_NOT_X1\n");
	obdd* not_x1_obdd = obdd_apply_not(x1_obdd);
	obdd* x1_and_not_x1 = obdd_apply_and(x1_obdd,not_x1_obdd);
	obdd_print(x1_and_not_x1);
	printf( "\n");
	printf( "OBDD_NOT(_X1_AND_X2)_OR_X1\n");
	obdd* not_x1_and_x2_obdd = obdd_apply_not(x1_and_x2_obdd);
	obdd* not_x1_and_x2_or_x1_obdd = obdd_apply_or(not_x1_and_x2_obdd,x1_obdd);
	obdd_print(not_x1_and_x2_or_x1_obdd);



	obdd_destroy(x1_obdd);
	obdd_destroy(x2_obdd);
	obdd_destroy(x1_or_x2_obdd);
	obdd_destroy(x1_and_x2_obdd);
	obdd_destroy(not_x1_obdd);
	obdd_destroy(x1_and_not_x1);
	obdd_destroy(not_x1_and_x2_obdd);
	obdd_destroy(not_x1_and_x2_or_x1_obdd);
	obdd_mgr_destroy(new_mgr);

}


int main (void){
	int save_out = dup(1);
	remove(formulasFile);
	int pFile = open(formulasFile, O_RDWR|O_CREAT|O_APPEND, 0600);
	if (-1 == dup2(pFile, 1)) { perror("cannot redirect stdout"); return 255; }
	formulas();
	fflush(stdout);
	close( pFile );
	dup2(save_out, 1);
	return 0;
}
