function main()
    local a;
    a = read("*l");
    if a then 
      write(a,"\n");
      a = main("ma sa zahodit", a);
    else
    end;
end;
