function factorial(n)
    local temp_result;
    local decremented_n;
    local result;

    if n < 2 then
        result = 1;
    else
        decremented_n = n - 1;
        temp_result = factorial(decremented_n);
        result = n * temp_result;
    end;
    return result;
end

function main()
    local a;
    local vysl;

    write("Zadejte cislo pro vypocet faktorialu\n");
    a = read("*n");
    if a < 0 then
        write("Faktorial nelze spocitat\n");
    else
        vysl = factorial(a);
        write("Vysledek je: ", vysl, "\n");
    end;
end;
