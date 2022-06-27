use crypto::digest::Digest;
use crypto::sha2::Sha256;

pub fn crypto_hash(out:&mut[u8;32],m:&[u8]) {
  let mut x = Sha256::new();
  x.input(m);
  x.result(out)
}
