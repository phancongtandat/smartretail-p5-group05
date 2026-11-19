# Verification record — 19 November 2026

## Executed checks

| Check | Observed result |
|---|---|
| C++17 compile with `-Wall -Wextra -Werror` | Success; no warnings |
| `bash tests/run_tests.sh` in working copy | 32/32 acceptance cases; 120/120 query/store combinations |
| Fresh clone of GitHub main, rebuild and same test script | Same passing results; no tracked-file changes after regeneration |
| AddressSanitizer and UndefinedBehaviorSanitizer | Sample, full, LLONG_MAX fixture, dirty and empty scenarios passed; empty returned expected exit1 |
| Original data against supplied ZIP and SHA256SUMS | All original files byte-identical |
| Local Markdown links | All referenced local files exist |
| Editable diagrams | 22 XML sources parse; PNGs contain mxfile metadata |
| Presentation | Nine PDF pages rendered and inspected; no clipped slide content |
| Selected diagram inspection | Similarity, nested sorting loops and main/error exits rendered and inspected |

## Reproduce

```sh
bash tests/run_tests.sh
```

Sanitizer build used on macOS:

```sh
g++ -std=c++17 -Wall -Wextra -Werror \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  src/main.cpp -o bin/p5-sanitized
bin/p5-sanitized data/sample P001
bin/p5-sanitized data/full P001 S001 40
bin/p5-sanitized tests/fixtures/limits P001
bin/p5-sanitized data/edge/dirty P001
bin/p5-sanitized data/edge/empty P001
```

Run the regular suite first to generate the ignored limits fixture. The empty case intentionally exits1. Test evidence is committed under tests/actual, with independently authored acceptance criteria under tests/expected and the executable reference in tests/verify.py. These checks validate program behavior and artifacts, not authentic group contributions or course administrative completion.
