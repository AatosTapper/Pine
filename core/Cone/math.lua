Math = {}

--- Round to specied precision
---@param num number
---@param decimals number
Math.round_to_decimal = function(num, decimals)
    local multiplier = 10^decimals
    return math.floor(num * multiplier + 0.5) / multiplier
end

return Math