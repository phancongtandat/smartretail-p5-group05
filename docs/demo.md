# Demo and oral-exam preparation

Use a clean clone with a C++17 compiler. Rebuild before demonstrating; do not depend on a committed executable.

```sh
mkdir -p bin
g++ -std=c++17 -Wall -Wextra src/main.cpp -o bin/p5
bin/p5 data/sample P001
bin/p5 data/full P001 S001 15
bin/p5 data/full P001 S002 5 4000000
bin/p5 data/full P001 S001 5 --brand Nike
bin/p5 data/full P001 S001 5 --export bin/top5.csv
cat bin/top5.csv
bin/p5 data/edge/dirty P001
bin/p5 data/edge/empty P001
bin/p5 data/sample UNKNOWN
bin/p5 data/sample P001 S001 abc
bash tests/run_tests.sh
```

## 5–10 minute route

1. Explain the shopper's substitute-selection problem and CLI (45 seconds).
2. Run sample: show all7 rows; manually derive P003 Sim10, adjustment-5, Final5, rank3 (90 seconds).
3. Run full Top15: explain P040 strict boundaries, P039 ID tie-break, dirty-row warnings and P003 other stores (90 seconds).
4. Show cap/brand filters and different best-by-store winners; P003 can win elsewhere because stock differs (60 seconds).
5. Export and inspect exact CSV header; demonstrate invalid K and empty input without crashing (60 seconds).
6. Show architecture, the T07 flowchart/pseudocode and testing evidence; declare AI assistance and actual submission status (60 seconds).

The tests themselves are evidence, not a substitute for explaining code. Let expected exit1 demonstrations finish; do not run the whole demo under shell `set -e` unless handling those statuses explicitly.

## Questions every member should answer

- Why are the four similarity checks separate `if` statements?
- Why does `(q.price-1)/5` implement a strict20% comparison, and why guard zero?
- Why store ratings in tenths? What happens at gap3 exactly?
- Why is the inventory key a pair, and what does missing stock mean?
- Trace the recursive search's interval and base case; why sort before searching?
- Explain each comparator tie-break, selection sort and worst-case complexity.
- Why do filters precede similarity/stock work? Why re-rank per store?
- Trace input/read/export error exits and first-valid-duplicate handling.
- Explain why exact cap equality is accepted and why P039 wins the filtered S001 example.
- Explain what the independent Python oracle checks and which workflow requirements it cannot verify.
