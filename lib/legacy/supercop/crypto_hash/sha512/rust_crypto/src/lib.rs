use crypto::digest::Digest;
use crypto::sha2::Sha512;

pub fn crypto_hash(out:&mut[u8;64],m:&[u8]) {
  let mut hasher = Sha512::new();
  hasher.input(m);
  hasher.result(out)
}
