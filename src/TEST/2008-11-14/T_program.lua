function main()
local Elektra;
local Barbora;
    Elektra = 4 - 2 - 1;
    write(Elektra);

    Elektra = 40 / 5 / 2;
    write(Elektra);

    Barbora = 40 / 5 / 2;
    write(Barbora);
    
    Elektra = 1 < 2;
    if Elektra then
        write("true\n");
    else
        write("false\n");
    end;
    
    Elektra = 2 == 3;
    if Elektra then
        write("true\n");
    else
        write("false\n");
    end;
end;
