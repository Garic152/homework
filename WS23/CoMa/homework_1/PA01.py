def roots(a1, a2, a3, b1, b2, b3):
    c = [1, a1*b1, a1*b2+a2*b1, a1*b3+a2*b2+a3*b1, a2*b3+a3*b2, a3*b3]


    changes = sum((a <= 0) != (b <= 0) for a, b in zip(c, c[1:]))

    if changes % 2 != 0:
        return("Das Polynom hat eine ungerade Anzahl von positiven reellen Nullstellen.")
    elif changes % 2 == 0:
        return("Das Polynom hat eine gerade Anzahl von positiven reellen Nullstellen.")