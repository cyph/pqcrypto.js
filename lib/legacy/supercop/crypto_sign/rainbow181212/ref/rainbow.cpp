#include "sizes.h"
#include "Compose.h"

using namespace std;

int main()
{
	unsigned char sk[SECRETKEY_BYTES]; unsigned long sklen;
	unsigned char pk[PUBLICKEY_BYTES]; unsigned long pklen;
	unsigned char m[SHORTMESSAGE_BYTES]; unsigned long mlen = nv1 ;
	unsigned char sm[SIGNATURE_BYTES] ; unsigned long smlen = ovn ;
	
	int i, eof ;
	i=keypair( sk, & sklen, pk, & pklen ) ; //TODO: keypair function should take random inputs or inputs from a fil

	srand(100);
#if DEBUG > 0
	cout << "\nmessage: ";
#endif
	for ( i = 0 ; i < nv1; i++ ) {
		m[i] = rand() % deg_pow; // pow = 256;
#if DEBUG > 0
		cout << GFpow(m[i])<<" ";
#endif
	}
#if DEBUG > 0	
	cout << endl ;
#endif
	eof = signedshortmessage( sm, & smlen, m, mlen, sk, sklen);
	
	if (eof < 0)
	{
		cout << "Could not sign message" << endl;
		return -1;
	}
	else
#if DEBUG > 0
	cout << "\nSignature: " ;
	for (i = 0; i < ovn; i++) cout << GFpow(sm[i]) << " "; // signed message
#endif
	eof = shortmessagesigned( m,& mlen, sm, smlen, pk, pklen);

#if DEBUG > 0
	cout << endl << eof << endl;
#endif

	return eof;
}
