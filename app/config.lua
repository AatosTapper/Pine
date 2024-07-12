config = {
    -- window
    window_width = 1280,
    window_height = 720,
    window_name = "Pine Engine",

    -- camera
    cam_fov = 10,
    cam_projection = 1, -- 0 = 3D, 1 = 2D (ortho)
    cam_start_z = 1, -- how far back the camera starts

    -- engine
    fixed_update_rate = 60, -- gameplay systems update on a fixed update time
                            -- camera updates are fixed but they are interpolated to move smoothly
                            -- some things like rendering update at vsync (or display fps)
}
