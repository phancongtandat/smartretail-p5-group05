# T03 – loadInventory(path, products, inventory)

Input: path, products, inventory.

Output: bool; valid stock records appended.

![Flowchart loadInventory](T03-loadInventory.drawio.png)

[Editable diagram](T03-loadInventory.drawio)

Pseudocode:

```text
START loadInventory(path, products, inventory)
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
        IF 3 nonempty valid fields; stock in 0..INT_MAX THEN
            CALL findProduct(products, fields[1], 0, size-1)
            IF product exists THEN
                CALL stockOf(inventory, fields[0], fields[1])
                IF duplicate key found THEN
                    CALL warn(path, lineNumber, duplicate key)
                ELSE
                    inventory.push_back(valid record)
                END IF
            ELSE
                CALL warn(path, lineNumber, unknown product)
            END IF
        ELSE
            CALL warn(path, lineNumber, invalid row)
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
