#!/usr/bin/env python3
"""Black-box acceptance checks with an independent exact-arithmetic ranking oracle."""
import csv
import hashlib
import io
import json
from pathlib import Path
import subprocess
from decimal import Decimal

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / 'bin/p5'
EXPECTED = ROOT / 'tests/expected'
ACTUAL = ROOT / 'tests/actual'
FIXTURES = ROOT / 'tests/fixtures'
for directory in (EXPECTED, ACTUAL, FIXTURES):
    directory.mkdir(parents=True, exist_ok=True)
records = []


def reference(folder, query, store='S001', cap=None, brand=None):
    products = {}
    with (ROOT / folder / 'products.csv').open() as source:
        for row in list(csv.reader(source))[1:]:
            try:
                if len(row) != 6 or not row[4].isdigit():
                    continue
                price = int(row[4])
                rating = Decimal(row[5])
                if not 0 <= rating <= 5 or row[0] in products:
                    continue
                products[row[0]] = (row, price, rating)
            except (ValueError, ArithmeticError):
                continue
    inventory = {}
    with (ROOT / folder / 'inventory.csv').open() as source:
        for row in list(csv.reader(source))[1:]:
            if len(row) == 3 and row[2].isdigit() and row[1] in products:
                inventory.setdefault((row[0], row[1]), int(row[2]))
    q, qp, qr = products[query]
    ranked = []
    for pid, (row, price, rating) in products.items():
        if pid == query or (cap is not None and price > cap) or (brand is not None and row[3] != brand):
            continue
        sim = 4 * (row[2] == q[2]) + 3 * (row[3] == q[3])
        sim += 2 * (abs(price - qp) * 100 < 20 * qp)
        sim += int(abs(rating - qr) < Decimal('0.3'))
        stock = inventory.get((store, pid), 0)
        adjustment = -5 if stock == 0 else 0 if stock <= 2 else 1
        ranked.append((pid, sim, stock, adjustment, sim + adjustment, rating))
    return sorted(ranked, key=lambda item: (-item[4], -item[1], -item[5], item[0]))


def table(output):
    rows = []
    for line in output.splitlines():
        cells = [part.strip() for part in line.split('|')]
        if len(cells) == 6 and cells[0].startswith('P') and cells[2].isdigit():
            rows.append((cells[0], int(cells[2]), int(cells[3]), int(cells[4]), int(cells[5].split()[0])))
    return rows


def run_case(identifier, scenario, args, expected, check, code=0):
    command = ['bin/p5', *args]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    actual = 'STDOUT\n' + result.stdout + '\nSTDERR\n' + result.stderr + '\nEXIT ' + str(result.returncode) + '\n'
    (EXPECTED / (identifier + '.txt')).write_text(expected + '\n')
    (ACTUAL / (identifier + '.txt')).write_text(actual)
    try:
        assert result.returncode == code, f'exit {result.returncode}, expected {code}'
        check(result)
        status, detail = 'Pass', 'Acceptance assertions passed'
    except (AssertionError, ValueError, IndexError) as error:
        status, detail = 'Fail', str(error)
    record = dict(id=identifier, scenario=scenario, command=' '.join(command), expected=expected,
                  actual=f'[output](actual/{identifier}.txt)', status=status, detail=detail)
    records.append(record)
    print(identifier, status, detail)


def require(condition, detail='Unexpected output'):
    assert condition, detail


def match_reference(result, folder, query, store='S001', cap=None, brand=None):
    want = [tuple(row[:5]) for row in reference(folder, query, store, cap, brand)]
    require(table(result.stdout) == want, 'Ranked rows differ from independent oracle')


def fixture(name, products, inventory='S001,P001,3\n'):
    folder = FIXTURES / name
    folder.mkdir(exist_ok=True)
    (folder / 'products.csv').write_text('product_id,name,category,brand,price,rating\n' + products)
    (folder / 'inventory.csv').write_text('store_id,product_id,stock\n' + inventory)
    return folder.relative_to(ROOT).as_posix()


sample = [('P005', 7, 12, 1, 8), ('P002', 7, 2, 0, 7), ('P003', 10, 0, -5, 5),
          ('P008', 4, 3, 1, 5), ('P007', 4, 4, 1, 5), ('P006', 4, 1, 0, 4), ('P004', 3, 5, 1, 4)]
run_case('TC01', 'P5 TC01 / normal sample', ['data/sample', 'P001'],
         'Ranked rows ' + repr(sample) + '; Top 3 P005 P002 P003; P003 rank 3; best S001 P005',
         lambda r: (require(table(r.stdout) == sample), require('drops to rank 3' in r.stdout),
                    require('S001 | P005' in r.stdout), require('in stock at: no other store' in r.stdout),
                    require('reasons: category +4, brand +3, close price +2, close rating +1' in r.stdout),
                    require(r.stderr == '')))
run_case('TC02', 'P5 TC02 / exact 20% and 0.3 boundaries', ['data/full', 'P001', 'S001', '40'],
         'P040 Sim=7, stock=5, adj=1, final=8; neither close-price nor close-rating reason',
         lambda r: (match_reference(r, 'data/full', 'P001'),
                    require(('P040', 7, 5, 1, 8) in table(r.stdout)),
                    require('P040 Nike Pegasus 41 GTX | 8 pts | 5 in stock\n   reasons: category +4, brand +3\n' in r.stdout)))
run_case('TC03', 'P5 TC03 / ID tie-break with P039', ['data/full', 'P001'],
         'Top 3 P009, P039, P010; P009 precedes P039 when Final, Sim and rating tie',
         lambda r: (match_reference(r, 'data/full', 'P001'),
                    require([x[0] for x in table(r.stdout)[:3]] == ['P009', 'P039', 'P010'])))
run_case('TC04', 'P5 TC04 / dirty lines and duplicate keys in full', ['data/full', 'P001'],
         '39 candidates; 7 warnings; first valid P005 and S001/P004 retained; unknown P999 skipped',
         lambda r: (require(len(table(r.stdout)) == 39), require(r.stderr.count('Warning ') == 7),
                    require('products.csv:42:' in r.stderr), require('inventory.csv:124:' in r.stderr),
                    require(('P004', 3, 5, 1, 4) in table(r.stdout))))
run_case('TC05', 'P5 TC05 / empty dataset', ['data/edge/empty', 'P001'],
         'Exit 1; clear query-not-found error; no crash',
         lambda r: require('query product not found: P001' in r.stderr), code=1)
run_case('TC06', 'P5 TC06 / availability elsewhere', ['data/full', 'P001', 'S001', '15'],
         'P003 appears at rank 15; S002 stock 13 and S003 stock 8 listed',
         lambda r: (require('15. P003' in r.stdout),
                    require('in stock at: S002 (stock 13), S003 (stock 8)' in r.stdout)))
run_case('TC07', 'P5 TC07 / inclusive max-price filter', ['data/full', 'P001', 'S002', '40', '3990000'],
         'P003 at price 3990000 retained; every candidate price <=3990000',
         lambda r: (match_reference(r, 'data/full', 'P001', 'S002', 3990000),
                    require('P003' in [x[0] for x in table(r.stdout)])))
run_case('TC08', 'Generic not found', ['data/full', 'UNKNOWN'], 'Exit 1; query not found',
         lambda r: require('query product not found: UNKNOWN' in r.stderr), code=1)
run_case('TC09', 'Dirty dataset with no valid candidate', ['data/edge/dirty', 'P001'],
         '4 warnings; no candidates; valid S001/P001=4 retained after earlier invalid row',
         lambda r: (require(r.stderr.count('Warning ') == 4), require('No matching candidates.' in r.stdout),
                    require('S001 | no matching candidates <- current store' in r.stdout)))
zero = fixture('zero', 'P001,Zero,C,B,0,4.0\nP002,Also Zero,C,B,0,4.0\n', 'S001,P001,3\nS001,P002,3\n')
run_case('TC10', 'Generic division-by-zero guard', [zero, 'P001'], 'P002 Sim=8, Final=9; no price points for zero query price',
         lambda r: require(table(r.stdout) == [('P002', 8, 3, 1, 9)]))
missing = fixture('missing-stock', 'P001,Query,C,B,100,4.0\nP002,Other,C,B,100,4.0\n')
run_case('TC11', 'Missing stock line', [missing, 'P001'], 'P002 Sim=10, stock=0, adj=-5, Final=5',
         lambda r: require(table(r.stdout) == [('P002', 10, 0, -5, 5)]))
for number, (tail, message) in enumerate([
    (['S001', '0'], 'K must'), (['S001', '-1'], 'K must'), (['S001', 'abc'], 'K must'),
    (['S001', '3', 'abc'], 'max_price must'), (['S001', '3', '0'], 'max_price must'),
    (['--brand'], 'missing value'), (['--unknown'], 'unknown option'),
    (['S001', '2147483648'], 'K must'),
    (['S001', '3', '9223372036854775808'], 'max_price must'),
    (['--brand', 'Nike', '--brand', 'Nike'], 'repeated option'),
], 12):
    run_case(f'TC{number:02}', 'Invalid CLI ' + ' '.join(tail), ['data/sample', 'P001', *tail],
             'Exit 1; clear error containing ' + message, lambda r, msg=message: require(msg in r.stderr), code=1)
run_case('TC22', 'Required arguments missing', [], 'Exit 1; usage printed',
         lambda r: require('Usage:' in r.stderr), code=1)
run_case('TC23', 'Missing input files', ['tests/fixtures/no-such-directory', 'P001'],
         'Exit 1; cannot open products.csv', lambda r: require('cannot open' in r.stderr), code=1)
run_case('TC24', 'Brand filter and bounded K', ['data/full', 'P001', 'S001', '100', '--brand', 'Nike'],
         'All ranked candidates are Nike; Top-K limited to candidate count',
         lambda r: (match_reference(r, 'data/full', 'P001', brand='Nike'),
                    require('TOP ' + str(len(table(r.stdout))) + ' SIMILAR PRODUCTS' in r.stdout)))
run_case('TC25', 'Filter leaves no candidates', ['data/sample', 'P001', '--brand', 'Unknown'],
         'Exit 0; no candidates; Top 0 and best-store table still present',
         lambda r: (require('TOP 0 SIMILAR PRODUCTS' in r.stdout), require('BEST SUBSTITUTE BY STORE' in r.stdout)))
run_case('TC26', 'Max-price sample from brief', ['data/full', 'P001', 'S002', '3', '4000000'],
         '8 excluded; best S001 P039, S002 P003, S003 P003',
         lambda r: (match_reference(r, 'data/full', 'P001', 'S002', 4000000),
                    require('Excluded (above max price 4000000): 8 products.' in r.stdout),
                    require('S001 | P039' in r.stdout), require('S002 | P003' in r.stdout),
                    require('S003 | P003' in r.stdout)))
export = 'tests/fixtures/top.csv'
run_case('TC27', 'Optional CSV export', ['data/sample', 'P001', '--export', export],
         'Header rank,store_id,product_id,sim,stock,final and exactly 3 correct rows',
         lambda r: require((ROOT / export).read_text() ==
             'rank,store_id,product_id,sim,stock,final\n1,S001,P005,7,12,8\n2,S001,P002,7,2,7\n3,S001,P003,10,0,5\n'))
run_case('TC28', 'Export path cannot open', ['data/sample', 'P001', '--export', 'tests/fixtures/missing/out.csv'],
         'Exit 1; explicit export open error', lambda r: require('cannot open export file' in r.stderr), code=1)
limits = fixture('limits', 'P001,Large,C,B,9223372036854775807,5.0\nP002,Zero,C,B,0,5.0\nP003,Near,C,B,9223372036854775806,4.8\n',
                 'S001,P001,3\nS001,P002,2\nS001,P003,3\n')
run_case('TC29', 'long long extremes without multiplication overflow', [limits, 'P001'],
         'P003 Sim=10 Final=11; P002 Sim=8 Final=8',
         lambda r: require(table(r.stdout) == [('P003', 10, 3, 1, 11), ('P002', 8, 2, 0, 8)]))
crlf = fixture('crlf', 'P001,Query,C,B,100,4.0\nP002,Other,C,B,100,4.0\n', 'S001,P001,3\nS001,P002,2\n')
for path in (ROOT / crlf).glob('*.csv'):
    path.write_bytes(path.read_bytes().replace(b'\n', b'\r\n'))
run_case('TC30', 'Windows CRLF input', [crlf, 'P001'], 'P002 Sim=10 stock=2 adj=0 Final=10',
         lambda r: require(table(r.stdout) == [('P002', 10, 2, 0, 10)]))
run_case('TC31', 'Unknown store treated as all zero stock', ['data/sample', 'P001', 'S999', '40'],
         'Every candidate stock=0; known-store report still ranks S001',
         lambda r: (match_reference(r, 'data/sample', 'P001', 'S999'),
                    require(all(row[2] == 0 for row in table(r.stdout)))))

# Exercise all 40 valid queries against all 3 stores using the same black-box entry point.
all_ok = True
summary = []
with (ROOT / 'data/full/products.csv').open() as source:
    ids = [row[0] for row in list(csv.reader(source))[1:41]]
for query in ids:
    for store in ['S001', 'S002', 'S003']:
        result = subprocess.run(['bin/p5', 'data/full', query, store, '40'], cwd=ROOT, capture_output=True, text=True)
        expected_rows = [tuple(row[:5]) for row in reference('data/full', query, store)]
        passed = result.returncode == 0 and table(result.stdout) == expected_rows
        all_ok = all_ok and passed
        summary.append(f'{query} {store}: {"Pass" if passed else "Fail"}')
(EXPECTED / 'TC32.txt').write_text('120 query/store combinations match the independent ranking oracle.\n')
(ACTUAL / 'TC32.txt').write_text('\n'.join(summary) + '\n')
records.append(dict(id='TC32', scenario='All valid full queries / all stores', command='python3 tests/verify.py',
                    expected='120 query/store combinations match independent exact-arithmetic oracle',
                    actual='[output](actual/TC32.txt)', status='Pass' if all_ok else 'Fail', detail='120 combinations'))
print('TC32', records[-1]['status'])

(ROOT / 'tests/results.json').write_text(json.dumps(records, indent=2) + '\n')
lines = ['# Acceptance tests', '',
         'Run `bash tests/run_tests.sh` from a fresh clone. Generated fixtures live in ignored `tests/fixtures/`.',
         'Expected files contain acceptance criteria derived independently of program output. Actual files capture the executed program.',
         'TC01–TC07 map to section 7 of P5. Generic not-found, invalid, empty and zero-denominator categories are also covered by TC08–TC10.',
         '', '| ID | Scenario | Input (command) | Expected | Actual | Pass/Fail |', '|---|---|---|---|---|---|']
for record in records:
    lines.append('| ' + ' | '.join([record['id'], record['scenario'], '`' + record['command'] + '`',
                 record['expected'].replace('|', '/'), record['actual'], record['status']]) + ' |')
lines += ['', '## Failure policy', '', 'Retain failed evidence and record its cause and corrective PR when applicable. TC26 initially failed because its manually authored expectation incorrectly used the sample winner P005 for full data. Full-data P039 has the same Final and Sim but a higher rating, so it wins at S001. The expectation was corrected against the independent oracle. Evidence is in `history/TC26-initial-output.txt`; no C++ change was needed. This correction is included in the initial direct-push commit.',
          '', '## Data integrity', '', 'Official CSV bytes are preserved. The fixtures for zero price, missing stock, large integers and CRLF are generated separately.',
          'Original CSV SHA-256 checksums are listed in `data/SHA256SUMS`.']
(ROOT / 'tests/test_cases.md').write_text('\n'.join(lines) + '\n')
failures = sum(record['status'] != 'Pass' for record in records)
print(f'Acceptance cases: {len(records) - failures}/{len(records)} passed; exhaustive ranking combinations: {sum("Pass" in row for row in summary)}/120')
raise SystemExit(bool(failures))
