#!/bin/python3
# Zajisteni zakladni kompatibility IFJ19->Python3 @ Merlin


def inputs():
    try:
        return input()
    except Exception:
        return None


def inputi():
    try:
        line = input()
        if line.count("0x") == 1:   # hexadecimalni format
            return int(line, 16)
        else:
            return int(line)
    except Exception:
        return None


def inputf():
    try:
        line = input()
        if line.count("p") == 1:    # hexadecimalni format
            return float.fromhex(line)
        else:
            return float(line)
    except Exception:
        return None


__len = len


def len(s):
    try:
        return __len(s)
    except Exception:
        return None


def substr(s, i, n):
    try:
        if i < 0 or i > __len(s) or n < 0:
            return None
        return s[i:i+n]
    except Exception:
        return None


__ord = ord


def ord(s, i):
    try:
        return __ord(s[i])
    except Exception:
        return None

# def chr(i):
#  chr(i)


__print = print


def print(*args):
    length = __len(args)
    for i, arg in enumerate(args):
        if arg.__class__ == float:
            __print(arg.hex(), end='')
        else:
            __print(arg, end='')
        if i < length - 1:
            __print(' ', end='')
    __print('\n', end='')
    return None

# Zde bude nasledovat program jazyka IFJ19

# Ukazka kolize lokalni a globalni promenne v IFJ19 (chyba 3)


a = 10  # globalni promenna


def f():
    print(a)  # pracuji pouze s globalni promennou, OK


def g():
    print(a)  # pracuji s globalni promennou, ale chyba 3 kvuli zbytku tela funkce
    a = 3
    return a


print(a)
f()
g()  # vyvola chybu 3
