#![allow(clippy::many_single_char_names)]

use crypto::aead::AeadEncryptor;
use crypto::aead::AeadDecryptor;
use crypto::aes_gcm::AesGcm;
use crypto::aes::KeySize::KeySize128;

pub fn crypto_aead_encrypt(
  c:&mut[u8],
  m:&[u8],
  ad:&[u8],
  _nsec:&[u8;0],
  npub:&[u8;12],
  k:&[u8;16])
-> usize
{
  let mut x = AesGcm::new(KeySize128,k,npub,ad);
  let clen = c.len();
  let (c,tag) = c.split_at_mut(m.len());
  x.encrypt(m,c,tag);
  clen
}

pub fn crypto_aead_decrypt(
  m:&mut[u8],
  _nsec:&mut[u8;0],
  c:&[u8],
  ad:&[u8],
  npub:&[u8;12],
  k:&[u8;16])
-> (usize,isize)
{
  let clen = c.len();
  if clen < 16 { return (0,-1) }
  let mlen = clen-16;
  let mut x = AesGcm::new(KeySize128,k,npub,ad);
  let (m,_) = m.split_at_mut(mlen);
  let (c,tag) = c.split_at(mlen);
  if !x.decrypt(c,m,tag) { return (0,-1) }
  (mlen,0)
}
