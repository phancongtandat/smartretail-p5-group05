# SmartRetail P5

Product similarity and store-aware substitution

- Prepared for Phan Cong Tan Dat | phancongtandat
- New AI-assisted implementation | 19 November 2026
- C++17 CLI + original CSV data + editable flowcharts + verification
- No invented group contributions, student IDs or review history.

# 1. Problem and observable outcome

Choose a useful substitute, not just a similar item

- Input: products.csv, inventory.csv, query product and store.
- Output: complete ranked table, Top-K reasons and stock labels.
- Out-of-stock candidates stay visible; suggest other available stores.
- Recompute the best substitute separately for every valid store.

# 2. Program structure

Load -> validate -> search -> filter -> score -> rank -> report

- Product / StockItem / Match / Options records stored in vectors.
- Handwritten selection sorts; recursive binary search after ID sort.
- One src/main.cpp; function documentation grouped by T01-T14.
- Exit 1 for fatal errors; warn and skip malformed or duplicate rows.

# 3. Exact scoring and boundary traps

Independent criteria; exact integer arithmetic

- Same category +4; same brand +3; price gap <20% +2; rating gap <0.3 +1.
- Price predicate: q.price > 0 AND gap <= (q.price-1) / 5.
- Ratings are integer tenths: abs(c.ratingTenths-q.ratingTenths) < 3.
- P040 versus P001: exactly20% and0.3 -> neither boundary earns points.

# 4. Stock changes the ranking

Final = Sim + adjustment

- Stock0 -> -5; stock1-2 -> 0; stock>=3 -> +1.
- Order: Final desc, Sim desc, rating desc, product ID asc.
- Sample Top3: P005 Final8; P002 Final7; P003 Final5.
- P003 Sim10 falls to rank3; in full, it is rank15 at S001.

# 5. Filters, stores and export

Apply identical eligibility rules to each store

- Exclude query; retain price==cap; filter brand before scoring.
- Full P001, cap4000000: 8 price exclusions.
- Best products: S001 P039; S002 P003; S003 P003.
- CSV header: rank,store_id,product_id,sim,stock,final.

# 6. Verification and honest evidence

32 acceptance cases + 120 query/store oracle comparisons

- Build with -std=c++17 -Wall -Wextra -Werror.
- Checks: sample/full/dirty/empty, boundary ties, bad CLI, zero price, export.
- Independent Python Decimal/integer reference checks complete ranking.
- Retain corrected TC26 expectation and actual stdout/stderr/exit codes.

# 7. Live demonstration

Explain one trace, then show the difficult cases

- bin/p5 data/sample P001
- bin/p5 data/full P001 S001 15
- bin/p5 data/full P001 S002 5 4000000
- Export, invalid K, empty data; open T07 flowchart and run tests.

# 8. Scope, team and submission

Technical artifacts are prepared; people must verify their work

- Original plan: Anh, Dat, Hung, Khoi; no authorship inferred from that plan.
- AI assistance is disclosed for code, diagrams, tests, docs and slides.
- Direct push chosen by user: PR/review contribution evidence is absent.
- Group registration, accepted invitations, live demo and viva remain external.
