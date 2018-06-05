require "event_list"
require "event_fmt"

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

function deep_copy(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for key, value in pairs(object) do
            new_table[_copy(key)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end

function dispatch(target, event, ...)

    local args = { ... }
    local buf = evpack:pack(event, args)

    self:ldispatch(target, event, buf)
end

function listen(eventID, func)

    function _callback(buf)
        _args = evunpack(buf)
        func(_args)
    end

	self:llisten(eventID, _callback)
end

function rpc(event_id, module_id, buf, callback)

    function _callback(resBuf, len, progress, succ)
        _args = evunpack(resBuf, len)
        callback(_args, progress, succ)
    end
    
    if callback ~= nil then
        self:lrpc(module_id, event_id, module_id, buf, _callback)
    else
        self:lrpc(module_id, event_id, module_id, buf, nil)
    end
end