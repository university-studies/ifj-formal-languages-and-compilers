function main()
local Elektra;
    Elektra = 1 + 2 * 3 + 3 * 4;
    if Elektra then
        write("true\n");
    else
        write("false\n");
    end;
    
    Elektra = 3*9 == 7+5*4;
    if Elektra then
        write("true\n");
    else
        write("false\n");
    end;

    Elektra = 21/7 - 1 <= 21/7 + 1;
    if Elektra then
        write("true\n");
    else
        write("false\n");
    end;
    
    Elektra = 1 == 48 == (24 == 3);
    if Elektra then
        write("true\n");
    else
        write("false\n");
    end;

    Elektra = true == true == true;
    if Elektra then
        write("true\n");
    else
        write("false\n");
    end;
end;
