# T13 – parseArguments(argc, argv, options)

Input: argc, argv, options.

Output: bool; validated options.

![Flowchart parseArguments](T13-parseArguments.drawio.png)

[Editable diagram](T13-parseArguments.drawio)

Pseudocode:

```text
START parseArguments(argc, argv, options)
    IF argc < 3 THEN
        print usage
        RETURN false
    END IF
    set folder and productId; positional = empty; i = 3
    WHILE i < argc
        IF argument is --brand or --export THEN
            IF missing, empty or option-like value THEN
                print missing-value error
                RETURN false
            END IF
            IF option already supplied THEN
                print duplicate error
                RETURN false
            END IF
            consume value; set option flag and value
        ELSE
            IF argument begins with -- THEN
                print unknown-option error
                RETURN false
            ELSE
                positional.push_back(argument)
            END IF
        END IF
        ++i
    END WHILE
    IF positional.size() > 3 THEN
        print argument-count error
        RETURN false
    END IF
    read optional storeId
    IF K supplied THEN
        CALL parseUnsigned(K, INT_MAX, number)
        IF invalid or number == 0 THEN
            print K error
            RETURN false
        END IF
        options.k = number
    END IF
    IF maxPrice supplied THEN
        CALL parseUnsigned(maxPrice, LLONG_MAX, number)
        IF invalid or number == 0 THEN
            print maxPrice error
            RETURN false
        END IF
        set maxPrice and hasMaxPrice
    END IF
    RETURN true
END
```

Integer `/` means integer division for integer operands. Skipping a record continues the current loop.
