Cone = require("core.Cone.cone")
game = require("app.game")

local function setup()
    game.create_main_scene()
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
end