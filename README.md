# RAM & Cache Latency Checker

This project measures **L1, L2, L3 cache** and **RAM latency** in CPU cycles.  
It tries to minimize the effects of **cache prefetching, page faults, and context switches** for more accurate results.

---

## Features

- Measures latency for:
  - **L1 Cache**
  - **L2 Cache**
  - **L3 Cache**
  - **RAM**
- Uses **pointer chasing** with **Fisher-Yates shuffled arrays** to get realistic memory access times.
- Can pin the measurement thread to a specific CPU core.
- Sets **high scheduling priority** to reduce OS interference.

---

## How to Use

1. Compile the program with GCC:

```bash
gcc -O2 -march=native -pthread -o latency_checker latency_checker.c
```

## Exemples
- ./latency_checker l1   # Measure L1 cache latency
- ./latency_checker l2   # Measure L2 cache latency
- ./latency_checker l3   # Measure L3 cache latency
- ./latency_checker ram  # Measure RAM latency
