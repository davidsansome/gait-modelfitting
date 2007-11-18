x = [-2:0.2:2];
y = [-2:0.2:2];
z = [-2:0.2:2];

cx = max(size(x));
cy = max(size(y));
cz = max(size(z));

file = fopen('thigh.filter', 'w');
fwrite(file, cx, 'uint8');
fwrite(file, cy, 'uint8');
fwrite(file, cz, 'uint8');

data = zeros(cx, cy, cz);
for nx = 1:cx
    for ny = 1:cy
        for nz = 1:cz
            %data(nx, ny, nz) = ThighFilter(x(nx), y(ny), nz/cz);
            d = ThighFilter(x(nx), y(ny), nz/cz);
            fwrite(file, d, 'float32');
        end
    end
end

fclose(file);

%contour(x,y,data(:,:,1))