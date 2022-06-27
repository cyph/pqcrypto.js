/*
The default crypto_core tests use random inputs.
These extra tests try a wider range of input weights.
*/

{
  long long offset,weight,i,direction;

  for (weight = 0;weight <= nlen;++weight) {
    for (direction = 0;direction < 2;++direction) {
      output_prepare(h2,h,hlen);
      input_prepare(n2,n,nlen);
      input_prepare(k2,k,klen);
      input_prepare(c2,c,clen);
  
      for (i = 0;i < nlen;++i) {
        n[i] &= ~1;
        if (direction) {
          if (nlen-1-i < weight) n[i] += 1;
        } else {
          if (i < weight) n[i] += 1;
        }
        n2[i] = n[i];
      }
  
      if (crypto_core(h,n,k,c) != 0) fail("crypto_core returns nonzero");
      checksum(h,hlen);
      output_compare(h2,h,hlen,"crypto_core");
      input_compare(n2,n,nlen,"crypto_core");
      input_compare(k2,k,klen,"crypto_core");
      input_compare(c2,c,clen,"crypto_core");
      
      double_canary(h2,h,hlen);
      double_canary(n2,n,nlen);
      double_canary(k2,k,klen);
      double_canary(c2,c,clen);
      if (crypto_core(h2,n2,k2,c2) != 0) fail("crypto_core returns nonzero");
      if (memcmp(h2,h,hlen) != 0) fail("crypto_core is nondeterministic");
    }
  }
}
