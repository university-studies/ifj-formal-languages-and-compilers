--[[
    © Vojtech Simetka
    2011    

    pocita urcity integral obdelnikovou metodou
    ocekavany vstup:
    
        integral
        from:
        [dolni mez] 
        to:
        [horni mez]
        step:
        [velikost kroku]
        function:
        [vstupni funkce]
        .
        .
        .
        end
    
    kde
        [dolni mez] .. nezaporne cislo
        [horni mez] .. nezaporne cislo > [dolni mez]
        [velikost kroku] .. nezaporne cislo, urcuje v jakych krocich integral pocitame
        [vstupni funkce] .. funkce majici smysl, umi operace +, -, neznamou x a libovolne nezaporne cisla
]]

function nacti(func)
    local tmp_str;
    local counter=0;
    local input;
    local nedokoncil = true;
    local previous;
    local typ;
    
    --prevadi nactenou funkci na mezivysledek
    while nedokoncil do
        counter = counter + 1;  
        input = substr(func,counter,counter);
        tmp_str = substr(func,1,counter);
        --write(input, " tmp: ", tmp_str, "\n");
        
        -- nacteni x
        if input=="x" then
            if previous == nil then
                return input;
            else
                return "chyba";
            end;
        else
        
            -- nacteni cisel
            typ = type(previous);
            if input == "0" then
                if previous == nil then
                    previous = 0;
                else
                    if typ == "number" then
                        previous = previous * 10;
                    else
                        return "chyba";
                    end;
                end;
            else
                
                if input == "1" then
                    if previous == nil then
                        previous = 1;
                    else
                        if typ == "number" then
                            previous = previous * 10 + 1;
                        else
                            return "chyba";
                        end;
                    end;
                else
                    if input == "2" then
                        if previous == nil then
                            previous = 2;
                        else
                            if typ == "number" then
                                previous = previous * 10 + 2;
                            else
                                return "chyba";
                            end;
                        end;
                    else
                        if input == "3" then
                            if previous == nil then
                                previous = 3;
                            else
                                if typ == "number" then
                                    previous = previous * 10 + 3;
                                else
                                    return "chyba";
                                end;
                            end;
                        else    
                            if input == "4" then
                                if previous == nil then
                                    previous = 4;
                                else
                                    if typ == "number" then
                                        previous = previous * 10 + 4;
                                    else
                                        return "chyba";
                                    end;
                                end;
                            else
                                if input == "5" then
                                    if previous == nil then
                                        previous = 5;
                                    else
                                        if typ == "number" then
                                            previous = previous * 10 + 5;
                                        else
                                            return "chyba";
                                        end;
                                    end;
                                else
                                    if input == "6" then
                                        if previous == nil then
                                            previous = 6;
                                        else
                                            if typ == "number" then
                                                previous = previous * 10 + 6;
                                            else
                                                return "chyba";
                                            end;
                                        end;
                                    else
                                        if input == "7" then
                                            if previous == nil then
                                                previous = 7;
                                            else
                                                if typ == "number" then
                                                    previous = previous * 10 + 7;
                                                else
                                                    return "chyba";
                                                end;
                                            end;
                                        else
                                            if input == "8" then
                                                if previous == nil then
                                                    previous = 8;
                                                else
                                                    if typ == "number" then
                                                        previous = previous * 10 + 8;
                                                    else
                                                        return "chyba";
                                                    end;
                                                end;
                                            else
                                                if input == "9" then
                                                    if previous == nil then
                                                        previous = 9;
                                                    else
                                                        if typ == "number" then
                                                            previous = previous * 10 + 9;
                                                        else
                                                            return "chyba";
                                                        end;
                                                    end;
                                                else
                                                    if input == "+" then
                                                        if previous == nil then
                                                            return "+";
                                                        else
                                                            nedokoncil = false;
                                                        end;
                                                    else
                                                        if input == "-" then
                                                            if previous == nil then
                                                                return "-";
                                                            else
                                                                nedokoncil = false;
                                                            end;
                                                        else
                                                            if input == "*" then
                                                                if previous == nil then
                                                                    return "*";
                                                                else
                                                                    nedokoncil = false;
                                                                end;
                                                            else
                                                                if input == "/" then
                                                                    if previous == nil then
                                                                        return "/";
                                                                    else
                                                                        nedokoncil = false;
                                                                    end;
                                                                else
                                                                    if input == "^" then
                                                                        if previous == nil then
                                                                            return "^";
                                                                        else
                                                                            nedokoncil = false;
                                                                        end;
                                                                    else
                                                                        if input == ";" then
                                                                            if previous == nil then
                                                                                return ";";
                                                                            else
                                                                                nedokoncil = false;
                                                                            end;
                                                                        else
                                                                            if input == " " then
                                                                                -- preskakovani mezer
                                                                            else
                                                                                return "chyba";
                                                                            end; -- " "
                                                                        end; -- ;
                                                                    end; -- ^
                                                                end; -- /
                                                            end; -- *
                                                        end; -- -
                                                    end; -- +
                                                end; --9
                                            end; --8
                                        end; --7
                                    end; --6
                                end; --5
                            end; --4
                        end; --3
                    end; --2
                end; --1
            end; --0;
        end; -- x
    end; -- while
    return previous;
end

function preskakuj_k_operatoru(func)
    local input;
    local tmp_str;
    local length;
    local counter = 0;
    local not_operator = true;
    
    length = 0-1;
    
    input = substr(func,1,1);
    func = substr(func,2,length);
    
    while not_operator do
        if input ~= "+" then
            if input ~= "-" then
                if input ~= "*" then
                    if input ~= "/" then
                        if input ~= "^" then
                            if input ~= ";" then
                                counter = counter + 1;
                                input = substr(func,1,1);
                                func = substr(func,2,length);
                            else
                                not_operator = false;
                            end;
                        else
                            not_operator = false;
                        end;
                    else
                        not_operator = false;
                    end;
                else
                    not_operator = false;
                end;
            else
                not_operator = false;
            end;
        else
            not_operator = false;
        end;                    
    end;
    func = input..func;
    return func;
end

--rekurzivni mocnina
function mocnina(func,prvni_cislo,x)
    local dalsi_operace;
    local length;
    local druhe_cislo;
    
    length = 0-1;
    --write("\n\n",func,"\n\n");
    
    druhe_cislo = nacti(func);
    
    if druhe_cislo == "x" then
        druhe_cislo = x;
    else
    end;
    
    func = preskakuj_k_operatoru(func);
    
    dalsi_operace = substr(func,1,1);
    func = substr(func,2,length);
    
    --write("\nZbyvajici funkce: ",func," rekonstrukce zbytku: ",prvni_cislo, "^",druhe_cislo,dalsi_operace);
    --return 1;
    if dalsi_operace == "^" then
        prvni_cislo = mocnina(func,prvni_cislo,x);
        return prvni_cislo ^ druhe_cislo;
    else
        return prvni_cislo ^ druhe_cislo;
    end;
end

-- Preskakuje az do operatoru s nizsi prioritou nez ^
function mocnina_retezec(func)
    local dalsi_operace;
    local length;
    
    length = 0-1;
    
    while true do
        func = substr(func,2,length);
        func = preskakuj_k_operatoru(func);
        dalsi_operace = substr(func,1,1);
        
        if dalsi_operace ~= "^" then
            return func;
        else
        end;
    end;
end

--rekurzivni nasobeni a deleni
function nasob_del(func,prvni_cislo,x,operace)
    local dalsi_operace;
    local length;
    local druhe_cislo;
    
    length = 0-1;
    
    func = substr(func,2,length);
    
    druhe_cislo = nacti(func);
    
    if druhe_cislo == "x" then
        druhe_cislo = x;
    else
    end;
    
    func = preskakuj_k_operatoru(func);
    
    dalsi_operace = substr(func,1,1);
    
    if operace == "*" then
        if dalsi_operace == "*" then
            prvni_cislo = prvni_cislo * druhe_cislo;
        else
            if dalsi_operace == "/" then
                prvni_cislo = prvni_cislo * druhe_cislo;
            else
                if dalsi_operace == "^" then    
                    --zavolame mocninu
                else
                    return prvni_cislo * druhe_cislo;
                end;
            end;
        end;
    else
        if druhe_cislo == 0 then
            return "Nulou nepodelis";
        else
            if dalsi_operace == "*" then
                prvni_cislo = prvni_cislo / druhe_cislo;
            else
                if dalsi_operace == "/" then
                    prvni_cislo = prvni_cislo / druhe_cislo;
                else
                    if dalsi_operace == "^" then    
                        --zavolame mocninu
                    else
                        return prvni_cislo / druhe_cislo;
                    end;
                end;
            end;
        end;
    end;
    prvni_cislo = nasob_del(func, prvni_cislo,x,dalsi_operace);
    return prvni_cislo;
end

-- Preskakuje az do operatoru s nizsi prioritou nez * a /
function nasob_del_retezec(func,operace)
    local dalsi_operace;
    local length;
    
    length = 0-1;
    
    while true do
        func = substr(func,2,length);
        func = preskakuj_k_operatoru(func);
        dalsi_operace = substr(func,1,1);
        
        if dalsi_operace ~= "*" then
            if dalsi_operace ~= "/" then
                if dalsi_operace ~= "^" then
                    return func;
                else
                end;
            else
            end;
        else
        end;
    end;
end

function zpracuj_fce(func,min,max,step)
    local typ;
    local vstup;
    local cyklus = 0;
    local saved;
    local length;
    local prvni_cislo;
    local druhe_cislo;
    local saved_vstup;
    local dalsi_operator;
    local x;
    local vysledek=0;
    
    length = 0-1;
    
    saved = func;
    
    vstup = nacti(func);
    saved_vstup = vstup;
    typ = type(vstup);

    while min + step * cyklus < max do
        typ = type(vstup);
        x = min + cyklus * step;
        
        if typ == "number" then
            prvni_cislo = vstup;
            func = preskakuj_k_operatoru(func);
            vstup = nacti(func);
            typ = nil;
        else
            if vstup == "x" then
                prvni_cislo = x;
                func = preskakuj_k_operatoru(func);
                vstup = nacti(func);
                typ = nil;
            else
            
                -- Scitani
                if vstup == "+" then
                    if prvni_cislo == nil then
                        return "chyba10";
                    else
                        func = substr(func,2,length);
                        
                        druhe_cislo = nacti(func);
                        func = preskakuj_k_operatoru(func);
                        
                        dalsi_operator = substr(func,1,1);
                        
                        if dalsi_operator == "*" then
                            druhe_cislo = nasob_del(func, druhe_cislo,x,"*");
                            func = nasob_del_retezec(func,"*");
                            
                            --write("\n\n\nmaaaam",func,"\n\n");
                            
                            vstup = nacti(func);
                            
                            prvni_cislo = prvni_cislo + druhe_cislo;
                        else
                            if dalsi_operator == "/" then
                                if x == 0 then
                                    x = 0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001;
                                else
                                end;
                                
                                druhe_cislo = nasob_del(func, druhe_cislo,x,"/");
                                func = nasob_del_retezec(func,"/");
                                
                                vstup = nacti(func);
                                
                                prvni_cislo = prvni_cislo + druhe_cislo;
                            else
                                if dalsi_operator == "^" then
                                    --druhe_cislo = umocnuj(func, druhe_cislo);
                                    --func = umocnuj(func);
                                    
                                    prvni_cislo = prvni_cislo + druhe_cislo;
                                else
                                    typ = type(druhe_cislo);
                        
                                    if typ == "number" then
                                        prvni_cislo = prvni_cislo + druhe_cislo;
                                        typ = nil;
                                        vstup = dalsi_operator;
                                    else
                                        if druhe_cislo == "x" then
                                            prvni_cislo = prvni_cislo + x;
                                            typ = nil;
                                            vstup = dalsi_operator;
                                        else
                                            return "chyba11";
                                        end;
                                    end;
                                end;
                            end;
                        end;
                    end;
                else
                
                    -- Odcitani
                    if vstup == "-" then
                        if prvni_cislo == nil then
                            return "chyba10";
                        else
                            func = substr(func,2,length);
                            
                            druhe_cislo = nacti(func);
                            func = preskakuj_k_operatoru(func);
                            
                            dalsi_operator = substr(func,1,1);
                            
                            if dalsi_operator == "*" then
                                druhe_cislo = nasob_del(func, druhe_cislo,x,"*");
                                func = nasob_del_retezec(func,"*");
                                
                                vstup = nacti(func);
                                
                                prvni_cislo = prvni_cislo - druhe_cislo;
                            else
                                if dalsi_operator == "/" then
                                    druhe_cislo = nasob_del(func, druhe_cislo,x,"/");
                                    func = nasob_del_retezec(func,"/");
                                    
                                    vstup = nacti(func);
                                    
                                    prvni_cislo = prvni_cislo - druhe_cislo;
                                else
                                    if dalsi_operator == "^" then
                                        --druhe_cislo = umocnuj(func, druhe_cislo);
                                        --func = umocnuj(func);
                                        
                                        prvni_cislo = prvni_cislo - druhe_cislo;
                                    else
                                        typ = type(druhe_cislo);
                            
                                        if typ == "number" then
                                            prvni_cislo = prvni_cislo - druhe_cislo;
                                            typ = nil;
                                            vstup = dalsi_operator;
                                        else
                                            if druhe_cislo == "x" then
                                                prvni_cislo = prvni_cislo - x;
                                                typ = nil;
                                                vstup = dalsi_operator;
                                            else
                                                return "chyba11";
                                            end;
                                        end;
                                    end;
                                end;
                            end;
                        end;
                    else
                    
                        -- Nasobeni
                        if vstup == "*" then
                            if prvni_cislo == nil then
                                return "chyba10";
                            else
                                func = substr(func,2,length);
                                
                                druhe_cislo = nacti(func);
                                func = preskakuj_k_operatoru(func);
                                
                                dalsi_operator = substr(func,1,1);
                                
                                if dalsi_operator == "^" then
                                    --druhe_cislo = umocnuj(func, druhe_cislo);
                                    --func = umocnuj(func);
                                    
                                    prvni_cislo = prvni_cislo * druhe_cislo;
                                else
                                    typ = type(druhe_cislo);
                        
                                    if typ == "number" then
                                        prvni_cislo = prvni_cislo * druhe_cislo;
                                        typ = nil;
                                        vstup = dalsi_operator;
                                    else
                                        if druhe_cislo == "x" then
                                            prvni_cislo = prvni_cislo * x;
                                            typ = nil;
                                            vstup = dalsi_operator;
                                        else
                                            return "chyba11";
                                        end;
                                    end;
                                end;
                            end;
                        else
                        
                            -- Deleni
                            if vstup == "/" then
                                if prvni_cislo == nil then
                                    return "chyba10";
                                else
                                    func = substr(func,2,length);
                                    
                                    druhe_cislo = nacti(func);
                                    func = preskakuj_k_operatoru(func);
                                    
                                    dalsi_operator = substr(func,1,1);
                                    
                                    if dalsi_operator == "^" then
                                        --druhe_cislo = umocnuj(func, druhe_cislo);
                                        --func = umocnuj(func);
                                        
                                        prvni_cislo = prvni_cislo / druhe_cislo;
                                    else
                                        typ = type(druhe_cislo);
                            
                                        if typ == "number" then
                                            if druhe_cislo == 0 then
                                                return "Nulou nepodelis";
                                            else
                                                prvni_cislo = prvni_cislo / druhe_cislo;
                                            end;
                                            typ = nil;
                                            vstup = dalsi_operator;
                                        else
                                            if druhe_cislo == "x" then
                                                if x == 0 then
                                                    x = 0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001;
                                                else
                                                end;
                                                prvni_cislo = prvni_cislo / x;
                                                typ = nil;
                                                vstup = dalsi_operator;
                                            else
                                                return "chyba11";
                                            end;
                                        end;
                                    end;
                                end;
                            else
                                if vstup == "^" then
                                    func = substr(func,2,length);
                                    
                                    prvni_cislo = mocnina(func,prvni_cislo,x);
                                    func = mocnina_retezec(func);
                                    
                                    --write(func);
                                    typ = nil;
                                    vstup = nacti(func);
                                else
                                    if vstup == ";" then
                                        --write(vstup, "\n");
                                        vysledek = vysledek + prvni_cislo;
                                        cyklus = cyklus +1;
                                        vstup = saved_vstup;
                                        func = saved;
                                    else
                                    end; -- ;
                                end; -- ^
                            end; -- /
                        end; -- *
                    end; -- -
                end; -- +
            end; -- x
        end; -- cislo
    end;-- pocitani integralu
    return vysledek*step;
end

function main()
    local step;
    local min;
    local max;
    local func;
    local tmp;
    local length;
    
    length = 0-2;
    
    tmp = read("*l");
    if tmp == "integral" then
        tmp = read("*l");
        -- nacitani mezi
        if tmp == "from:" then
            min = read("*n");
        else
            write("spatny vstup1");
            return 6;
        end;
        tmp = read("*l");
        tmp = read("*l");
        if tmp == "to:" then
            max = read("*n");
        else
            write("spatny vstup2");
            return 6;
        end;
        
        -- velikost kroku
        tmp = read("*l");
        tmp = read("*l");
        if tmp == "step:" then
            step = read("*n");
        else
            write("spatny vstup3");
            return 6;
        end;

        -- nacitani funkce
        tmp = read("*l");
        tmp = read("*l");
        if tmp == "function:" then
            func = read("*l");
        else
            write("spatny vstup4");
            return 6;
        end;  
    else
        write("spatny vstup5");
        return 6;
    end;
    
    -- prochazi jedlotlive funkce
    while func ~= "end" do
        func = func..";";
        
        tmp = zpracuj_fce(func,min,max,step);
        if tmp ~= nil then
            func = substr(func,1,length);
            write("Integral ", func," from ",min, " to ", max, "=", tmp,"\n");
        else
        end;
        func = read("*l");
    end;
end;
