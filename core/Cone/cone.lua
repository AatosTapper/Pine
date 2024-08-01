Cone = {}
Cone.Math = require("core.Cone.math")
Cone.Event = require("core.Cone.event")
Cone.Key = require("core.Cone.keys")

--- Universal "to string", use instead of the lua one (tostring())
--- @param val any
--- @return string
Cone.to_string = function(val)
    if "nil" == type(val) then
        return tostring(nil)
    elseif  "table" == type(val) then
        return table_print(val)
    elseif  "string" == type(val) then
        return val
    else
        return tostring(val)
    end
end

--[[    enum

MyEnum = Cone.enum { "Key1", "Key2", ... }
some_var = MyEnum.Key1
]]
Cone.enum = function(keys)
    local Enum = {}
    for _, value in ipairs(keys) do
        Enum[value] = {}
    end
    return Enum
end

function table_print(tt, indent, done, is_first)
    return table_serialize(tt, index, done, is_first):sub(1, -2)
end

--- slow as fuck
function table_serialize(tt, indent, done, is_first)
    is_first = is_first or true
    done = done or {}
    indent = indent or 0
    if type(tt) == "table" then
        local sb = {}
        table.insert(sb, "{\n");
        for key, value in pairs(tt) do
        table.insert(sb, string.rep(" ", indent)) -- indent it
        if type(value) == "table" and not done[value] then
            done[value] = true
            table.insert(sb, key .. " = ");
            table.insert(sb, table_serialize(value, indent + 2, done))
            table.insert(sb, string.rep(" ", indent)) -- indent it
            table.insert(sb, "\n");
        elseif "number" == type(key) then
            table.insert(sb, string.format("\"%s\",\n", tostring(value)))
        else
            table.insert(sb, string.format(
                "%s = \"%s\",\n", tostring(key), tostring(value)))
            end
        end
        table.insert(sb, string.format("%s%s", string.rep(" ", indent - 2), "},"));
        return table.concat(sb)
    else
        return tt .. "\n"
    end
end

--- String to table
---@param str string
---@return table
Cone.string_to_table = function(str)
    local func, err = load("return " .. str)
    if not func then
        return nil, err
    end
    local success, result = pcall(func)
    if not success then
        return nil, result
    end
    return result
end

--- Profile function in microseconds
Cone.profiler = function(func)
    local startTime = os.clock()
    func()
    local endTime = os.clock()
    return (endTime - startTime) * 10^6
end

return Cone