Math = {}

-- round with variable percision (decimals = how many numbers after , to leave)
Math.round_to_decimal = function(num, decimals)
    local multiplier = 10^decimals
    return math.floor(num * multiplier + 0.5) / multiplier
end

return Math