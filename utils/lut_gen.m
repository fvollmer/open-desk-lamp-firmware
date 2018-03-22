result = round(linspace(0.3, 2, 265).^2*100);
fid = fopen("lut.txt", "w");
dlmwrite(fid, result)
fclose(fid)