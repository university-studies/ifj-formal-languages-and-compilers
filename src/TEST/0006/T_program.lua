function main()
    local a;
    local b;

    -- 0.05
    write(2.10 % 2.05, "\n");
    -- -2
    a = 0 - 2.05;
    write(2.10 % a, "\n");
    -- -0.05
    b = 0 - 2.10;
    write(b % a, "\n");
    -- 2
    write(b % 2.05, "\n");
end;
