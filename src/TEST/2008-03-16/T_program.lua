function main()
  local a;
  local s;

  a = 1;
  s = sort(a);
  a = type(s);
  if a == "nil" then
      write("OK\n");
  end;
end;
