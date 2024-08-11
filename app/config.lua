config = {
    -- window
    window_width = 1280,
    window_height = 720,
    window_name = "Pine Engine",

    -- camera
    cam_fov = 8,
    cam_projection = 1, -- 0 = 3D, 1 = 2D (ortho)
    cam_start_z = 1, -- how far back the camera starts

    -- engine

    -- gameplay systems update on a specified tick rate
    -- camera updates are fixed but they are interpolated to move smoothly
    -- some things like rendering update at vsync (or display fps)
    tick_rate = 60,
    -- if not 0, allows the engine to lower the tick_rate temporarily when updates can't keep up.
    -- lowering is done in steps of specified size
    -- causes frame pacing issues but keeps responsiveness when having very poor performance
    tick_rate_downscale_step = 10,

    -- used for balancing physics calculations etc,
    -- DOES NOT DOWNSCALE
    tick_rate_reference = 60,
}
