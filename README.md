# approx log2

A reasonably fast log2 implementation for ieee floats

## Algorithm description

we want to decompose x into a natural number and a fractional part

using the fact that any x will in some half-open interval `[ 2^n, 2^(n+1) )`

such that `2^-n * x` will be in the half-open interval `[1, 2)`

then `log2(x) = n + log2(x')`

where `x' = 2^-n * x`

this can be done efficiently for ieee754 floats,

given that they store numbers as `m * 2^n`.

After the initial normalization step, we iterate as follows:

    square `x'` until it is in the half-open interval `[2,4)`
    set `x_new = x'^(2^k)` where `k` is the number of times we had to square `x'`
    then `log2(x_new) = 2^k * log2(x')`
    thus `log2(x') = log2(x_new)/2^k = ( 1 + log2(x_new/2) )/2^k = 2^-k + 2^-k * log2(x_new/2)`
    set `x' = x_new` and continue iteration.

now collecting all those `k`s from the iterations lets us calculate `log2(x)`:

`log2(x) = n + 2^(-k1) * (1 + 2^(-k2) * (1 + 2^(-k3) * (1 + ...  )))`

`log2(x) = n + 2^(-k1) + 2^(-k1-k2) + 2^(-k1-k2-k3) + ...`

At any iteration where the remaining `x'` is found to be `1` we can terminate as the logarithmic expansion is finite.

The precision of the result depends on the number of iterations after which the algorithm is terminated.

The error is bounded as `err(m) < 2^(-(k1+k2+k3+...+km))`

which in turn implies that:

    1: we can get a fixed upper bound on the error independent of the input
        by setting a fixed number of iterations the worst case will be:
        `err(m) < 2^(-(1+1+1+...+1)) = 2^(-m)`

    2: instead of a fixed number of iterations we can look at the current error:
        `2^(-(k1+k2+k3+...)` and decided if it meets our required error bounds and then terminate
