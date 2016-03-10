clc
%clear all
%DISTANCES BETWEEN TOP PTS
distOP= 3850;%//ljusg15570;//crf628;//origo -> første  (o) O------P  (x)
distPQ= 3752;%//ljusg15980;//crf600;//første til anden      \  R /
distOQ= 3300;%//ljusg25640;//crf437;//anden til origo         \  /
%//DISTANCES TO RUBE                                          (y)   Q
distOR =2600;%//20750;
distPR =2370;%//16450; //1444 er højden fra gulv sjuss
distQR =2456;%//21520;(y)   Q

center1 = [0 0 0];
center2 = [distOP 0 1000];
angleQOP = acos((distOP^2+distOQ^2-distPQ^2)/(2*distOP*distOQ));
angleQOY = pi/2 - angleQOP;
center3 = [sin(angleQOY)*distOQ cos(distOQ)*distOQ 0];

[p_12_a,p_12_b] = threeCirclesIntersection_noHelpFromMatlab(center1,center2,center3,distOR,distPR,distQR);

figure(1)
clf
hold on
axis equal


n=100;
[xUnit,yUnit,zUnit] = sphere(n); 
sphere1X = distOR*xUnit + center1(1)*ones(n+1,n+1);
sphere1Y = distOR*yUnit + center1(2)*ones(n+1,n+1);
sphere1Z = distOR*zUnit + center1(3)*ones(n+1,n+1);

sphere2X = distPR*xUnit + center2(1)*ones(n+1,n+1);
sphere2Y = distPR*yUnit + center2(2)*ones(n+1,n+1);
sphere2Z = distPR*zUnit + center2(3)*ones(n+1,n+1);

sphere3X = distQR*xUnit + center3(1)*ones(n+1,n+1);
sphere3Y = distQR*yUnit + center3(2)*ones(n+1,n+1);
sphere3Z = distQR*zUnit + center3(3)*ones(n+1,n+1);

mesh(sphere1X,sphere1Y,sphere1Z);
mesh(sphere2X,sphere2Y,sphere2Z);
mesh(sphere3X,sphere3Y,sphere3Z);

% plot3(sphere1X,sphere1Y,sphere1Z,'k');
% plot3(sphere2X,sphere2Y,sphere2Z,'k');
% plot3(sphere3X,sphere3Y,sphere3Z,'k');
plot3(p_12_a(1),p_12_a(2),p_12_a(3),'*');
plot3(p_12_b(1),p_12_b(2),p_12_b(3),'*');
