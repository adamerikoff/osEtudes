ONE = 1
IS_ZERO = lambda x: x == 0
SUB1 = lambda x: x - 1
MULT = lambda x: lambda y: x * y 
IF = lambda cond: lambda t_func: lambda f_func: t_func(None) if cond else f_func(None)    

def fact(n):
    if n == 0:
        return 1
    else:
        return n * fact(n - 1)
    

print("ordinary function: ")
print(fact(6))
print("lambda function: ")
print(
    (
        lambda myself: (
            lambda n: (
                IF(
                    IS_ZERO(n)
                )(
                    lambda _: ONE
                )(
                    lambda _: MULT(n)( myself(myself)(SUB1(n)) )
                )
            )
        )
    )(
        lambda myself: (
            lambda n: (
                IF(
                    IS_ZERO(n)
                )(
                    lambda _: ONE
                )(
                    lambda _: MULT(n)( myself(myself)(SUB1(n)) )
                )
            )
        )
    )(6)
)