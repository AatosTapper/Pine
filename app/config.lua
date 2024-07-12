config = {
    -- window
    window_width = 1280,
    window_height = 720,
    window_name = "Pine Engine",

    -- camera
    cam_fov = 75,
    cam_projection = 0, -- 0 = 3D, 1 = 2D (ortho)
    cam_start_z = 10, -- how far back the camera starts

    -- engine
    fixed_update_rate = 60, -- gameplay systems update on a fixed update time
                            -- camera updates are fixed but they are interpolated to move smoothly
                            -- some things like rendering update at vsync (or display fps)
}
