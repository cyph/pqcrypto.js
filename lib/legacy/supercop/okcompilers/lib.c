int not3(int n)
{
  return n != 3;
}

int bytes(int n)
{
  return (n + 7) / 8;
}

long long shr32(long long n)
{
  return n >> 32;
}

double double5(void)
{
  return 5.0;
}

int intbytes(void)
{
  return sizeof(int);
}

int longbytes(void)
{
  return sizeof(long);
}

int rand1(int *in)
{
  static int out[8];
  int t[12]; unsigned int x;
  int r; int i; int loop;

  for (i = 0;i < 12;++i) t[i] = in[i];
  for (i = 0;i < 8;++i) out[i] = in[i];
  x = t[11];
  for (loop = 0;loop < 50;++loop) {
    for (r = 0;r < 16;++r)
      for (i = 0;i < 12;++i) {
        x ^= t[i];
        x = (x<<3)|(x>>29);
        x += in[i];
        x = (x<<2)|(x>>30);
        t[i] += x;
        x += i;
      }
    for (i = 0;i < 8;++i) out[i] ^= t[i + 4];
  }
  return out[0];
}
