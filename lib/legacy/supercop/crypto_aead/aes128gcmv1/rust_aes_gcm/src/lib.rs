#![allow(clippy::many_single_char_names)]

use aes_gcm::Aes128Gcm;
use aes_gcm::aead::{Aead,NewAead,Payload,generic_array::GenericArray};

pub fn crypto_aead_encrypt(
  c:&mut[u8],
  m:&[u8],
  ad:&[u8],
  _nsec:&[u8;0],
  npub:&[u8;12],
  k:&[u8;16])
-> usize
{
  let k = GenericArray::from_slice(k);
  let npub = GenericArray::from_slice(npub);
  let x = Aes128Gcm::new(k);
  let payload = Payload{msg:m,aad:ad};
  let t = x.encrypt(npub,payload).unwrap();
  c.copy_from_slice(&t);
  c.len()
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
  let k = GenericArray::from_slice(k);
  let npub = GenericArray::from_slice(npub);
  let x = Aes128Gcm::new(k);
  let (m,_) = m.split_at_mut(mlen);
  let payload = Payload{msg:c,aad:ad};
  match x.decrypt(npub,payload) {
    Err(_e) => (0,-1),
    Ok(t) => { m.copy_from_slice(&t); (mlen,0) }
  }
}
