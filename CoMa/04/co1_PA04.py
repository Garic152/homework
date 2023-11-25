def gcd(a, b):
    while b:
        a, b = b, a % b
    return a


def sieve(n):
    # make bool array with length n
    prime = [True for _ in range(n+1)]

    p = 2

    # make use of the square root with p*p
    while (p * p <= n):
        # if it is not a prime yet loop through its multiples
        if (prime[p]):
            # update all multiples
            for i in range(p * p, n+1, p):
                prime[i] = False
        p += 1

    # Save the prime numbers in new array
    primes = []
    for p in range(2, n+1):
        if prime[p]:
            primes.append(p)

    return primes


def isprime(n):
    if n < 2:
        return None

    primes = sieve(n)

    return n in primes


def factorization(n):
    if n < 2:
        return None
    # first declare return list
    factors = []

    # get prime numbers and reverse
    primes = sieve(n)
    primes.reverse()

    for prime in primes:
        exponent = 1

        while n % (prime ** exponent) == 0:
            exponent += 1

        if exponent > 1:
            factors.append([prime, exponent - 1])

    factors.reverse()

    return factors


def euler_phi(n):
    if n < 1:
        return None

    amount = 0
    for i in range(1, n + 1):
        if gcd(n, i) == 1:
            amount += 1
    return amount


def iscoprime(m, n):
    if n < 1 or m < 1:
        return None

    if gcd(m, n) == 1:
        return True
    else:
        return False
