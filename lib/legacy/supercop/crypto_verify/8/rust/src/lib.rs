pub fn crypto_verify(x:&[u8;8],y:&[u8;8]) -> isize {
  let mut differentbits:u8 = 0;
  for i in 0..8 { differentbits |= x[i]^y[i] }
  let result = differentbits as u32;
  let result = result.wrapping_sub(1);
  let result = 1&(result>>8);
  (result as isize)-1
}
