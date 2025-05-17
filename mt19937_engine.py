# Engine forked from https://github.com/anneouyang

class MT19937:
  w, n = 32, 624
  f = 1812433253
  m, r = 397, 31
  a = 0x9908B0DF
  d, b, c = 0xFFFFFFFF, 0x9D2C5680, 0xEFC60000
  u, s, t, l = 11, 7, 15, 18

  def __init__(self, seed = 0x19937):
    self.state_vec = [0] * MT19937.n
    self.vec_index = 0
    self.initialize(seed)

  def initialize(self, seed):
    self.state_vec[0] = seed
    for i in range(1, MT19937.n):
      self.state_vec[i] = (MT19937.f * (self.state_vec[i - 1] ^ (self.state_vec[i - 1] >> (MT19937.w - 2))) + i) & ((1 << MT19937.w) - 1)
    self.twist()

  def twist(self):
    lower_mask = (1 << MT19937.r) - 1
    upper_mask =  (~lower_mask) & ((1 << MT19937.w) - 1)
    for i in range(MT19937.n):
      tmp = (self.state_vec[i] & upper_mask) + (self.state_vec[(i + 1) % MT19937.n] & lower_mask)
      tmpA = tmp >> 1
      if (tmp % 2):
        tmpA = tmpA ^ MT19937.a
      self.state_vec[i] = self.state_vec[(i + MT19937.m) % MT19937.n] ^ tmpA
    self.vec_index = 0

  def temper(self):
    if self.vec_index == MT19937.n:
      self.twist()
    y = self.state_vec[self.vec_index]
    y = y ^ ((y >> MT19937.u) & MT19937.d)
    y = y ^ ((y << MT19937.s) & MT19937.b)
    y = y ^ ((y << MT19937.t) & MT19937.c)
    y = y ^ (y >> MT19937.l)
    self.vec_index += 1
    return y & ((1 << MT19937.w) - 1)

def get_bit(x, i):
  return (x & (1 << (MT19937.w - i - 1)))

def reverse_bits(x):
  rev = 0
  for i in range(MT19937.w):
    rev = (rev << 1)
    if(x > 0):
      if (x & 1 == 1):
        rev = (rev ^ 1)
      x = (x >> 1)
  return rev

def inv_left(y, a, b):
  return reverse_bits(inv_right(reverse_bits(y), a, reverse_bits(b)))

def inv_right(y, a, b):
  x = 0
  for i in range(MT19937.w):
    if (i < a):
      x |= get_bit(y, i)
    else:
      x |= (get_bit(y, i) ^ ((get_bit(x, i - a) >> a) & get_bit(b, i)))
  return x

def untemper(y):
  x = y
  x = inv_right(x, MT19937.l, ((1 << MT19937.w) - 1))
  x = inv_left(x, MT19937.t, MT19937.c)
  x = inv_left(x, MT19937.s, MT19937.b)
  x = inv_right(x, MT19937.u, MT19937.d)
  return x
