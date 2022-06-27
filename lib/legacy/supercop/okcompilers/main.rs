use std::mem;
use std::os::raw::{c_int,c_long,c_longlong,c_double};
use std::convert::TryInto;

extern "C" {
  fn not3(n:c_int) -> c_int;
  fn bytes(n:c_int) -> c_int;
  fn shr32(n:c_longlong) -> c_longlong;
  fn double5() -> c_double;
  fn intbytes() -> c_int;
  fn longbytes() -> c_int;
  fn rand1(c_arg: *const c_int) -> c_int;
}

fn main() {
  unsafe {
    if intbytes() != mem::size_of::<c_int>().try_into().unwrap() { std::process::exit(100) }
    if longbytes() != mem::size_of::<c_long>().try_into().unwrap() { std::process::exit(100) }
    if not3(3) != 0 { std::process::exit(100) }
    if not3(shr32(1) as c_int) != 1 { std::process::exit(100) }
    if bytes(not3(1)) != 1 { std::process::exit(100) }
    if double5() < 0.0 { std::process::exit(100) }
    
    let x:[c_int;12] = [3,1,4,1,5,9,2,6,5,3,5,8];
    if rand1(&x as *const c_int) != -131401890 { std::process::exit(100) }
  }
}
