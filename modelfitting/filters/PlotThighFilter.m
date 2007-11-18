x = [-2:0.2:2];
y = [-2:0.2:2];
z = [-2:0.2:2];

cx = max(size(x));
cy = max(size(y));
cz = max(size(z));

total = 0;
data = zeros(cx, cy, cz);
for nx = 1:cx
    for ny = 1:cy
        for nz = 1:cz
            d = ThighFilter(x(nx), y(ny), nz/cz);
            total = total + d;
            data(nx, ny, nz) = d;
        end
    end
end

[file, message] = fopen('thigh.filter', 'w', 'ieee-be');
fwrite(file, cx, 'uint8');
fwrite(file, cy, 'uint8');
fwrite(file, cz, 'uint8');

for nx = 1:cx
    for ny = 1:cy
        for nz = 1:cz
            fwrite(file, data(nx, ny, nz) / total, 'float32');
        end
    end
end

fclose(file);

%contour(x,y,data(:,:,1))