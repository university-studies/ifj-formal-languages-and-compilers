function main()
    local x1;
    local x2;

    x1 = find("","xxx");

    x2 = type(x1);
    if x2 == "bool" and x2 == false then
        write("OK!");
    end;
end;
