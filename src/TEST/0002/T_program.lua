function main()
    local a;
    local vysl;

    write("Zadejte cislo pro vypocet faktorialu\n");
    a = read("*n");
    if a < 0 then
        write("Faktorial nelze spocitat\n");
    else
        vysl = 1;
        while a > 0 do
            vysl = vysl * a;
            a = a - 1;
        end;
        write("Vysledek je: ", vysl, "\n");
    end;
end;
