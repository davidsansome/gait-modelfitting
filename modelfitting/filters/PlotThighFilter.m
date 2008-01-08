x = [-2:0.2:2];
y = [-2:0.2:2];
z = [-2:0.2:2];

cx = max(size(x));
cy = max(size(y));
cz = max(size(z));

[file, message] = fopen('thigh.filter', 'w', 'ieee-be');
fwrite(file, cx, 'uint8');
fwrite(file, cy, 'uint8');
fwrite(file, cz, 'uint8');

total = 0;
data = zeros(cx, cy, cz);
for nz = 1:cz
    for ny = 1:cy
        for nx = 1:cx
            d = ThighFilter(x(nx), y(ny), 1 - nz/cz);
            total = total + d;
            data(nx, ny, nz) = d;
            fwrite(file, d, 'float32');
        end
    end
end

fwrite(file, total, 'float32');
fclose(file);

%contour(x,y,data(:,:,20))
%meshz(data(:,:,20))
pcolor(x,y,data(:,:,20))

'Done'