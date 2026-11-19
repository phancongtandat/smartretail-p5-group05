# SmartRetail P5 — Product Similarity & Substitution

A C++17 command-line program that ranks substitute products using category, brand, price, rating and store stock. Includes the original datasets, exact integer scoring, handwritten sorts, recursive binary search, CSV export, 22 editable function flowcharts with pseudocode, an acceptance suite and a nine-slide presentation.

Prepared for **Phan Công Tấn Đạt / phancongtandat**, 19 November 2026. This is a new AI-assisted implementation repository; its history does not represent the original group's contributions. It is not the existing `minhanhhOvO/smartretail-p5-group05` repository.

## Build and run

Requirements: a C++17 compiler (g++ or clang++); Python 3 only for the optional verification suite.

```sh
mkdir -p bin
g++ -std=c++17 -Wall -Wextra src/main.cpp -o bin/p5
bin/p5 data/sample P001
bin/p5 data/full P001 S002 5 4000000
bin/p5 data/full P001 S001 5 --brand Nike
bin/p5 data/full P001 S001 5 --export bin/top5.csv
```

```text
p5 <data_folder> <product_id> [store_id] [K] [max_price] [--brand <brand>] [--export <file.csv>]
```

Defaults: store `S001`, K `3`, no price cap or brand filter. Positional arguments retain this order; named options can appear between them. Quote a brand/path containing spaces. K and an explicitly supplied max_price must be positive decimal integers. K is bounded by the number of remaining candidates. Exit status is 0 for success, 1 for invalid arguments, unavailable/invalid input, missing query or failed export. Invalid CSV records are skipped with line-numbered warnings on stderr.

## Rules and expected outcome

| Check | Points / behavior |
|---|---|
| Same category | +4 |
| Same brand | +3 |
| Absolute price gap strictly below 20% of query price | +2; no points if query price is zero |
| Absolute rating gap strictly below 0.3 | +1 |
| Stock 0 / stock 1–2 / stock >=3 | adjustment -5 / 0 / +1 |
| Final | similarity + stock adjustment; out-of-stock rows remain visible |
| Order | Final descending, Sim descending, rating descending, product ID ascending |
| Filters | Exclude query; remove price > cap and nonmatching brand before scoring |

Prices use nonnegative `long long`; ratings use exact integer tenths. `gap <= (q.price - 1) / 5`, guarded by `q.price > 0`, implements the strict price boundary without floating-point rounding or multiplying large integers.

The full table shows every eligible candidate. Top-K includes stock labels and scoring reasons, other-store availability for out-of-stock entries, and a note when a most-similar out-of-stock item is overtaken by a lower-similarity item. Best substitutes are independently re-ranked for every known store, with the current store marked. A supplied price cap also prints the number excluded by that cap.

For `bin/p5 data/sample P001`, the complete ranking is:

| ID | Sim | Stock | Adj | Final |
|---|---:|---:|---:|---:|
| P005 | 7 | 12 | +1 | 8 |
| P002 | 7 | 2 | 0 | 7 |
| P003 | 10 | 0 | -5 | 5 |
| P008 | 4 | 3 | +1 | 5 |
| P007 | 4 | 4 | +1 | 5 |
| P006 | 4 | 1 | 0 | 4 |
| P004 | 3 | 5 | +1 | 4 |

Top-3: P005, P002, P003. P003 is most similar but falls to rank 3 because stock is zero. The sample contains only S001, so its other-store message is `no other store`; the full dataset demonstrates S002/S003 availability.

## Verification

```sh
bash tests/run_tests.sh
```

The script builds with `-std=c++17 -Wall -Wextra -Werror`, runs 32 acceptance cases and checks all 120 full-dataset query/store combinations using a separate Python Decimal/integer oracle. It regenerates ignored temporary fixtures, expected acceptance criteria, captured stdout/stderr/exit status and the test table. No third-party Python modules are needed. Original data is preserved byte-for-byte; `data/SHA256SUMS` records its checksums.

- [Test cases and actual results](tests/test_cases.md)
- [Machine-readable results](tests/results.json)
- [Original corrected test expectation](tests/history/TC26-initial-output.txt)
- [Requirements and traps](docs/requirements.md)
- [Architecture and algorithms](docs/architecture.md)
- [Function flowcharts and pseudocode](docs/flowcharts/README.md)
- [Slides (9 pages)](docs/slides.pdf)
- [Demo and oral-exam preparation](docs/demo.md)

## Task assignment table

One functional ID covers its code, pseudocode, flowcharts and verification; there are no separate T21/T22 IDs for the similarity documentation. The allocation below records the group's previously supplied plan for reference, not authorship of this new implementation. All implementation/documents in this repository were assembled with AI assistance for Đạt. Review and PR columns intentionally do not invent course evidence.

| ID | Task / functions | Planned assignee | Planned reviewer | Flowchart + pseudocode | PR |
|---|---|---|---|---|---|
| T01 | Product, StockItem, Match, Options; parseUnsigned, toTenths, splitCsv, warn | Khôi | Hùng | [T01](docs/flowcharts/T01-index.md) | Direct push |
| T02 | loadProducts | Hùng | Đạt | [T02](docs/flowcharts/T02-loadProducts.md) | Direct push |
| T03 | loadInventory | Anh | Hùng | [T03](docs/flowcharts/T03-loadInventory.md) | Direct push |
| T04 | findProduct, stockOf | Đạt | Hùng | [T04](docs/flowcharts/T04-index.md) | Direct push |
| T05 | sortProductsById | Hùng | Anh | [T05](docs/flowcharts/T05-sortProductsById.md) | Direct push |
| T06 | comesBefore, sortMatches | Hùng | Anh | [T06](docs/flowcharts/T06-index.md) | Direct push |
| T07 | similarity | Đạt | Khôi | [T07](docs/flowcharts/T07-similarity.md) | Direct push |
| T08 | availabilityAdjustment, availableElsewhere | Anh | Đạt | [T08](docs/flowcharts/T08-index.md) | Direct push |
| T09 | rankMatches | Khôi | Đạt | [T09](docs/flowcharts/T09-rankMatches.md) | Direct push |
| T10 | printReport, ratingText | Khôi | Anh | [T10](docs/flowcharts/T10-index.md) | Direct push |
| T11 | printTopK | Anh | Khôi | [T11](docs/flowcharts/T11-printTopK.md) | Direct push |
| T12 | printBestSubstitutes | Anh | Hùng | [T12](docs/flowcharts/T12-printBestSubstitutes.md) | Direct push |
| T13 | parseArguments, main | Đạt | Khôi | [T13](docs/flowcharts/T13-index.md) | Direct push |
| T14 | exportTopK (optional extension) | Hùng | Anh | [T14](docs/flowcharts/T14-exportTopK.md) | Direct push |
| T15 | Repository skeleton and PR template | Hùng | Anh | N/A | Direct push |
| T16 | README and task mapping | Anh | Hùng | N/A | Direct push |
| T17 | Test-case table | Khôi | Đạt | N/A | Direct push |
| T18 | Expected outputs, captured outputs and test script | Anh | Hùng | N/A | Direct push |
| T19 | Architecture and requirements traceability | Hùng | Khôi | N/A | Direct push |
| T20 | Slides and demo instructions | Đạt | Anh | N/A | Direct push |

Original group names: Đặng Hoàng Minh Anh, Phan Công Tấn Đạt, Nguyễn Minh Hùng, Đặng Anh Khôi. Student IDs and other members' GitHub accounts were not supplied and have not been invented.

## Input contract and assumptions

- P5 reads only `products.csv` and `inventory.csv`. Other CSVs from the original archive are retained unchanged but unused.
- Exact headers are required. CSV is simple unquoted comma-separated text; quoted commas/embedded newlines are outside the supplied format. CRLF and LF are accepted. Whitespace is not silently trimmed; field/brand comparisons are case-sensitive.
- Products require six fields, nonempty identity/name/category/brand, a decimal nonnegative price <= LLONG_MAX and a rating of exactly one decimal place in [0.0,5.0]. Inventory requires three fields, nonempty IDs and a nonnegative stock <= INT_MAX.
- Keep the first **valid** occurrence of a product ID or `(store_id, product_id)` inventory key. Earlier invalid records do not reserve keys. Inventory references to unknown products are warned and skipped.
- Missing stock means 0. An unknown requested store is treated as having no stock for all candidates; the best-by-store section lists stores present in valid inventory. An empty valid dataset reports query-not-found and exits 1.
- The max-price exclusion count counts non-query products over the cap before applying the brand filter. A product exactly at the cap remains eligible.
- Best substitute means first under the specified total ranking, even if every candidate is out of stock; no extra availability filter is introduced.
- Export uses the selected store and filtered Top-K with exact header `rank,store_id,product_id,sim,stock,final`. The destination parent directory must exist.

## AI assistance, references and submission status

ChatGPT/Codex generated and integrated the C++ implementation, flowcharts, pseudocode, verification suite, documentation and slide deck at the user's request. The program was checked by an independent arithmetic oracle and acceptance tests; this does not establish students' independent understanding. Every member must be able to explain the submitted code during the oral examination.

References: supplied **SmartRetail P5 brief (Vietnamese/English)**; **SmartRetail GitHub guide**; latest supplied **submission guide, version (3)**; original SmartRetail data ZIP; the group's previously supplied `similarity()` function and task table. Scoring and traps are traced in [requirements](docs/requirements.md). No instructor handout is redistributed in this repository.

This repository is pushed directly to main at the user's explicit request. PR/review evidence, accepted group/lecturer collaborator invitations, LMS registration, milestone submissions and live demo/viva remain external requirements. The included PR template is for later use and is not evidence of completed reviews. No full official-course compliance or personal contribution score is claimed. See [submission status](docs/submission-status.md).
