# Acceptance tests

Run `bash tests/run_tests.sh` from a fresh clone. Generated fixtures live in ignored `tests/fixtures/`.
Expected files contain acceptance criteria derived independently of program output. Actual files capture the executed program.
TC01–TC07 map to section 7 of P5. Generic not-found, invalid, empty and zero-denominator categories are also covered by TC08–TC10.

| ID | Scenario | Input (command) | Expected | Actual | Pass/Fail |
|---|---|---|---|---|---|
| TC01 | P5 TC01 / normal sample | `bin/p5 data/sample P001` | Ranked rows [('P005', 7, 12, 1, 8), ('P002', 7, 2, 0, 7), ('P003', 10, 0, -5, 5), ('P008', 4, 3, 1, 5), ('P007', 4, 4, 1, 5), ('P006', 4, 1, 0, 4), ('P004', 3, 5, 1, 4)]; Top 3 P005 P002 P003; P003 rank 3; best S001 P005 | [output](actual/TC01.txt) | Pass |
| TC02 | P5 TC02 / exact 20% and 0.3 boundaries | `bin/p5 data/full P001 S001 40` | P040 Sim=7, stock=5, adj=1, final=8; neither close-price nor close-rating reason | [output](actual/TC02.txt) | Pass |
| TC03 | P5 TC03 / ID tie-break with P039 | `bin/p5 data/full P001` | Top 3 P009, P039, P010; P009 precedes P039 when Final, Sim and rating tie | [output](actual/TC03.txt) | Pass |
| TC04 | P5 TC04 / dirty lines and duplicate keys in full | `bin/p5 data/full P001` | 39 candidates; 7 warnings; first valid P005 and S001/P004 retained; unknown P999 skipped | [output](actual/TC04.txt) | Pass |
| TC05 | P5 TC05 / empty dataset | `bin/p5 data/edge/empty P001` | Exit 1; clear query-not-found error; no crash | [output](actual/TC05.txt) | Pass |
| TC06 | P5 TC06 / availability elsewhere | `bin/p5 data/full P001 S001 15` | P003 appears at rank 15; S002 stock 13 and S003 stock 8 listed | [output](actual/TC06.txt) | Pass |
| TC07 | P5 TC07 / inclusive max-price filter | `bin/p5 data/full P001 S002 40 3990000` | P003 at price 3990000 retained; every candidate price <=3990000 | [output](actual/TC07.txt) | Pass |
| TC08 | Generic not found | `bin/p5 data/full UNKNOWN` | Exit 1; query not found | [output](actual/TC08.txt) | Pass |
| TC09 | Dirty dataset with no valid candidate | `bin/p5 data/edge/dirty P001` | 4 warnings; no candidates; valid S001/P001=4 retained after earlier invalid row | [output](actual/TC09.txt) | Pass |
| TC10 | Generic division-by-zero guard | `bin/p5 tests/fixtures/zero P001` | P002 Sim=8, Final=9; no price points for zero query price | [output](actual/TC10.txt) | Pass |
| TC11 | Missing stock line | `bin/p5 tests/fixtures/missing-stock P001` | P002 Sim=10, stock=0, adj=-5, Final=5 | [output](actual/TC11.txt) | Pass |
| TC12 | Invalid CLI S001 0 | `bin/p5 data/sample P001 S001 0` | Exit 1; clear error containing K must | [output](actual/TC12.txt) | Pass |
| TC13 | Invalid CLI S001 -1 | `bin/p5 data/sample P001 S001 -1` | Exit 1; clear error containing K must | [output](actual/TC13.txt) | Pass |
| TC14 | Invalid CLI S001 abc | `bin/p5 data/sample P001 S001 abc` | Exit 1; clear error containing K must | [output](actual/TC14.txt) | Pass |
| TC15 | Invalid CLI S001 3 abc | `bin/p5 data/sample P001 S001 3 abc` | Exit 1; clear error containing max_price must | [output](actual/TC15.txt) | Pass |
| TC16 | Invalid CLI S001 3 0 | `bin/p5 data/sample P001 S001 3 0` | Exit 1; clear error containing max_price must | [output](actual/TC16.txt) | Pass |
| TC17 | Invalid CLI --brand | `bin/p5 data/sample P001 --brand` | Exit 1; clear error containing missing value | [output](actual/TC17.txt) | Pass |
| TC18 | Invalid CLI --unknown | `bin/p5 data/sample P001 --unknown` | Exit 1; clear error containing unknown option | [output](actual/TC18.txt) | Pass |
| TC19 | Invalid CLI S001 2147483648 | `bin/p5 data/sample P001 S001 2147483648` | Exit 1; clear error containing K must | [output](actual/TC19.txt) | Pass |
| TC20 | Invalid CLI S001 3 9223372036854775808 | `bin/p5 data/sample P001 S001 3 9223372036854775808` | Exit 1; clear error containing max_price must | [output](actual/TC20.txt) | Pass |
| TC21 | Invalid CLI --brand Nike --brand Nike | `bin/p5 data/sample P001 --brand Nike --brand Nike` | Exit 1; clear error containing repeated option | [output](actual/TC21.txt) | Pass |
| TC22 | Required arguments missing | `bin/p5` | Exit 1; usage printed | [output](actual/TC22.txt) | Pass |
| TC23 | Missing input files | `bin/p5 tests/fixtures/no-such-directory P001` | Exit 1; cannot open products.csv | [output](actual/TC23.txt) | Pass |
| TC24 | Brand filter and bounded K | `bin/p5 data/full P001 S001 100 --brand Nike` | All ranked candidates are Nike; Top-K limited to candidate count | [output](actual/TC24.txt) | Pass |
| TC25 | Filter leaves no candidates | `bin/p5 data/sample P001 --brand Unknown` | Exit 0; no candidates; Top 0 and best-store table still present | [output](actual/TC25.txt) | Pass |
| TC26 | Max-price sample from brief | `bin/p5 data/full P001 S002 3 4000000` | 8 excluded; best S001 P039, S002 P003, S003 P003 | [output](actual/TC26.txt) | Pass |
| TC27 | Optional CSV export | `bin/p5 data/sample P001 --export tests/fixtures/top.csv` | Header rank,store_id,product_id,sim,stock,final and exactly 3 correct rows | [output](actual/TC27.txt) | Pass |
| TC28 | Export path cannot open | `bin/p5 data/sample P001 --export tests/fixtures/missing/out.csv` | Exit 1; explicit export open error | [output](actual/TC28.txt) | Pass |
| TC29 | long long extremes without multiplication overflow | `bin/p5 tests/fixtures/limits P001` | P003 Sim=10 Final=11; P002 Sim=8 Final=8 | [output](actual/TC29.txt) | Pass |
| TC30 | Windows CRLF input | `bin/p5 tests/fixtures/crlf P001` | P002 Sim=10 stock=2 adj=0 Final=10 | [output](actual/TC30.txt) | Pass |
| TC31 | Unknown store treated as all zero stock | `bin/p5 data/sample P001 S999 40` | Every candidate stock=0; known-store report still ranks S001 | [output](actual/TC31.txt) | Pass |
| TC32 | All valid full queries / all stores | `python3 tests/verify.py` | 120 query/store combinations match independent exact-arithmetic oracle | [output](actual/TC32.txt) | Pass |

## Failure policy

Retain failed evidence and record its cause and corrective PR when applicable. TC26 initially failed because its manually authored expectation incorrectly used the sample winner P005 for full data. Full-data P039 has the same Final and Sim but a higher rating, so it wins at S001. The expectation was corrected against the independent oracle. Evidence is in `history/TC26-initial-output.txt`; no C++ change was needed. This correction is included in the initial direct-push commit.

## Data integrity

Official CSV bytes are preserved. The fixtures for zero price, missing stock, large integers and CRLF are generated separately.
Original CSV SHA-256 checksums are listed in `data/SHA256SUMS`.
