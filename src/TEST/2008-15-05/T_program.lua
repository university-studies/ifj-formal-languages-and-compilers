function main()
local Vysledek;
local a;
local b;
   Vysledek = 0;
   a = 5;
   b = 5;
   while (a > 0) do
      while (b > 0) do
         Vysledek = Vysledek + 1;
         b = b - 1;
     end;
      a = a - 1;
      b = a;
  end;
   write(Vysledek);

end;
