# Requirements, outcomes and traps

This matrix applies the supplied P5 brief and latest submission guide (version (3)); original task/function IDs are grouped rather than splitting documentation into extra functional tasks. It records executable/artifact evidence separately from human/team submission obligations.

| Requirement / trap | Implementation or evidence |
|---|---|
| C++17, one main.cpp, course-level constructs, clean warnings | src/main.cpp; tests/run_tests.sh compiles with -Werror |
| struct/vector, functions, references, file input, recursive function | Product/StockItem/Match; const/mutable references; findProduct recursion |
| Handwritten search and sort; no library sort | Recursive binary search and selection sorts |
| P5 inputs only products/inventory, original data unchanged | data/SHA256SUMS; SHA256 / original archive verified |
| Correct CSV errors, duplicate rules and empty inputs | TC04, TC05, TC09, TC23; stderr line numbers |
| Category +4 and brand +3 independent | similarity; TC01 and 120-case oracle sweep |
| Strict price <20%, rating <0.3 | Integer boundary condition and ratingTenths; TC02 P040 gets neither boundary point |
| Guard zero query price, large price safety | TC10 zero-price fixture, TC29 long-long limit fixture |
| Query excluded; filters before scoring; cap equality kept | rankMatches; TC07, TC24, TC25, TC26 |
| Keep stock 0 visible; stock adjustments -5/0/+1 | TC01, TC11 missing stock, 120-case sweep |
| Rank Final, Sim, rating desc, ID asc | comesBefore; TC01, TC03 P039, oracle sweep |
| Full table, Top-K stock labels and reasons | printReport/printTopK; TC01 and captured outputs |
| Clamp K and handle no candidates | TC09, TC24, TC25 |
| Explain most-similar item demoted by stock | P003 sample rank 3; full rank 15; TC01/TC06 |
| Other stores for out-of-stock Top-K | TC06: S002 stock13 and S003 stock8 |
| Best per store must recompute stock/ranking and apply filters | TC26: cap4000000 -> S001 P039, S002 P003, S003 P003 |
| Count price exclusions | TC26: 8; count does not silently become brand-filter count |
| Non-numeric, nonpositive or overflowing CLI values | TC12–TC22; exit1 and clear errors |
| Optional CSV export and file failures | TC27/TC28: exact header, rows, error path |
| CRLF accepted, no mixed-line-ending churn | TC30; .gitattributes LF for source/docs |
| Unknown store assumption documented | TC31: missing stock treated as0 |
| All core functions documented with correct symbols | docs/flowcharts index; 22 PNGs + XML + pseudocode |
| Global architecture: flow/core calls/types/I/O/algorithm rationale | docs/architecture.md sections1–5 |
| Reproducible tests: expected vs actual, retain failures | tests/test_cases.md, expected/, actual/, history/ |
| <=10 slides; live demo and any-task viva preparation | docs/slides.pdf (9 pages); docs/demo.md |
| Public repo, final code tag and clean-clone verification | GitHub repository; final tag after validation |
| Member accounts/MSSV, invitations, LMS registration | Not fabricated; see submission-status.md |
| Contribution PRs/reviews, code ownership, approvals | User requested direct push; not claimed completed |

## Official P5 TC01–TC07 versus general test categories

The P5 brief's TC01–TC07 are retained as the first seven acceptance cases: normal sample, exact scoring boundaries, ID tie-break, dirty/full data, empty data, elsewhere availability and price cap. Generic submission-guide categories (normal, boundary, not-found, invalid, empty, division-by-zero and project-specific traps) are additionally covered; they are not renumbered to overwrite the P5 cases.

## Data observations

Full: 40 valid products, 39 candidates per unfiltered query, 3 stores and 7 skipped lines (4 product and 3 inventory warnings). Sample: 8 products and 7 candidates. The sample's P003 is most similar with Sim10 but stock0, Final5 and rank3. In full it falls to rank15; Top-K must be >=15 to demonstrate its other-store availability. P040 is exactly 20% above P001 and exactly0.3 away in rating, so Sim7, not10. P039 demonstrates the final ID tie-break.

A test expectation was initially wrong for the filtered best-in-S001 result: P039, not P005, is the true full-data winner at cap4000000. The expectation was corrected against the independent oracle; original failure output is retained in tests/history. No implementation change or fabricated passing history hides that correction.

## Assessment and scope limits

Automated passing tests support the technical behavior; they do not replace the lecturer's grading, live demonstration, independent explanations, milestone submissions or authentic individual contributions. The optional export is implemented but no bonus or final grade is promised. This direct-push repository cannot supply the PR/review trail required for that portion of the course assessment. Later fixes must follow the latest guide's definition of a fix; they must not be represented as new substantive contributions merely to increase counts.
