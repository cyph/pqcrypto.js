use std::os::raw::{c_int,c_long,c_longlong,c_double};
use std::mem;
use std::slice;
use std::convert::TryInto;

fn not3(n:c_int) -> c_int {
  if n != 3 { 1 } else { 0 }
}

fn bytes(n:c_int) -> c_int {
  (n + 7) / 8
}

fn shr32(n:c_longlong) -> c_longlong {
  n >> 32
}

fn double5() -> c_double {
  5.0
}

fn intbytes() -> c_int {
  mem::size_of::<c_int>().try_into().unwrap()
}

fn longbytes() -> c_int {
  mem::size_of::<c_long>().try_into().unwrap()
}

fn rand1(c_arg: *const c_int) -> c_int {
  let input: &[c_int] = unsafe { slice::from_raw_parts(c_arg,12) };
  let mut out:[c_int;8] = [0;8];
  let mut t:[c_int;12] = [0;12];

  for i in 0..12 { t[i] = input[i] }
  for i in 0..8 { out[i] = input[i] }
  let mut x = t[11];
  for _loop in 0..50 {
    for _r in 0..16 {
      for i in 0..12 {
        x ^= t[i];
        x = x.rotate_left(3);
        x = x.wrapping_add(input[i]);
        x = x.rotate_left(2);
        t[i] = t[i].wrapping_add(x);
        x = x.wrapping_add(i as c_int);
      }
    }
    for i in 0..8 {
      out[i] ^= t[i+4]
    }
  }
  out[0]
}

fn main() {
  if intbytes() != mem::size_of::<c_int>().try_into().unwrap() { std::process::exit(100) }
  if longbytes() != mem::size_of::<c_long>().try_into().unwrap() { std::process::exit(100) }
  if not3(3) != 0 { std::process::exit(100) }
  if not3(shr32(1) as c_int) != 1 { std::process::exit(100) }
  if bytes(not3(1)) != 1 { std::process::exit(100) }
  if double5() < 0.0 { std::process::exit(100) }
  
  let x:[c_int;12] = [3,1,4,1,5,9,2,6,5,3,5,8];
  if rand1(&x as *const c_int) != -131401890 { std::process::exit(100) }
}
