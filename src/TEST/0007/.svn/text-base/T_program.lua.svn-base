function my(x, n)
    if n == 0 then
        return 0;
    end;
    return x; --[[ malo by sa pokracovat tymto riadkom
        avsak program okamzite skonci (pravdepodobne
        problem s NOP) ]]
end

function my2(x, n)
    if n == 0 then
        return 0;
    else
        return x;
    end;
end

function main()
    local number = 2;
    local tmp;

    tmp = my2(number, 0);
    write(tmp, "\n");
    tmp = my2(number, 1);
    write(tmp, "\n");

    tmp = my(number, 0);
    write(tmp, "\n");
    tmp = my(number, 1);
    write(tmp, "\n");
end;