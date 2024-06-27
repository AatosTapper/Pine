-- main() is the entry point of a Pine application

function setup()
    print("Game starting")
end

function cleanup()
    print("Game closing")
end

function main()
    setup()
    pine_run() -- this blocks the execution until the game loop has finished
    cleanup()
end