function v = ThighFilter(x,y,z)
%THIGHFILTER Generates a thigh-shaped filter

% Constants
rT = 1.0;  % radius of thigh
a = 0.4;   % ellipse x stretch
b = 0.8;   % ellipse y stretch
minAbMod = 0.25;

% Add thigh radius to ellipse
abMod = (1.0-minAbMod)*(1.0-z) + minAbMod;
a = a*abMod + rT;
b = b*abMod + rT;

% Convert x,y to polar form
L = sqrt(x*x + y*y);
theta = atan2(y, x);

% Bounds for lerp
minL = rT;
maxL = a*b / sqrt(a^2*sin(theta)^2 + b^2*cos(theta)^2);

% Calculate the value

if L <= minL
    v = 1.0;
elseif L >= maxL
    v = 0.0;
else
    v = 1.0 - (L - rT) / (maxL - rT);
end
