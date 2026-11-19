# T02 – loadProducts(path, products)

Input: path, products.

Output: bool; valid products appended.

![Flowchart loadProducts](T02-loadProducts.drawio.png)

[Editable diagram](T02-loadProducts.drawio)

Pseudocode:

```text
START loadProducts(path, products)
    open ifstream input(path)
    IF input opened THEN
    ELSE
        cerr: cannot open path
        RETURN false
    END IF
    IF getline header and splitCsv(header) valid THEN
    ELSE
        cerr: invalid header
        RETURN false
    END IF
    lineNumber = 1
    WHILE getline(input, line)
        ++lineNumber; fields = splitCsv(line)
        IF fields.size() == 6 THEN
            parseUnsigned(price); toTenths(rating)
            IF required fields and numbers valid THEN
                duplicate = false
                WHILE another existing product
                    IF existing.id == fields[0] THEN
                        duplicate = true
                    END IF
                END WHILE
                IF duplicate THEN
                    CALL warn(path, lineNumber, duplicate)
                ELSE
                    assign product fields; products.push_back(product)
                END IF
            ELSE
                CALL warn(path, lineNumber, invalid fields)
            END IF
        ELSE
            CALL warn(path, lineNumber, column count)
        END IF
    END WHILE
    IF input.bad() THEN
        cerr: read error
        RETURN false
    END IF
    RETURN true
END
```

Integer `/` means integer division for integer operands. Skipping a record continues the current loop.
