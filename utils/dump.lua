function dump ( t )  
    local print_r_cache={}
    local function sub_print_r(t,indent)
        if (print_r_cache[tostring(t)]) then
            print(indent.."*"..tostring(t))
        else
            print_r_cache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
                        print(indent.."["..pos.."] => "..tostring(t).." {")
                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
                        print(indent..string.rep(" ",string.len(pos)+6).."}")
                    elseif (type(val)=="string") then
                        print(indent.."["..pos..'] => "'..val..'"')
                    else
                        print(indent.."["..pos.."] => "..tostring(val))
                    end
                end
            else
                print(indent..tostring(t))
            end
        end
    end
    if (type(t)=="table") then
        print(tostring(t).." {")
        sub_print_r(t,"  ")
        print("}")
    else
        sub_print_r(t,"  ")
    end
    print()
end

function dumpStr(t)
    local _str = ""
    local print_r_cache={}
    local function sub_print_r(t,indent)
        if (print_r_cache[tostring(t)]) then
            _str = _str .. indent.."*"..tostring(t) .. '\n'
        else
            print_r_cache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
                        _str = _str .. indent.."["..pos.."] => "..tostring(t).." {\n"
                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
                        _str = _str .. indent..string.rep(" ",string.len(pos)+6).."}\n"
                    elseif (type(val)=="string") then
                        _str = _str .. indent.."["..pos..'] => "'..val..'"\n'
                    else
                        _str = _str .. indent.."["..pos.."] => "..tostring(val) .. '\n'
                    end
                end
            else
                _str = _str .. indent..tostring(t) .. '\n'
            end
        end
    end
    if (type(t)=="table") then
        _str = _str .. tostring(t).." {\n"
        sub_print_r(t,"  ")
        _str = _str .. "}\n"
    else
        sub_print_r(t,"")
    end

    return _str
end