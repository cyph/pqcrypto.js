#![allow(clippy::many_single_char_names)]

use crypto::symmetriccipher::SynchronousStreamCipher;
use crypto::salsa20::Salsa20;

pub fn crypto_stream_xor(c:&mut[u8],m:&[u8],n:&[u8;8],k:&[u8;32]) {
  let mut x = Salsa20::new(k,n);
  x.process(m,c)
}

pub fn crypto_stream(c:&mut[u8],n:&[u8;8],k:&[u8;32]) {
  let mut x = Salsa20::new(k,n);
  let m:Vec<u8> = vec![0;c.len()];
  x.process(&m,c)
}
