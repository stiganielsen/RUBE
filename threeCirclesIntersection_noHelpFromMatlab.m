% Find the intersection of three spheres
% A,B,C are the centers, r1,r2,r3 are the radii
% Implementaton based on Wikipedia Trilateration article.
% http://stackoverflow.com/questions/1406375/finding-intersection-points-between-3-spheres
function [p_12_a,p_12_b] = threeCirclesIntersection_noHelpFromMatlab(A,B,C,r1,r2,r3)
AB = B-A;%x3
absAB = normDIY(AB);
e_x = AB/absAB;%x3
CA = C-A;%x3
i = dotDIY(e_x,CA);
temp3 = CA - i*e_x;%x3
absTemp3 = normDIY(temp3);
e_y = temp3/absTemp3;%x3
e_z = crossDIY(e_x,e_y);
d = normDIY(B-A);
j = dotDIY(e_y,CA);
x = (r1*r1 - r2*r2 + d*d) / (2*d);
y = (r1*r1 - r3*r3 -2*i*x + i*i + j*j) / (2*j);
temp4 = r1*r1 - x*x - y*y;
if temp4<0
    error('The three spheres do not intersect!');
end
z = sqrt(temp4);
p_12_a = A + x*e_x + y*e_y + z*e_z;
p_12_b = A + x*e_x + y*e_y - z*e_z;
end

function length = normDIY(a)
length = sqrt(a(1)*a(1) + a(2)*a(2) + a(3)*a(3));
end

function result = dotDIY(a,b)
result = a(1)*b(1) + a(2)*b(2) + a(3)*b(3);
end

function c = crossDIY(a,b)
c(1) = a(2)*b(3)-a(3)*b(2);
c(2) = a(3)*b(1)-a(1)*b(3);
c(3) = a(1)*b(2)-a(2)*b(1);
end