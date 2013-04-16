
* This is csc_geom.inc

      real*4       strip_angle, k3, height, wg_width, r_wg_0
      parameter    (strip_angle = 0.0023271) ! Angle in radians between strip 
                                             ! centers 
      parameter    (k3 = .330)        ! Gatti function parameter
      parameter    (height = 4.7625)  ! Distance from wire plane to cathodes, mm
      parameter    (wg_width = 50.577) ! Width of a wire group, mm
      parameter    (r_wg_0 = 3662.0)  ! Distance to the center of the first 
                                      ! wire group, mm
