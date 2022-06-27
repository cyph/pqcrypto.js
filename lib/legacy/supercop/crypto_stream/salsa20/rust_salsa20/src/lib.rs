#![allow(clippy::many_single_char_names)]

use salsa20::{Salsa20,Key,Nonce};
use salsa20::cipher::{NewStreamCipher,SyncStreamCipher};

pub fn crypto_stream_xor(c:&mut[u8],m:&[u8],n:&[u8;8],k:&[u8;32]) {
  let k = Key::from_slice(k);
  let n = Nonce::from_slice(n);
  let mut x = Salsa20::new(&k,&n);
  c.copy_from_slice(m);
  x.apply_keystream(c)
}

pub fn crypto_stream(c:&mut[u8],n:&[u8;8],k:&[u8;32]) {
  let k = Key::from_slice(k);
  let n = Nonce::from_slice(n);
  let mut x = Salsa20::new(&k,&n);
  for ci in c.iter_mut() { *ci = 0 }
  x.apply_keystream(c)
}
