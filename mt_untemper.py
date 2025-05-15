from mt19937_engine import MT19937, untemper
import random

def compare_streams(r1, r2, lim=100000):
  """
  Compare the first `lim` outputs of two zero-argument generators r1() and r2().
  """
  for i in range(lim):
    # v1 = r1()
    # v2 = r2()
    if r1() != r2():
      print("Outputs differ at index:", i)
      return
  print(f"{lim} outputs match")

def main():
  rng1 = MT19937(0)
  rng1_clone = MT19937()
  for i in range(MT19937.n):
    rng1_clone.state_vec[i] = untemper(rng1.temper())
  rng1_clone.twist()

  print("=== MT19937 clone ===")
  compare_streams(
      rng1.temper,
      rng1_clone.temper
  )

  rng_py = random.Random(0)
  rng_py_clone = MT19937()
  for i in range(MT19937.n):
    rng_py_clone.state_vec[i] = untemper(rng_py.getrandbits(MT19937.w))
  rng_py_clone.twist()

  print("\n=== Python random.Random ===")
  compare_streams(
    lambda: rng_py.getrandbits(MT19937.w),
    rng_py_clone.temper
  )

if __name__ == "__main__":
    main()
