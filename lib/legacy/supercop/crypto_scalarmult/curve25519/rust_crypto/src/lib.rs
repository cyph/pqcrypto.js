use crypto::curve25519;

pub fn crypto_scalarmult(q:&mut[u8;32],n:&[u8;32],p:&[u8;32]) {
  q.copy_from_slice(&curve25519::curve25519(n,p))
}

pub fn crypto_scalarmult_base(q:&mut[u8;32],n:&[u8;32]) {
  q.copy_from_slice(&curve25519::curve25519_base(n))
}
