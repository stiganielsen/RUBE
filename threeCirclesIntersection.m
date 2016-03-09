% Find the intersection of three spheres
% P1,P2,P3 are the centers, r1,r2,r3 are the radii
% Implementaton based on Wikipedia Trilateration article.
% http://stackoverflow.com/questions/1406375/finding-intersection-points-between-3-spheres
function [p_12_a,p_12_b] = threeCirclesIntersection(P1,P2,P3,r1,r2,r3)
temp1 = P2-P1;
e_x = temp1/norm(temp1);
temp2 = P3-P1;
i = dot(e_x,temp2);
temp3 = temp2 - i*e_x;
e_y = temp3/norm(temp3);
e_z = cross(e_x,e_y);
d = norm(P2-P1);
j = dot(e_y,temp2);
x = (r1*r1 - r2*r2 + d*d) / (2*d);
y = (r1*r1 - r3*r3 -2*i*x + i*i + j*j) / (2*j);
temp4 = r1*r1 - x*x - y*y;
if temp4<0
    error('The three spheres do not intersect!');
end
z = sqrt(temp4);
p_12_a = P1 + x*e_x + y*e_y + z*e_z;
p_12_b = P1 + x*e_x + y*e_y - z*e_z;
end