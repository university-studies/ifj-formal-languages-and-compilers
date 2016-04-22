function main()
    local a;
    local b;
-- rozsireni za 0.5 bodu
   write(5 % 2);
   write(5 % 3);

   a = 0;
   b = 2;

   a = a - 5;

   -- -5 % 2
   write(a % b);
   -- -5 % -3
   b = b - 5;
   write(a % b);
   -- 5 % -3
   a = a + 10;
   write(a % b);
end;
