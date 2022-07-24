# ARTag-Cluster-Robots
(Hopefully) cluster robots that work together using AprilTags to draw a picture

reading formats for the serial messages: 

Motor command format: (to hardware board from computer)

C:1:510;2:510;E;
1 = left wheel speed, range from -1 to 1 as values from 255 to 765
2 = right wheel speed, same as left 

RGB light command format: (to sensor board from hardware board from computer)

R:1:0;2:255;3:255;4:255;E;

MPU reading format: (to computer from hardware board from sensor board)

M:1:xaxisaccel;2:yaxisaccel;3:zaxisaccel;4:xaxisgyro;5:yaxisgyro;6:zaxisgyro;E;
