require "event_list"

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

local at_uint8 = 1
local at_int8 = 2
local at_uint16 = 3
local at_int16 = 4
local at_uint32 = 5
local at_int32 = 6
local at_uint64 = 7
local at_int64 = 8
local at_bool = 9
local at_float = 10
local at_double = 11
local at_string = 12
local at_list = 13
local at_vec = 14
local at_map = 15
local at_eof = 20

-- 后续自动生成
_type_map = {
    [eid.distributed.coordinat_select] = function(args) 
        local pack = Args.new()
        pack:push_string(args[1])
        pack:push_i32(args[2])
        return pack:pop_block(0, pack:get_size())
    end, 
    [eid.distributed.mysql_query] = function(args)
        local pack = Args.new()
        pack:push_i32(args[1])
        pack:push_string(args[2])
        return pack:pop_block(0, pack:get_size())
    end, 
    [eid.distributed.mysql_update] = function(args) 
        local pack = Args.new()
        pack:push_string(args[1])
        pack:push_i32(args[2])
        pack:push_string(args[3])
        pack:push_i32(args[4])
        return pack:pop_block(0, pack:get_size())
    end,
    [eid.timer.delay_milliseconds] = function(args)
        local pack = Args.new()
        pack:push_i32(args[1])
        pack:push_i32(args[2])
        return pack:pop_block(0, pack:get_size())
    end, 
    [eid.node.node_regist] = function(args) 
        local pack = Args.new()
        pack:push_i32(args[1])
        pack:push_i32(args[2])
        pack:push_string(args[3])
        pack:push_i32(args[4])
        return pack:pop_block(0, pack:get_size())
    end, 
    [10001] = function(args) 
        local pack = Args.new()
        pack:push_string(args[1])
        pack:push_i32(args[2])
        pack:push_i32(args[3])
        return pack:pop_block(0, pack:get_size())
    end, 
    [10002] = function(args) 
        local pack = Args.new()
        pack:push_ui16(args[1])
        pack:push_i32(args[2])
        pack:push_string(args[3])
        return pack:pop_block(0, pack:get_size())
    end,
}

function __pack(event_id, args)
    return _type_map[event_id](args)
end

function __unpack(buf, len)
    unpack = Args.new(buf, len)
    
    _args = {}
    _idx = 1
    _tag = unpack:get_tag()
    while(_tag ~= 0) do

        if _tag == at_int32 then
            table.insert(_args, _idx, unpack:pop_i32())
        elseif _tag == at_string then
            table.insert(_args, _idx, unpack:pop_string())
        elseif _tag == at_uint32 then
            table.insert(_args, _idx, unpack:pop_ui32())
        elseif _tag == at_int64 then
            table.insert(_args, _idx, unpack:pop_i64())
        elseif _tag == at_uint64 then
            table.insert(_args, _idx, unpack:pop_ui64())
        elseif _tag == at_bool then
            table.insert(_args, _idx, unpack:pop_bool())
        end
        
        _idx = _idx + 1
        _tag = unpack:get_tag()
    end

    return _args
end

function dispatch(target, eventID, args)
    reqBuf = __pack(eventID, args)

    resBuf = event:ldispatch(module_id, target, eventID, reqBuf)
    if #resBuf == 0 then
        return nil
    else
        return __unpack(resBuf, #resBuf)
    end
end

function __dispatch(target, eventID, args, func)
    return event:ldispatch(module_id, target, eventID, args, func)
end

function listen(target, eventID, func)
	event:llisten(module_id, target, eventID, func)
end

function rpc(event_id, module_id, args, callback)

    function _callback(resBuf, len, progress, succ)
        _args = __unpack(resBuf, len)
        callback(_args, progress, succ)
    end

    reqBuf = __pack(event_id, args)
    
    if callback ~= nil then
        event:lrpc(module_id, event_id, module_id, reqBuf, _callback)
    else
        event:lrpc(module_id, event_id, module_id, reqBuf, nil)
    end
end

function logInfo(moduleName, info)
    local args = Args.new()
    args:push_ui16(2)
    args:push_string(moduleName)
    args:push_string(info)
    __dispatch(log_m_, eid.log.print, args:pop_block(0, args:get_size()))
end

function logWarn(moduleName, warn)
    local args = Args.new()
    args:push_ui16(1)
    args:push_string(moduleName)
    args:push_string(warn)
    __dispatch(log_m_, eid.log.print, args:pop_block(0, args:get_size()))
end

function logError(moduleName, err)
    local args = Args.new()
    args:push_ui16(0)
    args:push_string(moduleName)
    args:push_string(err)
    __dispatch(log_m_, eid.log.print, args:pop_block(0, args:get_size()))
end

function dispatch_createNode(target, nodeID, moduleID, nodeType, acceptor_ip, acceptor_port, rootIp, rootPort, modules)
    local args = Args.new()
    args:push_i32(nodeID)
    args:push_i32(module_id)
    args:push_string(nodeType)

    args:push_string(acceptor_ip)
    args:push_i32(acceptor_port)

    args:push_string(rootIp)
    args:push_i32(rootPort)

    args:push_i32(#modules)
    for i = 1, #modules do
        local mNode = modules[i]
        args:push_string(mNode[1])
        args:push_i32(mNode[2])
        args:push_i32(mNode[3])
    end

    __dispatch(target, eid.node.node_create, args:pop_block(0, args:get_size()))
end