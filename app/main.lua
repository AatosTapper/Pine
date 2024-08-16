Cone = require("core.Cone.cone")
game = require("app.game")
attack_system = require("app.scripts.attack_system")

local function setup()
    game.create_main_scene()
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
end