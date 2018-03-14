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

function dispatch(target, eventID, buf, callback)

    function _callback(buf, len)
        _args = evunpack(buf, len)
        callback(_args)
    end

    if callback ~= nil then
        event:ldispatch(module_id, target, eventID, buf, _callback)
    else
        event:ldispatch(module_id, target, eventID, buf)
    end
end

function listen(target, eventID, func)

    function _callback(buf, len)
        _args = evunpack(buf, len)
        func(_args)
    end

	event:llisten(module_id, target, eventID, _callback)
end

function rpc(event_id, module_id, buf, callback)

    function _callback(resBuf, len, progress, succ)
        _args = evunpack(resBuf, len)
        callback(_args, progress, succ)
    end
    
    if callback ~= nil then
        event:lrpc(module_id, event_id, module_id, buf, _callback)
    else
        event:lrpc(module_id, event_id, module_id, buf, nil)
    end
end

-- 在使用日志接口前先获取 LogModule
-- 调试日志 --
function DEBUG_LOG(module, reason, ...)

    logContent = "[module] " .. module .. " [reason] " .. reason .. '\n'
    local _args = { ... }

    for k, v in ipairs(_args) do
        logContent = logContent .. dumpStr(v)
    end

    local args = Args.new()
    args:push_ui16(0)
    args:push_string(logContent)
    event:ldispatch(module_id, log_m_, eid.log.print, args:pop_block(0, args:get_size()), nil)
end

-- 诊断日志 --
function INFO_LOG(module, reason, ...)
    logContent = "[module] " .. module .. " [reason] " .. reason .. ' '
    local _args = { ... }

    for k, v in ipairs(_args) do
        _v = v
        if type(v) ~= "string" then
            _v = tostring(v)
        end 

        logContent = logContent .. _v .. ' '
    end

    local args = Args.new()
    args:push_ui16(1)
    args:push_string(logContent)
    event:ldispatch(module_id, log_m_, eid.log.print, args:pop_block(0, args:get_size()), nil)
end

function WARN_LOG(module, reason, ...)
    logContent = "[module] " .. module .. " [reason] " .. reason .. ' '
    local _args = { ... }

    for k, v in ipairs(_args) do
        _v = v
        if type(v) ~= "string" then
            _v = tostring(v)
        end 

        logContent = logContent .. _v .. ' '
    end

    local args = Args.new()
    args:push_ui16(2)
    args:push_string(logContent)
    event:ldispatch(module_id, log_m_, eid.log.print, args:pop_block(0, args:get_size()), nil)
end

function ERR_LOG(module, reason, ...)
    logContent = "[module] " .. module .. " [reason] " .. reason .. ' '
    local _args = { ... }

    for k, v in ipairs(_args) do
        _v = v
        if type(v) ~= "string" then
            _v = tostring(v)
        end 

        logContent = logContent .. _v .. ' '
    end

    local args = Args.new()
    args:push_ui16(3)
    args:push_string(logContent)
    event:ldispatch(module_id, log_m_, eid.log.print, args:pop_block(0, args:get_size()), nil)
end

-- 统计日志 --

function RECORD_LOG(behavior, player, time, ...)

end