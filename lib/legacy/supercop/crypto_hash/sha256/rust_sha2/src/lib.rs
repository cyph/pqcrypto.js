use sha2::{Sha256,Digest};

pub fn crypto_hash(out:&mut[u8;32],m:&[u8]) {
  let h = Sha256::digest(m);
  out.copy_from_slice(&h)
}
