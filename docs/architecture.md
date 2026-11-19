# Architecture

## 1. End-to-end flow

[Global flowchart](flowcharts/T13-main.md) includes parse, input, query and export failure exits. CLI validation happens before file access. Products are loaded and sorted by ID before inventory validation and recursive lookup. Query lookup then feeds filtering, scoring, stock adjustment and sorting for the selected store. Reporting prints the full table, Top-K and independently recomputed per-store winners. Optional export writes the same Top-K. Errors are on stderr with exit 1; record-level warnings skip the record and continue.

## 2. Core function decomposition

| Stage | Functions | Contract |
|---|---|---|
| Input | parseArguments, splitCsv, parseUnsigned, toTenths | Validate without numeric exceptions; maintain exact integers |
| Load | loadProducts, loadInventory, warn | Keep first valid keys; invalid records never enter model |
| Search | sortProductsById, findProduct, stockOf | Sorted-ID prerequisite; recursive binary search; missing stock key -> index -1 |
| Score | similarity, availabilityAdjustment | Sim 0..10; adjustment -5/0/+1; final -5..11 |
| Rank | rankMatches, comesBefore, sortMatches | Exclude query/filter before scoring; strict four-level order |
| Report | ratingText, printReport, printTopK, availableElsewhere, printBestSubstitutes | No hidden removal of out-of-stock products; per-store stock recalculated |
| Export | exportTopK | Exact required header; open and write failures observable |

Each function's flowchart, editable XML and pseudocode live under its functional T01–T14 ID. A called documented function is represented as a predefined process. Outputs use parallelograms; assignments use rectangles; conditions use diamonds with Yes/No; returns use terminals. PNGs are rendered at 200% scale and include diagram XML; separate `.drawio` sources are also provided. There is no Mermaid task flowchart.

## 3. Data and call relationships

`Product` holds id, name, category, brand, long long price and int ratingTenths. `StockItem` holds storeId/productId/stock. `Match` contains a Product copy plus sim, stock, adjustment, finalScore and reasons. `Options` contains CLI selections and explicit option-presence flags. Data lives in vectors, passed by const reference when read and by mutable reference for outputs.

`main` calls parseArguments -> loadProducts -> sortProductsById -> loadInventory -> findProduct -> rankMatches -> printReport -> optional exportTopK. loadInventory uses findProduct and stockOf. rankMatches calls similarity, stockOf, availabilityAdjustment and sortMatches; sortMatches uses comesBefore. printReport calls printTopK and printBestSubstitutes; printTopK calls availableElsewhere; printBestSubstitutes calls rankMatches for each distinct valid store. findProduct recurses into itself on the smaller interval.

The only global objects are standard library facilities; there is no mutable global project state. Record types are global declarations. Input and report vectors/options have main scope; row temporaries, loop counters and scoring values have function/block scope. reasons and excluded are explicit reference outputs.

## 4. Inputs, outputs and failure behavior

Read-only source data: `data/{sample,full,edge/...}/{products,inventory}.csv`. P5 uses neither sales nor any unrelated archive file. Console: QUERY, full ranked table, Top-K with reasons/labels, elsewhere availability, stock-demotion notes, best substitute per valid store and optional max-price exclusion count. Optional output CSV contains the current store's filtered Top-K. No input file is rewritten.

Fatal errors: invalid arguments, unavailable files, invalid headers, file read failure, absent query and export failure. Recoverable data errors: wrong field count, malformed/out-of-range numbers, empty required fields, duplicate valid keys and inventory with unknown product IDs. Warnings identify file and physical line number. First valid duplicate wins.

## 5. Algorithms, rationale and complexity

Let N = valid products, M = valid inventory rows, C <= N-1 = eligible candidates and S = distinct stores.

| Operation | Time | Extra space | Rationale |
|---|---|---|---|
| Product duplicate detection | O(N²) total | O(N) records | Simple vector scan in permitted course scope |
| Sort products (selection) | O(N²) | O(1) sorting workspace | Handwritten, deterministic, prepares binary search |
| Recursive findProduct | O(log N) | O(log N) call stack | Base case left > right prevents invalid access |
| stockOf | O(M) | O(1) | Composite-key linear search |
| Inventory validation | O(M log N + M²) | O(M) records | Known-product check plus duplicate-key search |
| similarity | O(1) | O(1) bounded reason text | Four independent exact checks |
| Rank one store | O(N + C M + C²) | O(C) | Score each candidate and handwritten selection sort |
| Best-by-store | O(M S + S² + S(N + C M + C²)) | O(S+C) | Build/sort distinct stores; rank afresh per store |
| Top-K notes | O(C² + K M) worst case | O(1) besides result text | Detect demotion without misattributing rating/ID ties |

The small official datasets favor transparent course algorithms over optimization. No `std::sort`, map, set, custom class, inheritance or user-managed pointer data structure is used. The standard `char* argv[]` signature is solely the CLI entry point. The submitted C++ stays in one `src/main.cpp`; Python is an external testing tool, not a program dependency.

Prices are in [0, LLONG_MAX], so their difference is representable in [-LLONG_MAX, LLONG_MAX]. Guarded `(q.price-1)/5` avoids multiplication overflow and enforces strict 20%. Rating subtraction is bounded by 50. CLI and CSV integer conversion validate digit-by-digit with an overflow check before multiplying. Monetary units remain exactly those in the provided CSV.
