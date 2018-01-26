
local entity = {

    -- db property
    property = {
        id = 0,
        name = "''",
        hp = 100,
        lv = 1,
        loginTime = 0,
    },

    -- 用于标记需要更新到数据库的字段
    dirty = {

    },

    create_sql = function() end,
    init = function(stream) end,
    update = function() end,    

    setProperty = function(type, val) end,
}

entity.create_sql = function(self)
    _sql = string.format("insert into Entity (id,name,hp,lv,loginTime) values (%d,%s,%d,%d,%d);"
        , self.property.id
        , self.property.name
        , self.property.hp
        , self.property.lv
        , os.time())
    
    return _sql
end

entity.init = function(self, stream)

    -- 从数据库请求数据
    

end

entity.setProperty = function(self, type, val)

    if self.property[type] == nil then
        -- warning
        return
    end
    
    self.property[type] = val
    self.dirty[type] = val 
end


entity.update = function(self)

    -- 如果空则跳过
    if next(self.dirty) == nil then
        return
    end

    it = {"entity"}

    for k, v in pairs(self.dirty) do
        table.insert(it, k)
        table.insert(it, v)
    end

    self.dirty = {}

    rpc(eid.distributed.mysql_update, module_id, it, nil)

    -- 更新到数据库 
end


return entity