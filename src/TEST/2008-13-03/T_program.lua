function main()
    local sk8er;
    local b;
    sk8er = find("abcabcabcabc","abcd");

    b = type(sk8er);
    if b == "bool" then
        if sk8er == true then
            write(sk8er);
        end;
    end;
end;
