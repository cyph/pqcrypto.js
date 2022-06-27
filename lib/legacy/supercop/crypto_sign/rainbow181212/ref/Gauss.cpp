#include "rainbow.h"
using namespace std;

void display(GFpow *a, int const n, int const m, char * ch ) {   
	// display in a in matrix form:  n rows , m columns
	int i,j ;
	cout<<endl<<ch<<endl;
	for (i =0 ; i < n ; i++ ) {
		cout << endl ;
		for (j=0 ; j < m ; j++ ) if( a[i*m+j] == 0 ) cout<<". ";else cout << a[i*m+j]<<' ';
	}
	cout << endl ;
}



int gauss( GFpow ab[],  int const nequ, int const nvar  )  {
	// nequ = number of equations,
	// nvar = number of variables
	// dimension: ab[nequ*nvar] ab[neq][nvar]
	int i, j, k, i0, i00, j0, step=0;
	int * ind  ;

	int *ij = new int[nequ] ;


	for (i=0; i < nequ ; i++) ij[i]=i ;


	GFpow temp ;


#if DEBUG > 2
	int b0,b1 ;

	cout << "\nstarting gauss"<<endl ;

	for ( i = 0 ; i < nequ ; i++ ) {
		b0 = 0 ;
		for ( j = 0 ; j < nvar; j++) {
			if (ab[i*nvar+j] != 0 ) {
				if (b0==0) 	b0=1 ;
				else cout <<'+' ;
				cout << ab[i*nvar+j]<<"*u["<<j<<"]" ;
			}
		}
		if (b0==0) cout <<"0,"<<endl ;
		else cout <<','<<endl ;
	}
#endif


    ind = new int[nvar] ; 


	i0 = j0 = 0 ;
	while ( (i0< nequ ) && ( j0 < nvar )) {
		step++;
#if DEBUG > 2
		cout << endl << "step " << step << ": " << endl;
		display(ab, nequ, nvar+1, " ");
#endif
		i = i0 ;
		while ( ( ab[i*nvar+j0] ==  0 ) && ( i < nequ ) ) i++ ;

		if ( i < nequ ) {
			if ( i > i0 ) {  // interchange row i and i0
#if DEBUG > 2
				cout << endl << "interchange " << step << ": " << endl;
				display(ab, nequ, nvar+1, " ");
#endif
				for ( j = j0 ; j < nvar ; j++ ) {
					temp = ab[i*nvar+j] ;
					ab[i*nvar+j] = ab[i0*nvar+j] ;
					ab[i0*nvar+j] = temp ;
				}

			}
		
			
			//Inverse
			temp = 1/ab[i0*nvar+j0] ;
			
			ab[i0*nvar+j0] = 1 ;
			ind[i0] = j0 ;

#if DEBUG > 2
			cout << endl << "Substep1 " << step << ": " << endl;
			display(ab, nequ, nvar+1, " ");
#endif
			for ( k = j0+1 ; k < nvar; k++ ) ab[i0*nvar+k] = ab[i0*nvar+k]*temp ; 
#if DEBUG > 2
			cout << endl << "Substep2 " << step << ": " << endl;
			display(ab, nequ, nvar+1, " ");
#endif

			for ( j = i+1; j < nequ; j++ ) {
				if( (temp = ab[j*nvar+j0]) != 0 ){ 
					ab[j*nvar+j0] = 0 ;
					for ( k = j0+1; k < nvar ; k++ )  
						ab[j*nvar+k] += ab[i0*nvar+k]*temp ;
				}
			}
			i0++ ;
		}
		j0++;

	}


	i00 = i0 ;

#if DEBUG > 2
	cout << endl << "Pre: " << endl;
	display(ab, nequ, nvar+1, " ");
#endif

	for ( i0--; i0>=0 ; i0-- ) {

		j0 = ind[i0] ;
		for ( i = i0-1 ; i >= 0 ; i-- ) {
			temp = ab[i*nvar+j0] ;
			ab[i*nvar+j0] = 0 ;
			for ( k = j0+1;  k < nvar ; k++ ) 
				ab[i*nvar+k] = ab[i*nvar+k]+ab[i0*nvar+k]*temp ;
		}
		
	}

#if DEBUG > 2
	cout << endl << "Post: " << endl;
	display(ab, nequ, nvar+1, " ");

	cout << "\nend of gauss: " << i00 << endl;
	for ( i = 0 ; i < i00 ; i++ ) {
		b0 = b1 = 0 ;
		for ( j = 0 ; j < nvar; j++) {
			if (ab[i*nvar+j] != 0 ) {
				if (b0==0) {
					b0=1 ;
					if ( ab[i*nvar+j]!= 1) cout << ab[i*nvar+j]<<'*'; 
					cout <<"u["<<j<<"]->" ;
				}
				else {
					if ( b1==1 ) cout << '+' ;
					cout <<ab[i*nvar+j]<< "*u["<<j<<"]" ;
					b1=1 ;
				}
			}
		}
		if (b1==0) cout <<"0,"<<endl ;
		else cout <<','<<endl ;
	}
#endif
	return ( i00 ) ;

}