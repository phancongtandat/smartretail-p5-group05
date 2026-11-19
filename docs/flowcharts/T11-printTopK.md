# T11 – printTopK(matches, inventory, options)

Input: matches, inventory, options.

Output: Top-K, reasons, other stores, demotion notes.

![Flowchart printTopK](T11-printTopK.drawio.png)

[Editable diagram](T11-printTopK.drawio)

Pseudocode:

```text
START printTopK(matches, inventory, options)
    count = smaller of options.k and matches.size(); print heading; i = 0
    WHILE i < count
        print rank, product, finalScore and stock label; print reasons
        IF matches[i].stock == 0 THEN
            CALL availableElsewhere(inventory, storeId, product.id)
            print other stores
        END IF
        ++i
    END WHILE
    highestSimilarity = -1
    WHILE another match
        IF match.sim > highestSimilarity THEN
            highestSimilarity = match.sim
        END IF
    END WHILE
    WHILE another index i in matches
        IF matches[i].stock == 0 && matches[i].sim == highestSimilarity THEN
            demoted = false; j = 0
            WHILE j < i
                IF matches[j].sim < matches[i].sim THEN
                    demoted = true
                END IF
                ++j
            END WHILE
            IF demoted THEN
                print most-similar product and its reduced rank
            END IF
        END IF
    END WHILE
    RETURN void
END
```

Integer `/` means integer division for integer operands. Skipping a record continues the current loop.
